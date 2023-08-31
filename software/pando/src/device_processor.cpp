#include "device_processor.h"

#include "correlator.h"
#include "dtof.h"
#include "pando.pb.h"
#include "protobuf_util.h"
#include "reporter.h"
#include "zip_refs.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <exception>
#include <execution>
#include <functional>
#include <future>
#include <map>
#include <numeric>
#include <utility>

namespace pnd {
namespace pando {

DeviceProcessor::DeviceProcessor(std::unique_ptr<DeviceBase> device) : device_{std::move(device)} {
  // For inproc transport, the effective send hwm is ZMQ_SNDHWM plus the ZMQ_RCVHWM of the socket on
  // the other end of the connection (in this case, the ingress port of the proxy).
  pub_socket_.setsockopt(ZMQ_SNDHWM, 1);
  pub_socket_.setsockopt(ZMQ_SNDTIMEO, 0); // Do not block on send
  pub_socket_.setsockopt(ZMQ_XPUB_NODROP, 1); // Do not silently drop messages
  pub_socket_.connect(PandoInterface::kProxyIngressInprocEp);
}

void DeviceProcessor::StartImpl(
    int32_t experiment_id,
    PandoInterface::Config config,
    const char* raw_file_name) {
  config_ = std::move(config);
  experiment_id_ = experiment_id;
  g2_seq_ = 0;
  event_seq_ = 0;

  if (config_.log_raw_data)
    raw_logger_ = std::make_unique<HDF5RawLogger>(raw_file_name);

  device_session_handle_ = device_->Start(config_);

  // Call UpdateRawData with a zero-length time span as a means of blocking until the device
  // acquisition has actually begun.
  auto dummy = device_->CreateDest();
  device_->UpdateRawData(0, 0, dummy);

  // Create a correlator for each enabled channel
  correlators_ = {config_.enabled_channels.cbegin(),
                  config_.enabled_channels.cend(),
                  std::chrono::nanoseconds(config_.bin_size_ns),
                  config_.points_per_level,
                  config_.n_levels,
                  config_.rebin_factor};

  // Reset the TimeShifter
  time_shifter_ = {};

  count_seq_ = 0;
  dtof_seq_ = 0;
  cri_seq_ = 0;
  marker_seq_ = 0;
  pps_seq_ = 0;

  run_thread_ = common::ThreadContainer([&] { Run(); }, "DeviceProcessor::Run", &run_stop_signal_);
}

void DeviceProcessor::StopImpl() {
  run_thread_.Stop();
  run_thread_.Join();

  // release the unique_pointer so the HDF5 file is written out on experiment end.
  raw_logger_.reset();

  device_session_handle_.reset();
}

void DeviceProcessor::Run() {
  size_t begin_bin_idx = 0;
  size_t end_bin_idx = 0;

  // Used for benchmarking
  // std::chrono::steady_clock::time_point tick, tock;

  // We have two instances of RawData. The first one is for passing into UpdateRawData, the second
  // is for passing into Process. This enables UpdateRawData and Process to run concurrently.
  auto raw_data = device_->CreateDest();
  auto raw_data_proc = device_->CreateDest();

  // Calculate the ratio between the final and starting bin sizes
  size_t bin_scale = 1;
  for (int i = 1; i < config_.n_levels; i++) {
    bin_scale *= config_.rebin_factor;
  }

  // If we're relying on PPS for synchronization, discard one integration period worth of raw data
  // at a time until we observe the first pulse.
  if (config_.use_pps) {
    while (!time_shifter_.IsPrimed()) {
      // Time out eventually if no PPS is found
      if (std::chrono::nanoseconds{config_.bin_size_ns} * begin_bin_idx >=
          std::chrono::milliseconds(20100)) {
        throw std::runtime_error(
            "Waited for first PPS pulse for more than 20 seconds. Is a PPS signal connected to marker channel 0?");
      }

      begin_bin_idx = end_bin_idx;
      end_bin_idx += config_.final_bin_count * bin_scale;
      device_->UpdateRawData(begin_bin_idx, end_bin_idx, raw_data);
      HandlePps(raw_data);
    }
  }

  // Create a destination for the dtof for each enabled channel
  auto resolution = MicroTime{device_->GetMicrotimeResolutionPs()};
  auto range_start = MicroTime{config_.dtof_range_min};

  // there is no point in having the DTOF range exceed the sync period
  // because all the points after the sync period will be 0 anyway
  MicroTime range_end;
  if (config_.laser_sync_period_ps >= config_.dtof_range_max) {
    range_end = MicroTime{config_.dtof_range_max};
  } else {
    g_reporter->warn(
        "DTOF Range Max ({}) exceeds XHarp sync period ({}), "
        "so truncating DTOF Range Max to XHarp sync period.",
        config_.dtof_range_max,
        config_.laser_sync_period_ps);
    range_end = MicroTime{config_.laser_sync_period_ps};
  }

  std::chrono::nanoseconds first_integ_periods_start_time{config_.bin_size_ns * begin_bin_idx};

  // TODO implement bin_size as a user-selected parameter.
  // Right now is just equal to the resolution.
  dtofs_ = {config_.enabled_channels.cbegin(),
            config_.enabled_channels.cend(),
            resolution,
            resolution,
            range_start,
            range_end,
            first_integ_periods_start_time};

  // Create counters for each enabled channel
  ts_counters_ = {config_.enabled_channels.cbegin(),
                  config_.enabled_channels.cend(),
                  first_integ_periods_start_time};
  hist_counters_ = {config_.enabled_channels.cbegin(),
                    config_.enabled_channels.cend(),
                    first_integ_periods_start_time};
  cri_counters_ = {config_.enabled_channels.cbegin(),
                   config_.enabled_channels.cend(),
                   first_integ_periods_start_time};

  // send zmq START event
  SendZmqEvent(
      proto::EVENT_START,
      event_seq_++,
      time_shifter_.Shift(first_integ_periods_start_time).count());

  // Process the very first integration period that contained a PPS edge
  if (config_.use_pps) {
    Process(raw_data, config_, begin_bin_idx, end_bin_idx);
  }

  Worker proc_worker("DeviceProcessor::Process");
  BlockingFuture<void> worker_done;

  while (!run_stop_signal_.ShouldStop()) {
    // Update dynamic config (may change while experiment is in progress)
    auto dynamic_config = SingletonPando::Get()->GetConfig();

    begin_bin_idx = end_bin_idx;
    end_bin_idx += dynamic_config.final_bin_count * bin_scale;

    // Convert raw records into timestamps
    // tick = std::chrono::steady_clock::now();
    device_->UpdateRawData(begin_bin_idx, end_bin_idx, raw_data);
    // tock = std::chrono::steady_clock::now();
    // auto UpdateRawData_time = std::chrono::round<std::chrono::microseconds>(tock -
    // tick).count(); g_reporter->debug("UpdateRawData took {} us", UpdateRawData_time);

    // Wait for any previous call to DeviceProcessor::Process to complete
    if (worker_done.valid()) {
      worker_done.get();
    }

    // Process the new raw data
    std::swap(raw_data, raw_data_proc);
    worker_done = proc_worker.Async(
        // Capture raw_data_proc by reference, all else by value
        [this, &raw_data_proc, config = std::move(dynamic_config), begin_bin_idx, end_bin_idx]() {
          if (config_.use_pps) {
            HandlePps(raw_data_proc);
          }
          Process(raw_data_proc, config, begin_bin_idx, end_bin_idx);
        });
  }

  // send zmq STOP event
  std::chrono::nanoseconds last_integ_period_end_time{config_.bin_size_ns * begin_bin_idx};
  SendZmqEvent(
      proto::EVENT_STOP, event_seq_++, time_shifter_.Shift(last_integ_period_end_time).count());
}

void DeviceProcessor::Process(
    DeviceBase::RawData& raw_data,
    const PandoInterface::Config& dynamic_config,
    size_t begin_bin_idx,
    size_t end_bin_idx) {
  // Calculate the timetamp (in the sync box time domain) of the left edge of the first bin
  auto first_bin_timestamp =
      time_shifter_.Shift(std::chrono::nanoseconds{config_.bin_size_ns * begin_bin_idx});

  // All protobuf messages from this integration period will be allocated on this arena. It will be
  // deleted after the messages have been serialized.
  auto arena = std::make_unique<google::protobuf::Arena>();

  proto::Packet* timestamps_packet = nullptr;
  proto::Packet* histograms_packet = nullptr;
  proto::Packet* markers_packet = nullptr;
  proto::Packet* dtofs_packet = nullptr;
  proto::Packet* cris_packet = nullptr;
  proto::Packet* counts_packet = nullptr;
  proto::Packet* g2s_packet = nullptr;
  proto::Packet* g2is_packet = nullptr;

  // Kick off asynchronous logging of raw data to HDF5
  BlockingFuture<void> raw_data_log_done;
  if (raw_logger_) {
    raw_data_log_done = raw_data_log_worker_.Async([&] {
      // Log either timestamps or histograms, depending on device type
      if (device_->GetDeviceType() == DeviceBase::DeviceType::TIMETAGGER) {
        raw_logger_->LogTimestamps(raw_data.timestamps);
      } else if (device_->GetDeviceType() == DeviceBase::DeviceType::HISTOGRAMMER) {
        raw_logger_->LogHistograms(raw_data.histograms);
      }

      // Log marker timestamps
      raw_logger_->LogMarkers(raw_data.marker_timestamps);
    });
  }

  // Kick off asynchronous building (but not serialization) of proto::Packets containing raw data.
  // This is usually the most intensive protobuffing step.
  // If config_.log_raw_data == false, we skip this step.
  BlockingFuture<void> raw_data_pb_built;
  if (config_.publish_raw_data) {
    raw_data_pb_built = raw_data_pb_worker_.Async([&] {
      // Protobuf either timestamps or histograms, depending on device type
      if (device_->GetDeviceType() == DeviceBase::DeviceType::TIMETAGGER) {
        timestamps_packet = ProtobufTimetags(raw_data, arena.get(), first_bin_timestamp);
      } else if (device_->GetDeviceType() == DeviceBase::DeviceType::HISTOGRAMMER) {
        histograms_packet = ProtobufHistograms(raw_data, arena.get(), first_bin_timestamp);
      }

      // Protobuf marker timestamps
      markers_packet = ProtobufMarkers(raw_data, arena.get(), first_bin_timestamp);
    });
  }

  // Zip together per-channel objects into a vector of tuples of references.
  // This gives us a single range to use with std::transform below.
  auto zipped_channel_items = ZipRefs(
      raw_data.timestamps.begin(),
      raw_data.timestamps.end(),
      raw_data.histograms.begin(),
      correlators_.begin(),
      ts_counters_.begin(),
      hist_counters_.begin(),
      cri_counters_.begin(),
      dtofs_.begin());

  // Determine which calculations to perform and how to perform them based on config_ and device
  // details
  const bool calculate_dtof_and_cri = device_->DeviceGeneratesMicrotimes() &&
      (config_.exp_type == PandoInterface::ExperimentType::TD ||
       config_.exp_type == PandoInterface::ExperimentType::TDDCS);
  const bool calculate_cr_from_timestamps =
      device_->GetDeviceType() == DeviceBase::DeviceType::TIMETAGGER;
  const bool calculate_cr_from_histograms =
      device_->GetDeviceType() == DeviceBase::DeviceType::HISTOGRAMMER;
  const bool calculate_g2 = config_.exp_type == PandoInterface::ExperimentType::DCS ||
      config_.exp_type == PandoInterface::ExperimentType::TDDCS;
  const bool calculate_histogram_from_timestamps =
      calculate_g2 && device_->GetDeviceType() == DeviceBase::DeviceType::TIMETAGGER;
  const bool g2_is_g2i = calculate_histogram_from_timestamps &&
      device_->DeviceGeneratesMicrotimes() && config_.calc_g2i &&
      config_.exp_type == PandoInterface::ExperimentType::TDDCS;

  // Perform processing for all channels in parallel
  std::vector<std::exception_ptr> transform_eptrs(zipped_channel_items.size());
  std::transform(
      std::execution::par,
      zipped_channel_items.cbegin(),
      zipped_channel_items.cend(),
      transform_eptrs.begin(),
      [&](const decltype(zipped_channel_items)::value_type& channel_items) {
        std::exception_ptr eptr;
        try {
          // Extract channel index from the first KVPair in channel_items
          auto ch_idx = std::get<0>(channel_items).first;
          // All channel indices match
          assert(std::get<1>(channel_items).first == ch_idx);
          assert(std::get<2>(channel_items).first == ch_idx);
          assert(std::get<3>(channel_items).first == ch_idx);
          assert(std::get<4>(channel_items).first == ch_idx);
          assert(std::get<5>(channel_items).first == ch_idx);
          assert(std::get<6>(channel_items).first == ch_idx);

          // Name the values of the KVPairs stored in channel_items
          auto& timestamps = std::get<0>(channel_items).second;
          auto& histogram = std::get<1>(channel_items).second;
          auto& correlator = std::get<2>(channel_items).second;
          auto& timestamps_counter = std::get<3>(channel_items).second;
          auto& histogram_counter = std::get<4>(channel_items).second;
          auto& roi_counter = std::get<5>(channel_items).second;
          auto& dtof = std::get<6>(channel_items).second;

          // Calculate DTOF and CRI if the device generates microtimes
          MicroTime cri_from{0};
          MicroTime cri_until{0};
          if (calculate_dtof_and_cri) {
            // Calculate DTOF
            MacroTime dtof_integration_period{dynamic_config.dtof_integ_period_ns * 1000};
            dtof.Generate(timestamps, dtof_integration_period);

            // Calculate CRI
            MacroTime cri_integration_period{dynamic_config.cri_integ_period_ns * 1000};
            auto cri_offset_it = dynamic_config.cri_offset.find(ch_idx);
            if (cri_offset_it != dynamic_config.cri_offset.end()) {
              cri_from = MicroTime{cri_offset_it->second};
            }
            auto cri_width_it = dynamic_config.cri_width.find(ch_idx);
            if (cri_width_it != dynamic_config.cri_width.end()) {
              cri_until = cri_from + MicroTime{cri_width_it->second};
            }
            roi_counter.CountROI(timestamps, cri_integration_period, cri_from, cri_until);
          }

          // Calculate count rate
          MacroTime counts_integration_period{dynamic_config.count_integ_period_ns * 1000};
          if (calculate_cr_from_timestamps) {
            timestamps_counter.CountTimestamps(timestamps, counts_integration_period);
          } else if (calculate_cr_from_histograms) {
            histogram_counter.CountHistogram(histogram, counts_integration_period);
          }

          // Calculate g2
          if (calculate_g2) {
            if (calculate_histogram_from_timestamps) {
              if (g2_is_g2i) {
                // downsample macrotimes
                MacroTimes downsampled = DownsampleMacroTimes(timestamps, cri_from, cri_until);
                histogram.BinMacroTimes(downsampled, begin_bin_idx, end_bin_idx);
              } else {
                histogram.BinMacroTimes(timestamps.macro_times(), begin_bin_idx, end_bin_idx);
              }
            }

            correlator.UpdateG2(histogram);
          }
        } catch (...) {
          eptr = std::current_exception();
        }
        return eptr;
      });

  // If exceptions were caught during the above parallel processing, rethrow one of them.
  auto null_pred = [](auto& a) -> auto& {
    return a;
  };
  auto first_eptr_it = std::find_if(transform_eptrs.begin(), transform_eptrs.end(), null_pred);
  if (first_eptr_it != transform_eptrs.end()) {
    g_reporter->warn(
        "DeviceProcessor::Process: {} exceptions caught inside std::transform. Rethrowing one of them.",
        std::count_if(transform_eptrs.begin(), transform_eptrs.end(), null_pred));
    std::rethrow_exception(*first_eptr_it);
  }

  // Protobuf (but don't serialize) the results of the above processing.
  if (calculate_dtof_and_cri) {
    dtofs_packet = ProtobufDtofs(arena.get());
    cris_packet = ProtobufCris(arena.get());
  }
  if (calculate_cr_from_timestamps) {
    counts_packet = ProtobufCountsFromTimetags(arena.get());
  } else if (calculate_cr_from_histograms) {
    counts_packet = ProtobufCountsFromHistograms(arena.get());
  }
  if (calculate_g2) {
    if (g2_is_g2i) {
      g2is_packet = ProtobufG2is(arena.get(), dynamic_config, first_bin_timestamp);
    } else {
      g2s_packet = ProtobufG2s(arena.get(), first_bin_timestamp);
    }
  }

  // Wait for protobuffing of raw data to complete
  if (raw_data_pb_built.valid()) {
    raw_data_pb_built.get();
  }

  // Increment sequence number shared by g2s_packet, g2is_packet, timestamps_packet,
  // histograms_packet, & markers_packet
  g2_seq_++;

  // g_reporter->debug("arena final allocation is {}", arena->SpaceAllocated());
  // g_reporter->debug("arena final size is {}", arena->SpaceUsed());

  // Kick off asynchronous serialization of all the protobuf packets we built. This can take a
  // while, and we expect to have multiple serialization jobs in progress at once.
  // The arena the packets were allocated on is moved into the lambda to extend its lifetime.
  auto zmq_messages_future = std::async(std::launch::async, [=, arena{std::move(arena)}] {
    // vector of tuples of topic, topic_size, payload
    std::vector<std::tuple<const char*, size_t, zmq::message_t>> zmq_messages;

    if (timestamps_packet) {
      static constexpr char timestamps_topic[] = "pando.time_tags_array";
      zmq_messages.emplace_back(timestamps_topic, sizeof(timestamps_topic), zmq::message_t{});
      common::ProtobufSerialize(*timestamps_packet, std::get<2>(zmq_messages.back()));
    }
    if (histograms_packet) {
      static constexpr char histograms_topic[] = "pando.histograms";
      zmq_messages.emplace_back(histograms_topic, sizeof(histograms_topic), zmq::message_t{});
      common::ProtobufSerialize(*histograms_packet, std::get<2>(zmq_messages.back()));
    }
    if (markers_packet) {
      static constexpr char marker_topic[] = "pando.markers";
      zmq_messages.emplace_back(marker_topic, sizeof(marker_topic), zmq::message_t{});
      common::ProtobufSerialize(*markers_packet, std::get<2>(zmq_messages.back()));
    }
    if (dtofs_packet) {
      static constexpr char dtof_topic[] = "pando.dtof";
      zmq_messages.emplace_back(dtof_topic, sizeof(dtof_topic), zmq::message_t{});
      common::ProtobufSerialize(*dtofs_packet, std::get<2>(zmq_messages.back()));
    }
    if (cris_packet) {
      static constexpr char cri_topic[] = "pando.cri";
      zmq_messages.emplace_back(cri_topic, sizeof(cri_topic), zmq::message_t{});
      common::ProtobufSerialize(*cris_packet, std::get<2>(zmq_messages.back()));
    }
    if (counts_packet) {
      static constexpr char counts_topic[] = "pando.counts";
      zmq_messages.emplace_back(counts_topic, sizeof(counts_topic), zmq::message_t{});
      common::ProtobufSerialize(*counts_packet, std::get<2>(zmq_messages.back()));
    }
    if (g2s_packet) {
      static constexpr char g2_topic[] = "pando.g2";
      zmq_messages.emplace_back(g2_topic, sizeof(g2_topic), zmq::message_t{});
      common::ProtobufSerialize(*g2s_packet, std::get<2>(zmq_messages.back()));
    }
    if (g2is_packet) {
      static constexpr char g2i_topic[] = "pando.g2i";
      zmq_messages.emplace_back(g2i_topic, sizeof(g2i_topic), zmq::message_t{});
      common::ProtobufSerialize(*g2is_packet, std::get<2>(zmq_messages.back()));
    }

    return zmq_messages;
  });

  // Queue task to send ZMQ messages when serialization job completes.
  // The FIFO queue of pando::Worker ensures that the messages from concurrent jobs stay ordered.
  zmq_publish_worker_.Async([zmq_messages_future{std::move(zmq_messages_future)}, this]() mutable {
    for (auto& msg_tuple : zmq_messages_future.get()) {
      PublishZmqMessage(std::get<0>(msg_tuple), std::get<1>(msg_tuple), std::get<2>(msg_tuple));
    }
  });

  // Wait for logging of raw data to complete
  if (raw_data_log_done.valid()) {
    raw_data_log_done.get();
  }
}

void DeviceProcessor::HandlePps(const DeviceBase::RawData& raw_data) {
  // Get an iterator pointing to the PPS channel in raw_data.marker_timestamps
  auto pps_ch_it = std::find_if(
      raw_data.marker_timestamps.cbegin(),
      raw_data.marker_timestamps.cend(),
      [](const auto& ch_kv) { return ch_kv.first == kPpsMarkerChIdx; });
  assert(pps_ch_it < raw_data.marker_timestamps.cend());

  // Process all available PPS pulses. Note that we effectively only make use of the last PPS pulse
  // in the provided raw_data.
  for (const auto& pps_time : pps_ch_it->second.macro_times()) {
    auto stats = time_shifter_.Adjust(pps_time);

    if (stats.has_value()) {
      // By definition, the shifted timestamp of the PPS pulse should be evenly divisible by 1
      // second
      auto shifted_pps_time = time_shifter_.Shift(pps_time);
      assert((shifted_pps_time % std::chrono::seconds{1}).count() == 0);

      // Protobuf & publish stats on the PPS pulse
      proto::Packet pps_pb;
      zmq::message_t payload_msg;

      // Populate header
      pps_pb.mutable_header()->set_experiment_id(experiment_id_);
      pps_pb.mutable_header()->set_sequence_number(pps_seq_++);
      pps_pb.mutable_header()->set_timestamp(shifted_pps_time.count());

      // Populate payload
      pps_pb.mutable_payload()->mutable_pps_stats()->set_offset_ns(stats->offset.count());
      pps_pb.mutable_payload()->mutable_pps_stats()->set_jitter_ns(stats->jitter.count());

      // Serialize and queue the result to be published
      common::ProtobufSerialize(pps_pb, payload_msg);
      zmq_publish_worker_.Async([this, payload_msg{std::move(payload_msg)}]() mutable {
        static constexpr char pps_stats_topic[] = "pando.pps_stats";
        PublishZmqMessage(pps_stats_topic, sizeof(pps_stats_topic), payload_msg);
      });
    }
  }
}

void DeviceProcessor::PublishZmqMessage(
    const char* topic,
    size_t topic_size,
    zmq::message_t& payload_msg) {
  bool ret = pub_socket_.send(topic, topic_size, ZMQ_SNDMORE);
  ret &= pub_socket_.send(payload_msg);
  if (!ret) {
    throw std::overflow_error("DeviceProcessor::PublishZmqMessage: ZMQ send operation failed");
  }
}

void DeviceProcessor::SendZmqEvent(
    proto::EventType event_type,
    int64_t event_seq,
    int64_t timestamp) {
  proto::Packet event_pb;
  zmq::message_t payload_msg;

  // Create header for protobuf'd data
  proto::Packet::Header header;
  header.set_experiment_id(experiment_id_);
  header.set_sequence_number(event_seq);
  header.set_timestamp(timestamp);

  *event_pb.mutable_header() = header;
  auto& event_data_pb = (*event_pb.mutable_payload()->mutable_event());
  event_data_pb.set_event(event_type);

  common::ProtobufSerialize(event_pb, payload_msg);

  zmq_publish_worker_.Async([this, payload_msg{std::move(payload_msg)}]() mutable {
    static constexpr char ev_topic[] = "pando.event";
    PublishZmqMessage(ev_topic, sizeof(ev_topic), payload_msg);
  });
}

proto::Packet* DeviceProcessor::ProtobufTimetags(
    const DeviceBase::RawData& raw_data,
    google::protobuf::Arena* arena,
    std::chrono::nanoseconds timestamp) {
  proto::Packet* packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
  packet->mutable_header()->set_experiment_id(experiment_id_);
  packet->mutable_header()->set_sequence_number(g2_seq_);
  packet->mutable_header()->set_timestamp(timestamp.count());
  auto* channels_map = packet->mutable_payload()->mutable_timestamps()->mutable_channels();

  for (const auto& channel_timestamps : raw_data.timestamps) {
    auto& timestamps_data_pb = channels_map->operator[](channel_timestamps.first);

    // Copy macro & micro times into protobuf message
    common::AssignRepeatedField(
        channel_timestamps.second.macro_times().RepCBegin(),
        channel_timestamps.second.macro_times().RepCEnd(),
        timestamps_data_pb.mutable_macro_times());

    common::AssignRepeatedField(
        channel_timestamps.second.micro_times().RepCBegin(),
        channel_timestamps.second.micro_times().RepCEnd(),
        timestamps_data_pb.mutable_micro_times());
  }

  return packet;
}

proto::Packet* DeviceProcessor::ProtobufHistograms(
    const DeviceBase::RawData& raw_data,
    google::protobuf::Arena* arena,
    std::chrono::nanoseconds timestamp) {
  proto::Packet* packet = nullptr;

  if (!raw_data.histograms.empty()) {
    // Create the packet
    proto::Packet* packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
    packet->mutable_header()->set_experiment_id(experiment_id_);
    packet->mutable_header()->set_sequence_number(g2_seq_);
    packet->mutable_header()->set_timestamp(timestamp.count());

    auto& first_histogram = raw_data.histograms.begin()->second;

    // Populate meta field
    auto* meta = packet->mutable_payload()->mutable_histograms()->mutable_meta();
    meta->set_bin_size(first_histogram.GetBinSize().count());
    meta->set_first_bin_idx(first_histogram.GetBeginBinIdx());
    meta->set_last_bin_idx(first_histogram.GetEndBinIdx() - 1);

    // Populate channels field
    auto* channels_map = packet->mutable_payload()->mutable_histograms()->mutable_channels();
    for (const auto& channel_histogram_kv : raw_data.histograms) {
      auto ch_idx = channel_histogram_kv.first;
      auto& channel_histogram = channel_histogram_kv.second;

      // We populated the meta field from the first channel, but all channels match.
      assert(channel_histogram.GetBinSize() == first_histogram.GetBinSize());
      assert(channel_histogram.GetBeginBinIdx() == first_histogram.GetBeginBinIdx());
      assert(channel_histogram.GetEndBinIdx() == first_histogram.GetEndBinIdx());

      auto& histograms_data_pb = channels_map->operator[](ch_idx);

      // Copy histogram into protobuf message
      common::AssignRepeatedField(
          channel_histogram.cbegin(),
          channel_histogram.cend(),
          histograms_data_pb.mutable_counts());
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufCris(google::protobuf::Arena* arena) {
  proto::Packet* packet = nullptr;
  if (!cri_counters_.empty()) {
    const auto& first_counter = cri_counters_.begin()->second;

    if (first_counter.IntegrationPeriodComplete()) {
      // Create the packet
      packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
      packet->mutable_header()->set_experiment_id(experiment_id_);
      packet->mutable_header()->set_sequence_number(cri_seq_++);
      packet->mutable_header()->set_timestamp(
          time_shifter_.Shift(first_counter.GetIntegrationPeriodStartTime()).count());

      // Populate meta field
      auto* meta = packet->mutable_payload()->mutable_cri()->mutable_meta();
      meta->set_integration_period_ns(
          std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(
              first_counter.GetIntegrationPeriod())
              .count());

      // Populate channels field
      auto* channels_map = packet->mutable_payload()->mutable_cri()->mutable_channels();
      for (const auto& cri_counter_kv : cri_counters_) {
        auto ch_idx = cri_counter_kv.first;
        auto& ch_counter = cri_counter_kv.second;

        // We populated the meta field from the first channel, but all channels match.
        assert(ch_counter.IntegrationPeriodComplete());
        assert(
            ch_counter.GetIntegrationPeriodStartTime() ==
            first_counter.GetIntegrationPeriodStartTime());
        assert(ch_counter.GetIntegrationPeriod() == first_counter.GetIntegrationPeriod());

        auto& cri_data_pb = channels_map->operator[](ch_idx);

        cri_data_pb.set_utime_from_ps(ch_counter.GetROIMin().count());
        cri_data_pb.set_utime_until_ps(ch_counter.GetROIMax().count());
        cri_data_pb.set_count(ch_counter.GetCount());
      }
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufDtofs(google::protobuf::Arena* arena) {
  proto::Packet* packet = nullptr;
  if (!dtofs_.empty()) {
    const auto& first_dtof = dtofs_.begin()->second;

    if (first_dtof.IntegrationPeriodComplete()) {
      // Create the packet
      packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
      packet->mutable_header()->set_experiment_id(experiment_id_);
      packet->mutable_header()->set_sequence_number(dtof_seq_++);
      packet->mutable_header()->set_timestamp(
          time_shifter_.Shift(first_dtof.GetIntegrationPeriodStartTime()).count());

      // Populate meta field
      auto* meta = packet->mutable_payload()->mutable_dtof()->mutable_meta();
      meta->set_resolution(first_dtof.GetBinSize().count());
      meta->set_range_min(first_dtof.GetRangeMin().count());
      meta->set_range_max(first_dtof.GetRangeMax().count());
      meta->set_integration_period_ns(
          std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(
              first_dtof.GetIntegrationPeriod())
              .count());

      // Populate channels field
      auto* channels_map = packet->mutable_payload()->mutable_dtof()->mutable_channels();
      for (const auto& dtof_kv : dtofs_) {
        auto ch_idx = dtof_kv.first;
        auto& ch_dtof = dtof_kv.second;

        // We populated the meta field from the first channel, but all channels match.
        assert(ch_dtof.IntegrationPeriodComplete());
        assert(
            ch_dtof.GetIntegrationPeriodStartTime() == first_dtof.GetIntegrationPeriodStartTime());
        assert(ch_dtof.GetBinSize() == first_dtof.GetBinSize());
        assert(ch_dtof.GetRangeMin() == first_dtof.GetRangeMin());
        assert(ch_dtof.GetRangeMax() == first_dtof.GetRangeMax());
        assert(ch_dtof.GetIntegrationPeriod() == first_dtof.GetIntegrationPeriod());

        auto& dtof_data_pb = channels_map->operator[](ch_idx);

        common::AssignRepeatedField(
            ch_dtof.cbegin(), ch_dtof.cend(), dtof_data_pb.mutable_counts());
      }
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufCountsFromTimetags(google::protobuf::Arena* arena) {
  proto::Packet* packet = nullptr;
  if (!ts_counters_.empty()) {
    const auto& first_counter = ts_counters_.begin()->second;

    if (first_counter.IntegrationPeriodComplete()) {
      // Create the packet
      packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
      packet->mutable_header()->set_experiment_id(experiment_id_);
      packet->mutable_header()->set_sequence_number(count_seq_++);
      packet->mutable_header()->set_timestamp(
          time_shifter_.Shift(first_counter.GetIntegrationPeriodStartTime()).count());

      // Populate meta field
      auto* meta = packet->mutable_payload()->mutable_counts()->mutable_meta();
      meta->set_integration_period_ns(
          std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(
              first_counter.GetIntegrationPeriod())
              .count());

      // Populate channels field
      auto* channels_map = packet->mutable_payload()->mutable_counts()->mutable_channels();
      for (const auto& counter_kv : ts_counters_) {
        auto ch_idx = counter_kv.first;
        auto& ch_counter = counter_kv.second;

        // We populated the meta field from the first channel, but all channels match.
        assert(ch_counter.IntegrationPeriodComplete());
        assert(
            ch_counter.GetIntegrationPeriodStartTime() ==
            first_counter.GetIntegrationPeriodStartTime());
        assert(ch_counter.GetIntegrationPeriod() == first_counter.GetIntegrationPeriod());

        auto& counts_data_pb = channels_map->operator[](ch_idx);

        counts_data_pb.set_count(ch_counter.GetCount());
      }
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufCountsFromHistograms(google::protobuf::Arena* arena) {
  proto::Packet* packet = nullptr;
  if (!hist_counters_.empty()) {
    const auto& first_counter = hist_counters_.begin()->second;

    if (first_counter.IntegrationPeriodComplete()) {
      // Create the packet
      packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
      packet->mutable_header()->set_experiment_id(experiment_id_);
      packet->mutable_header()->set_sequence_number(count_seq_++);
      packet->mutable_header()->set_timestamp(
          time_shifter_.Shift(first_counter.GetIntegrationPeriodStartTime()).count());

      // Populate meta field
      auto* meta = packet->mutable_payload()->mutable_counts()->mutable_meta();
      meta->set_integration_period_ns(
          std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(
              first_counter.GetIntegrationPeriod())
              .count());

      // Populate channels field
      auto* channels_map = packet->mutable_payload()->mutable_counts()->mutable_channels();
      for (const auto& counter_kv : hist_counters_) {
        auto ch_idx = counter_kv.first;
        auto& ch_counter = counter_kv.second;

        // We populated the meta field from the first channel, but all channels match.
        assert(ch_counter.IntegrationPeriodComplete());
        assert(
            ch_counter.GetIntegrationPeriodStartTime() ==
            first_counter.GetIntegrationPeriodStartTime());
        assert(ch_counter.GetIntegrationPeriod() == first_counter.GetIntegrationPeriod());

        auto& counts_data_pb = channels_map->operator[](ch_idx);

        counts_data_pb.set_count(ch_counter.GetCount());
      }
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufG2s(
    google::protobuf::Arena* arena,
    std::chrono::nanoseconds timestamp) {
  proto::Packet* packet = nullptr;

  if (!correlators_.empty()) {
    // Create the packet
    packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
    packet->mutable_header()->set_experiment_id(experiment_id_);
    packet->mutable_header()->set_sequence_number(g2_seq_);
    packet->mutable_header()->set_timestamp(timestamp.count());

    const auto& first_correlator = correlators_.begin()->second;
    auto& first_g2_result = first_correlator.GetResult();

    // Populate meta field
    auto* meta = packet->mutable_payload()->mutable_g2()->mutable_meta();
    meta->set_dt(std::chrono::duration_cast<std::chrono::duration<double>>(
                     decltype(first_g2_result.tau_k)::value_type(1))
                     .count());
    common::AssignRepeatedField(
        first_g2_result.tau_k.RepCBegin(), first_g2_result.tau_k.RepCEnd(), meta->mutable_k());

    // Populate channels field
    auto* channels_map = packet->mutable_payload()->mutable_g2()->mutable_channels();
    for (const auto& correlator_kv : correlators_) {
      auto ch_idx = correlator_kv.first;
      auto& g2_result = correlator_kv.second.GetResult();

      // We populated the meta field from the first channel, but all channels match.
      assert(std::equal(
          g2_result.tau_k.cbegin(),
          g2_result.tau_k.cend(),
          first_g2_result.tau_k.cbegin(),
          first_g2_result.tau_k.cend()));

      auto& g2_data_pb = channels_map->operator[](ch_idx);
      g2_data_pb.set_channel_1(ch_idx);
      g2_data_pb.set_channel_2(ch_idx);
      common::AssignRepeatedField(
          g2_result.g2.cbegin(), g2_result.g2.cend(), g2_data_pb.mutable_g2());
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufG2is(
    google::protobuf::Arena* arena,
    PandoInterface::Config dynamic_config,
    std::chrono::nanoseconds timestamp) {
  proto::Packet* packet = nullptr;

  if (!correlators_.empty()) {
    // Create the packet
    proto::Packet* packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);
    packet->mutable_header()->set_experiment_id(experiment_id_);
    packet->mutable_header()->set_sequence_number(g2_seq_);
    packet->mutable_header()->set_timestamp(timestamp.count());

    const auto& first_correlator = correlators_.begin()->second;
    auto& first_g2i_result = first_correlator.GetResult();

    // Populate meta field
    auto* meta = packet->mutable_payload()->mutable_g2i()->mutable_meta();
    meta->set_dt(std::chrono::duration_cast<std::chrono::duration<double>>(
                     decltype(first_g2i_result.tau_k)::value_type(1))
                     .count());
    common::AssignRepeatedField(
        first_g2i_result.tau_k.RepCBegin(), first_g2i_result.tau_k.RepCEnd(), meta->mutable_k());

    // Populate channels field
    auto* channels_map = packet->mutable_payload()->mutable_g2i()->mutable_channels();
    for (const auto& correlator_kv : correlators_) {
      auto ch_idx = correlator_kv.first;
      auto& g2i_result = correlator_kv.second.GetResult();

      // We populated the meta field from the first channel, but all channels match.
      assert(std::equal(
          g2i_result.tau_k.cbegin(),
          g2i_result.tau_k.cend(),
          first_g2i_result.tau_k.cbegin(),
          first_g2i_result.tau_k.cend()));

      auto& g2i_data_pb = channels_map->operator[](ch_idx);

      MicroTime cri_from{0};
      MicroTime cri_until{0};

      auto cri_offset_it = dynamic_config.cri_offset.find(ch_idx);
      if (cri_offset_it != dynamic_config.cri_offset.end()) {
        cri_from = MicroTime{cri_offset_it->second};
      }
      auto cri_width_it = dynamic_config.cri_width.find(ch_idx);
      if (cri_width_it != dynamic_config.cri_width.end()) {
        cri_until = cri_from + MicroTime{cri_width_it->second};
      }
      g2i_data_pb.set_channel_1(ch_idx);
      g2i_data_pb.set_channel_2(ch_idx);
      common::AssignRepeatedField(
          g2i_result.g2.cbegin(), g2i_result.g2.cend(), g2i_data_pb.mutable_g2i());
      g2i_data_pb.set_utime_from_ps(cri_from.count());
      g2i_data_pb.set_utime_until_ps(cri_until.count());
    }
  }
  return packet;
}

proto::Packet* DeviceProcessor::ProtobufMarkers(
    const DeviceBase::RawData& raw_data,
    google::protobuf::Arena* arena,
    std::chrono::nanoseconds timestamp) {
  proto::Packet* packet = google::protobuf::Arena::CreateMessage<proto::Packet>(arena);

  auto* channels_map = packet->mutable_payload()->mutable_markers()->mutable_channels();

  auto markers_present = false;
  for (const auto& timestamps_it : raw_data.marker_timestamps) {
    auto& ch_idx = timestamps_it.first;
    auto& ch_timestamps = timestamps_it.second;
    if (!ch_timestamps.macro_times().empty()) {
      // this check is used to make sure that empty pb messages aren't being sent
      markers_present = true;
    }
    auto& marker_data_pb = channels_map->operator[](ch_idx);

    // Copy macro & micro times into protobuf message
    common::AssignRepeatedField(
        ch_timestamps.macro_times().RepCBegin(),
        ch_timestamps.macro_times().RepCEnd(),
        marker_data_pb.mutable_macro_times());
  }

  if (markers_present) {
    packet->mutable_header()->set_experiment_id(experiment_id_);
    packet->mutable_header()->set_sequence_number(marker_seq_++);
    packet->mutable_header()->set_timestamp(timestamp.count());
    return packet;
  } else {
    return nullptr;
  }
}

MacroTimes DeviceProcessor::DownsampleMacroTimes(
    const ChannelTimestamps& timestamps,
    MicroTime from,
    MicroTime until) {
  MacroTimes downsampled;
  auto& microtimes = timestamps.micro_times();
  auto& macrotimes = timestamps.macro_times();
  downsampled.reserve(macrotimes.size());
  for (int i = 0; i < microtimes.size(); i++) {
    if (microtimes[i] >= from && microtimes[i] <= until) {
      downsampled.push_back(macrotimes[i]);
    }
  }
  return downsampled;
}

} // namespace pando
} // namespace pnd
