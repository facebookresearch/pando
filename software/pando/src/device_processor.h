#pragma once

#include "correlator.h"
#include "counter.h"
#include "device_base.h"
#include "device_interface.h"
#include "dtof.h"
#include "hdf5_raw_logger.h"
#include "kv_container.h"
#include "pando.h"
#include "pando.pb.h"
#include "singleton_context.h"
#include "thread_container.h"
#include "time_shifter.h"
#include "worker.h"

#include <chrono>
#include <memory>

namespace pnd {
namespace pando {

/** Performs processing of raw data acquired from an instance of a class derived from DeviceBase.
 *   - Calculates g2, g2i, CR, CRi, and DTOFs.
 *   - Protobufs calculation results (and optionally the raw data itself) and publishes it over ZMQ
 *   - Optionally logs raw data to HDF5 (the Pando class logs analyzed data received over ZMQ)
 *   - Optionally shifts timestamps of analyzed data into an external time domain defined by a PPS
 *     signal present in the raw data
 */
class DeviceProcessor : public DeviceInterface {
 public:
  /** Constructor that takes ownership of an injected instance of a DeviceBase descendant. */
  DeviceProcessor(std::unique_ptr<DeviceBase> device);

 private:
  static constexpr size_t kPpsMarkerChIdx = 0;

  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final;
  void StopImpl() final;

  /** Thread function that processes and publishes data obtained from UpdateRawData implementation.
   */
  void Run();
  void Process(
      DeviceBase::RawData& raw_data,
      const PandoInterface::Config& dynamic_config,
      size_t begin_bin_idx,
      size_t end_bin_idx);

  void HandlePps(const DeviceBase::RawData& raw_data);

  proto::Packet* ProtobufTimetags(
      const DeviceBase::RawData& raw_data,
      google::protobuf::Arena* arena,
      std::chrono::nanoseconds timestamp);
  proto::Packet* ProtobufHistograms(
      const DeviceBase::RawData& raw_data,
      google::protobuf::Arena* arena,
      std::chrono::nanoseconds timestamp);
  proto::Packet* ProtobufMarkers(
      const DeviceBase::RawData& raw_data,
      google::protobuf::Arena* arena,
      std::chrono::nanoseconds timestamp);
  proto::Packet* ProtobufDtofs(google::protobuf::Arena* arena);
  proto::Packet* ProtobufCris(google::protobuf::Arena* arena);
  proto::Packet* ProtobufCountsFromTimetags(google::protobuf::Arena* arena);
  proto::Packet* ProtobufCountsFromHistograms(google::protobuf::Arena* arena);
  proto::Packet* ProtobufG2s(google::protobuf::Arena* arena, std::chrono::nanoseconds timestamp);
  proto::Packet* ProtobufG2is(
      google::protobuf::Arena* arena,
      PandoInterface::Config dynamic_config,
      std::chrono::nanoseconds timestamp);

  void PublishZmqMessage(const char* topic, size_t topic_size, zmq::message_t& payload_msg);
  void SendZmqEvent(proto::EventType event_type, int64_t event_seq, int64_t timestamp);

  static MacroTimes
  DownsampleMacroTimes(const ChannelTimestamps& timestamps, MicroTime from, MicroTime until);

  std::shared_ptr<zmq::context_t> context_{common::SingletonContext::Get()};
  zmq::socket_t pub_socket_{*context_, zmq::socket_type::pub};

  PandoInterface::Config config_;

  std::unique_ptr<DeviceBase> device_;
  common::SessionHandle device_session_handle_;

  KVContainer<int32_t, Correlator<std::chrono::nanoseconds>> correlators_;

  // Used to get different integration periods for counts and dtof
  KVContainer<int32_t, TimestampCounter> ts_counters_;
  KVContainer<int32_t, HistogramCounter> hist_counters_;
  KVContainer<int32_t, ROICounter> cri_counters_;
  int64_t count_seq_ = 0;
  int64_t dtof_seq_ = 0;
  int64_t cri_seq_ = 0;
  int64_t marker_seq_ = 0;

  KVContainer<int32_t, Dtof> dtofs_;

  Worker raw_data_pb_worker_{"DeviceProcessor::raw_data_pb_worker_"};
  Worker raw_data_log_worker_{"DeviceProcessor::raw_data_log_worker_"};
  Worker zmq_publish_worker_{"DeviceProcessor::zmq_publish_worker_"};

  std::unique_ptr<HDF5RawLogger> raw_logger_;

  common::StopSignal run_stop_signal_;
  common::ThreadContainer run_thread_;
  int32_t experiment_id_ = 0; // will be set in Start() method for each experiment
  int64_t g2_seq_ = 0; // will be set in Start() method for each experiment
  int64_t event_seq_ = 0; // will be reset in Start()method for each experiement
  int64_t pps_seq_ = 0;

  TimeShifter time_shifter_;
};
} // namespace pando
} // namespace pnd
