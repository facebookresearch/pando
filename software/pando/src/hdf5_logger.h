#pragma once

#include "hdf5_table.h"
#include "mutex.h"
#include "pando.pb.h"
#include "protobuf_h5_transcoder.h"
#include "reporter.h"

#include <H5Cpp.h>
#include <fmt/format.h>
#include <google/protobuf/message.h>

#include <algorithm>
#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace pnd {
namespace pando {

/** Base class for PacketRowLogger and PacketColLogger. */
class PacketLoggerBase {
 public:
  PacketLoggerBase(
      H5::H5File& file,
      std::vector<ProtobufH5TranscoderBase::FieldTypeOverride> overrides = {})
      : file_(file), overrides_(overrides){};

 protected:
  bool initialized_ = false;

  HDF5Table::Buffer wbuf_;

  H5::H5File& file_;
  std::vector<ProtobufH5TranscoderBase::FieldTypeOverride> overrides_;

  std::string payload_name_;

  void CheckSeqNum(uint32_t sequence_number) {
    if (sequence_number != packet_count) {
      g_reporter->debug(
          "PacketLoggerBase: packet of type {} had sequence_number = {}, expected {}",
          payload_name_,
          sequence_number,
          packet_count);
      throw std::runtime_error("PacketLoggerBase: Encountered nonsequential sequence_number");
    }
    packet_count++;
  }

 private:
  google::protobuf::int32 packet_count = 0;
};

/** Class for creating and writing to a 2D HDF5 dataset where each channel is represented as a
 * column and each protobuf packet is represented as a row.
 * @tparam Payload the protobuf message type this logger is responsible for (eg. proto::G2)
 */
template <class Payload>
class PacketRowLogger : public PacketLoggerBase {
 public:
  using PacketLoggerBase::PacketLoggerBase;

  /**
   * Log all channels of a packet to HDF5.
   * @param header The packet header
   * @param payload A single member of the packet oneof payload field
   */
  void LogChannels(const proto::Packet::Header& header, const Payload& payload) {
    H5LockGuard l;
    if (!initialized_) {
      Init(payload);
    }
    Write(header, payload);
  }

 private:
  hsize_t kChunkSize = 20;
  void Init(const Payload& payload) {
    const auto& channels = payload.channels();
    assert(channels.size() > 0);

    // Note the channels indices present in this input.
    // All future inputs must have the same channels.
    channel_indices_.reserve(channels.size());
    for (auto& channel_kv : channels) {
      channel_indices_.push_back(channel_kv.first);
    }
    std::sort(channel_indices_.begin(), channel_indices_.end());

    // Create a header serializer based on a prototype
    proto::Packet::Header header_prototype;
    header_transcoder_ = std::make_unique<ProtobufH5RowTranscoder>(header_prototype);

    // Create a serializer for the Meta message type if the meta field is populated, using the meta
    // field as a prototype. (Payload types that don't happen to have any common data across
    // channels have a meta field, but it's type is an empty message and it's never populated.)
    if (payload.has_meta()) {
      meta_transcoder_ = std::make_unique<ProtobufH5RowTranscoder>(payload.meta(), overrides_);
    }

    // Create a serializer for the Channel message type, using the first channel as a prototype
    const auto& channel_prototype = channels.cbegin()->second;
    channel_transcoder_ = std::make_unique<ProtobufH5RowTranscoder>(channel_prototype, overrides_);

    // Create the data sets
    payload_name_ = Payload::descriptor()->name();
    payload_table_ = std::make_unique<HDF5Table>(
        file_,
        channel_transcoder_->GetH5CompType(),
        payload_name_ + "_channels",
        GetChannelIndices(),
        kChunkSize);
    header_table_ = std::make_unique<HDF5Table>(
        file_, header_transcoder_->GetH5CompType(), payload_name_ + "_headers", kChunkSize);
    if (meta_transcoder_) {
      meta_table_ = std::make_unique<HDF5Table>(
          file_, meta_transcoder_->GetH5CompType(), payload_name_ + "_meta", kChunkSize);
    }

    g_reporter->debug("PacketRowLogger::Init: Created dataset {}", payload_name_);

    initialized_ = true;
  }

  void Write(const proto::Packet::Header& header, const Payload& payload) {
    CheckSeqNum(header.sequence_number());

    const auto& channels = payload.channels();

    // Check that no extra channels are present (missing channels detected by channels.at)
    if (channels.size() > channel_indices_.size()) {
      throw std::runtime_error("PacketRowLogger::LogChannels: Extra channels present");
    }

    // Serialize payload into wbuf_ and perform write
    wbuf_.clear();
    header_transcoder_->Serialize(header, wbuf_);
    header_table_->Append(wbuf_);

    // Serialize meta field into wbuf_ and perform write
    if (meta_transcoder_) {
      wbuf_.clear();
      meta_transcoder_->Serialize(payload.meta(), wbuf_);
      meta_table_->Append(wbuf_);
    }

    // Serialize channels field into wbuf_ and perform write
    wbuf_.clear();
    for (auto index : channel_indices_) {
      auto& channel = channels.at(index);
      channel_transcoder_->Serialize(channel, wbuf_);
    }
    payload_table_->Append(wbuf_);
  }

  const std::vector<int>& GetChannelIndices() const {
    return channel_indices_;
  }

  std::vector<google::protobuf::int32> channel_indices_;

  std::unique_ptr<ProtobufH5RowTranscoder> header_transcoder_;
  std::unique_ptr<ProtobufH5RowTranscoder> meta_transcoder_;
  std::unique_ptr<ProtobufH5RowTranscoder> channel_transcoder_;

  std::unique_ptr<HDF5Table> payload_table_;
  std::unique_ptr<HDF5Table> header_table_;
  std::unique_ptr<HDF5Table> meta_table_;
};

/* Logs channeless payload types in a 1D dataset.*/
template <class Payload>
class PacketChannelLessColLogger : public PacketLoggerBase {
 public:
  using PacketLoggerBase::PacketLoggerBase;

  void LogChannelLess(const proto::Packet::Header& header, const Payload& payload) {
    H5LockGuard l;
    if (!initialized_) {
      Init(payload);
    }
    Write(header, payload);
  }

 private:
  hsize_t kChunkSize = 1000;

  void Init(const Payload& payload) {
    // Create a header serializer based on a prototype
    proto::Packet::Header header_prototype;
    header_transcoder_ = std::make_unique<ProtobufH5RowTranscoder>(header_prototype);

    // Create a serializer based on a prototype (the payload)
    // The field sizes of the prototype don't matter like they do for PacketRowLogger.
    payload_transcoder_ = std::make_unique<ProtobufH5RowTranscoder>(payload, overrides_);

    payload_name_ = Payload::descriptor()->name();
    payload_table_ = std::make_unique<HDF5Table>(
        file_, payload_transcoder_->GetH5CompType(), payload_name_, kChunkSize);
    header_table_ = std::make_unique<HDF5Table>(
        file_, header_transcoder_->GetH5CompType(), payload_name_ + "_headers", kChunkSize);
    g_reporter->debug("PacketChannelLessColLogger::InitChannel: Created dataset {}", payload_name_);

    initialized_ = true;
  }

  void Write(const proto::Packet::Header& header, const Payload& payload) {
    CheckSeqNum(header.sequence_number());

    // Serialize header
    wbuf_.clear();
    header_transcoder_->Serialize(header, wbuf_);
    header_table_->Append(wbuf_);

    // Serialize channel into wbuf_
    wbuf_.clear();
    payload_transcoder_->Serialize(payload, wbuf_);

    payload_table_->Append(wbuf_);
  }

  std::unique_ptr<ProtobufH5RowTranscoder> payload_transcoder_;
  std::unique_ptr<ProtobufH5RowTranscoder> header_transcoder_;
  std::unique_ptr<HDF5Table> payload_table_;
  std::unique_ptr<HDF5Table> header_table_;
};

/** Class for creating and writing to multiple 1D HDF5 datasets where each channel is represented as
 * a dataset and each protobuf packet is represented as multiple rows in those datasets.
 * @tparam Payload the protobuf message type this logger is responsible for (eg. proto::Timestamps)
 */
template <class Payload>
class PacketColLogger : public PacketLoggerBase {
 public:
  using PacketLoggerBase::PacketLoggerBase;

  /**
   * Log all channels of a packet to HDF5.
   * @param header The packet header
   * @param payload A single member of the packet oneof payload field
   */
  void LogChannels(const proto::Packet::Header& header, const Payload& payload) {
    H5LockGuard l;
    if (!initialized_) {
      Init(payload);
    }
    Write(header, payload);
  }

 private:
  hsize_t kChunkSize = 1000;

  void Init(const Payload& payload) {
    const auto& channels = payload.channels();
    assert(channels.size() > 0);

    // Create a serializer based on a prototype
    // The field sizes of the prototype don't matter like they do for PacketRowLogger.
    const auto& prototype = channels.cbegin()->second;
    payload_transcoder_ = std::make_unique<ProtobufH5ColTranscoder>(prototype, overrides_);

    payload_name_ = Payload::descriptor()->name();

    initialized_ = true;
  }

  void Write(const proto::Packet::Header& header, const Payload& payload) {
    CheckSeqNum(header.sequence_number());

    for (auto& channel_kv : payload.channels()) {
      auto tables_it = tables_.find(channel_kv.first);
      if (tables_it == tables_.end()) {
        auto name = fmt::format("{}Channel{}", payload_name_, channel_kv.first);
        auto ep_res = tables_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(channel_kv.first),
            std::forward_as_tuple(
                file_,
                payload_transcoder_->GetH5CompType(),
                name,
                std::vector<int>{(int)channel_kv.first},
                kChunkSize));

        tables_it = ep_res.first;
        assert(ep_res.second); // Check insert took place

        g_reporter->debug("PacketColLogger::InitChannel: Created dataset {}", name);
      }

      // Serialize channel into wbuf_
      wbuf_.clear();
      payload_transcoder_->Serialize(channel_kv.second, wbuf_);

      tables_it->second.Append(wbuf_);
    }
  }

 private:
  std::unique_ptr<ProtobufH5ColTranscoder> payload_transcoder_;
  std::map<google::protobuf::int32, HDF5Table> tables_;
};

/** Top level HDF5 logger class.
 * Manages two HDF5 files and the loggers that write to them.
 */
class HDF5Logger {
 public:
  /**
   * @param analyzed_file_name The file name for the HDF5 file containing analyzed data
   * @param peripheral_file_name The file name for the HDF5 file containing peripheral (pandoboxd)
   * data (everything else)
   */
  explicit HDF5Logger(const char* analyzed_file_name, const char* peripheral_file_name);

  /** Pass the packet to the appropriate logger based on which field in the oneof is set.
   * @param packet the packet to log
   */
  void LogPacket(const proto::Packet& packet);

  /** Log configuration details to an H5 file */
  void LogConfig();

 private:
  H5LockAdapter<H5::H5File> analyzed_file_;
  H5LockAdapter<H5::H5File> peripheral_file_;

  bool log_analyzed_{}, log_peripheral_{};

  PacketRowLogger<proto::Counts> count_rate_logger_{analyzed_file_};
  PacketChannelLessColLogger<proto::Event> event_logger_{analyzed_file_};
  PacketRowLogger<proto::G2> g2_logger_{analyzed_file_};
  PacketRowLogger<proto::G2i> g2i_logger_{analyzed_file_};
  PacketRowLogger<proto::Dtof> dtof_logger_{analyzed_file_};
  PacketRowLogger<proto::Cri> cri_logger_{analyzed_file_};
  PacketChannelLessColLogger<proto::PpsStats> pps_stats_logger_{analyzed_file_};
  PacketRowLogger<proto::Power> power_logger_analyzed_{analyzed_file_};
  PacketChannelLessColLogger<proto::HRM> hrm_logger_{peripheral_file_};
  PacketChannelLessColLogger<proto::PulseOx> pulse_ox_logger_{peripheral_file_};
  PacketChannelLessColLogger<proto::RespBelt> resp_belt_logger_{peripheral_file_};
  PacketChannelLessColLogger<proto::AnalogPowerMeter> analog_power_meter_logger_{peripheral_file_};
  PacketChannelLessColLogger<proto::Imu> imu_logger_{peripheral_file_};
  PacketChannelLessColLogger<proto::DigitalIn> digital_in_logger_{peripheral_file_};
};

} // namespace pando
} // namespace pnd
