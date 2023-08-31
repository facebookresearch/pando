#include "pf32_g2.h"

#include "kv_container.h"
#include "protobuf_util.h"
#include "reporter.h"

#include <algorithm>
#include <array>
#include <chrono>

namespace {
/** Helper function to reinterpret_cast an array with elements of type std::duration<Rep, Period> to
 * an array with elements of type Rep. */
template <class Rep, class Period, std::size_t size>
auto& AsRepArray(const std::array<std::chrono::duration<Rep, Period>, size>& arr) {
  return reinterpret_cast<const std::array<Rep, size>&>(arr);
}
} // namespace

namespace pnd {
namespace pando {
PF32G2::PF32G2(const char* firmware_path) : pf32_ll_(firmware_path) {}

void PF32G2::StartImpl(
    int32_t experiment_id,
    PandoInterface::Config config,
    const char* raw_file_name) {
  config_ = std::move(config);

  if (config_.log_raw_data)
    raw_logger_ = std::make_unique<HDF5RawLogger>(raw_file_name);

  // Reset the TimeShifter
  // Set holdoff period to 0, because The marker channel feature of this device effectively
  // debounces the signal for us.
  time_shifter_ = TimeShifter(std::chrono::milliseconds(0));

  experiment_id_ = experiment_id;
  g2_seq_ = 0;
  count_seq_ = 0;
  event_seq_ = 0;
  marker_seq_ = 0;
  pps_seq_ = 0;

  std::chrono::nanoseconds frame_period(config_.bin_size_ns);

  acquisition_ = pf32_ll_.CreateAcquisitionG2(
      frame_period,
      config_.pf32_g2_frame_count,
      config_.pf32_g2_burst_mode,
      {config_.pf32_g2_rebin_factor_0,
       config_.pf32_g2_rebin_factor_1,
       config_.pf32_g2_rebin_factor_2,
       config_.pf32_g2_rebin_factor_3},
      config_.enabled_channels);

  // Acquire some data n as a means of blocking until the device acquisition has actually begun.
  acquisition_->GetResult();

  // Create packet payload fields for each enabled channel
  g2_packet_ = {};
  counts_packet_ = {};
  auto& g2_channels_map = *g2_packet_.mutable_payload()->mutable_g2()->mutable_channels();
  auto& counts_channels_map =
      *counts_packet_.mutable_payload()->mutable_counts()->mutable_channels();
  for (auto ch_idx : config_.enabled_channels) {
    if (ch_idx < 0 || ch_idx >= pf32_ll::kChannelCount) {
      throw Exception("Invalid channel number specified");
    }
    g2_channels_map[ch_idx] = {};
    counts_channels_map[ch_idx] = {};
  }

  run_thread_ = common::ThreadContainer([&] { Run(); }, "PF32G2::Run", &run_stop_signal_);
}

void PF32G2::StopImpl() {
  run_thread_.Stop();
  run_thread_.Join();

  // Deallocate memory used by protobuf messages
  g2_packet_ = {};
  counts_packet_ = {};

  acquisition_.reset();

  // release the unique_pointer so the HDF5 file is written out on experiment end.
  raw_logger_.reset();
}

void PF32G2::Run() {
  storage_pool::FactoryHandle<pf32_ll::AcquisitionG2::CorrelatorResult> result_handle;
  std::chrono::nanoseconds first_integ_period_start_time{};

  // Discard results while searching for the first PPS edge
  if (config_.use_pps) {
    while (!time_shifter_.IsPrimed()) {
      // Time out eventually if no PPS is found
      if (result_handle &&
          result_handle->begin_frame_idx * acquisition_->GetFramePeriod() >=
              std::chrono::milliseconds(20100)) {
        throw std::runtime_error(
            "Waited for first PPS pulse for more than twenty seconds. Is a PPS signal connected to marker channel 0?");
      }

      result_handle = acquisition_->GetResult();

      HandlePps(*result_handle);
    }

    first_integ_period_start_time = {config_.bin_size_ns * result_handle->begin_frame_idx};

    g_reporter->debug(
        "PF32G2: Detected first PPS after {}ms",
        std::chrono::round<std::chrono::milliseconds>(first_integ_period_start_time).count());
  }

  // send zmq START event
  // Note: Because the precision of marker channel timestamps is one TInt, shifting
  // first_integ_period_start_time into the PPS time domain will always yield a timestamp of 0, just
  // as if we weren't doing PPS adjustment at all.
  publisher_.SendZmqEvent(
      proto::EVENT_START,
      experiment_id_,
      event_seq_++,
      time_shifter_.Shift(first_integ_period_start_time));

  // Process the result that contained the first PPS edge
  if (config_.use_pps) {
    Process(*result_handle);
  }

  while (!run_stop_signal_.ShouldStop()) {
    result_handle.reset();
    result_handle = acquisition_->GetResult();
    if (config_.use_pps) {
      HandlePps(*result_handle);
    }
    Process(*result_handle);
  }

  // send zmq STOP event
  auto end_frame_idx = result_handle->begin_frame_idx + result_handle->frame_count;
  std::chrono::nanoseconds last_integ_period_end_time{config_.bin_size_ns * end_frame_idx};
  publisher_.SendZmqEvent(
      proto::EVENT_STOP,
      experiment_id_,
      event_seq_++,
      time_shifter_.Shift(last_integ_period_end_time));
}

void PF32G2::Process(const pf32_ll::AcquisitionG2::CorrelatorResult& result) {
  // The timestamp for this data points to the beginning of the g2 integration period
  auto timestamp = MacroTime{acquisition_->GetFramePeriod()} * result.begin_frame_idx;

  if (config_.exp_type == PandoInterface::ExperimentType::DCS) {
    ProtobufAndPublishG2(result, timestamp);
  }

  if (config_.publish_raw_data) {
    ProtobufAndPublishMarkers(result, timestamp);
  }

  if (raw_logger_) {
    LogMarkers(result, timestamp);
  }

  ProtobufAndPublishCounts(result, timestamp);
}

void PF32G2::ProtobufAndPublishG2(
    const pf32_ll::AcquisitionG2::CorrelatorResult& result,
    MacroTime timestamp) {
  // Populate packet header fields
  auto& header = *g2_packet_.mutable_header();
  header.set_experiment_id(experiment_id_);
  header.set_sequence_number(g2_seq_++);
  header.set_timestamp(time_shifter_.Shift(timestamp).count());

  // Populate meta field
  auto* meta = g2_packet_.mutable_payload()->mutable_g2()->mutable_meta();
  meta->set_dt(std::chrono::duration_cast<std::chrono::duration<double>>(
                   decltype(result.tau_k)::value_type(1))
                   .count());
  common::AssignRepeatedField(
      AsRepArray(result.tau_k).begin(),
      AsRepArray(result.tau_k).begin() + result.n_points,
      meta->mutable_k());

  // Populate channels field (map already contains keys for enabled channels)
  auto& channels_map = *g2_packet_.mutable_payload()->mutable_g2()->mutable_channels();
  for (auto& channel_kv : channels_map) {
    auto& ch_idx = channel_kv.first;
    auto& g2_data_pb = channel_kv.second;

    g2_data_pb.set_channel_1(ch_idx);
    g2_data_pb.set_channel_2(ch_idx);

    g2_data_pb.clear_g2();
    auto row_idx = ch_idx / pf32_ll::kRowColCount;
    auto col_idx = ch_idx % pf32_ll::kRowColCount;
    for (size_t tau_idx = 0; tau_idx < result.n_points; ++tau_idx) {
      g2_data_pb.add_g2(result.g2[row_idx][tau_idx][col_idx]);
    }
  }

  zmq::message_t msg;
  common::ProtobufSerialize(g2_packet_, msg);

  static constexpr char g2_topic[] = "pando.g2";
  publisher_.PublishZmqMessage(g2_topic, sizeof(g2_topic), msg);
}

void PF32G2::ProtobufAndPublishMarkers(
    const pf32_ll::AcquisitionG2::CorrelatorResult& result,
    MacroTime timestamp) {
  // Return early if no markers present
  if (std::none_of(result.markers.begin(), result.markers.end(), [](bool b) { return b; })) {
    return;
  }

  proto::Packet packet;

  auto* channels_map = packet.mutable_payload()->mutable_markers()->mutable_channels();

  for (auto ch_idx = 0; ch_idx < result.markers.size(); ++ch_idx) {
    if (result.markers[ch_idx]) {
      channels_map->operator[](ch_idx).add_macro_times(timestamp.count());
    }
  }

  packet.mutable_header()->set_experiment_id(experiment_id_);
  packet.mutable_header()->set_sequence_number(marker_seq_++);
  packet.mutable_header()->set_timestamp(time_shifter_.Shift(timestamp).count());

  zmq::message_t msg;
  common::ProtobufSerialize(packet, msg);

  static constexpr char marker_topic[] = "pando.markers";
  publisher_.PublishZmqMessage(marker_topic, sizeof(marker_topic), msg);
}

void PF32G2::ProtobufAndPublishCounts(
    const pf32_ll::AcquisitionG2::CorrelatorResult& result,
    MacroTime timestamp) {
  // Populate packet header fields
  auto& header = *counts_packet_.mutable_header();
  header.set_experiment_id(experiment_id_);
  header.set_sequence_number(count_seq_++);
  header.set_timestamp(time_shifter_.Shift(timestamp).count());

  // Populate meta field
  auto* meta = counts_packet_.mutable_payload()->mutable_counts()->mutable_meta();
  meta->set_integration_period_ns(
      std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(
          result.frame_count * acquisition_->GetFramePeriod())
          .count());

  // Populate channels field (map already contains keys for enabled channels)
  auto& channels_map = *counts_packet_.mutable_payload()->mutable_counts()->mutable_channels();
  for (auto& channel_kv : channels_map) {
    auto& ch_idx = channel_kv.first;
    auto& counts_data_pb = channel_kv.second;

    counts_data_pb.set_count(result.intensity_map.at(ch_idx));
  }

  zmq::message_t msg;
  common::ProtobufSerialize(counts_packet_, msg);

  static constexpr char counts_topic[] = "pando.counts";
  publisher_.PublishZmqMessage(counts_topic, sizeof(counts_topic), msg);
}

/** Helper to initialize a constexpr array with a sequence (eg. {0, 1, 2, ...}).
 * @tparam T The type of the array elements
 * @tparam n The number of elements in the array
 */
template <class T, std::size_t n>
constexpr std::array<T, n> MakeSequenceArray() {
  std::array<T, n> ret{};
  for (std::size_t i = 0; i < n; ++i) {
    ret[i] = static_cast<T>(i);
  }

  return ret;
}

void PF32G2::LogMarkers(
    const pf32_ll::AcquisitionG2::CorrelatorResult& result,
    MacroTime timestamp) {
  // Return early if no markers present
  if (std::none_of(result.markers.begin(), result.markers.end(), [](bool b) { return b; })) {
    return;
  }

  // Somewhat tortuous translation of the raw marker channel data (a an array of kMarkerChannelCount
  // bools) into a KVContainer<int32_t, ChannelTimestamps>, which is what HDF5RawLogger happens to
  // take.
  static constexpr auto marker_channel_indices =
      MakeSequenceArray<int32_t, pf32_ll::AcquisitionG2::kMarkerChannelCount>();
  KVContainer<int32_t, ChannelTimestamps> marker_timestamps{marker_channel_indices.begin(),
                                                            marker_channel_indices.end()};
  for (auto& timestamps_kv : marker_timestamps) {
    auto ch_idx = timestamps_kv.first;
    auto& timestamps = timestamps_kv.second;

    if (result.markers.at(ch_idx)) {
      timestamps.macro_times().push_back(timestamp);
      timestamps.micro_times().emplace_back(0);
    }
  }

  raw_logger_->LogMarkers(marker_timestamps);
}

void PF32G2::HandlePps(const pf32_ll::AcquisitionG2::CorrelatorResult& result) {
  if (result.markers[kPpsMarkerChIdx]) {
    auto pps_time = MacroTime{acquisition_->GetFramePeriod()} * result.begin_frame_idx;

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
      static constexpr char pps_stats_topic[] = "pando.pps_stats";
      publisher_.PublishZmqMessage(pps_stats_topic, sizeof(pps_stats_topic), payload_msg);
    }
  }
}

} // namespace pando
} // namespace pnd
