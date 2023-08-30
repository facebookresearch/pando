#pragma once

#include "device_interface.h"
#include "hdf5_raw_logger.h"
#include "pando.h"
#include "pando.pb.h"
#include "pf32_acquisition_g2.h"
#include "pf32_ll.h"
#include "publisher.h"
#include "thread_container.h"
#include "time_shifter.h"
#include "types.h"
#include "worker.h"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

/** A device class for the PF32 running in the hardware-correlator mode.
 * Unlike other device classes, this one doesn't inherit from DeviceBase. It essentially protobufs
 * data produced by pf32_ll::PF32AcquisitionG2.
 */
class PF32G2 : public DeviceInterface {
 public:
  class Exception : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  PF32G2(const char* firmware_path);

 private:
  static constexpr std::size_t kPpsMarkerChIdx = 0;

  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final;
  void StopImpl() final;

  void Run();
  void Process(const pf32_ll::AcquisitionG2::CorrelatorResult& result);

  void ProtobufAndPublishG2(
      const pf32_ll::AcquisitionG2::CorrelatorResult& result,
      MacroTime timestamp);
  void ProtobufAndPublishMarkers(
      const pf32_ll::AcquisitionG2::CorrelatorResult& result,
      MacroTime timestamp);

  void ProtobufAndPublishCounts(
      const pf32_ll::AcquisitionG2::CorrelatorResult& result,
      MacroTime timestamp);

  void LogMarkers(const pf32_ll::AcquisitionG2::CorrelatorResult& result, MacroTime timestamp);

  void HandlePps(const pf32_ll::AcquisitionG2::CorrelatorResult& result);

  PandoInterface::Config config_;
  int32_t experiment_id_ = 0;
  int64_t g2_seq_ = 0;
  int64_t count_seq_ = 0;
  int64_t event_seq_ = 0;
  int64_t marker_seq_ = 0;
  int64_t pps_seq_ = 0;

  Publisher publisher_;

  pf32_ll::PF32LL pf32_ll_;
  std::unique_ptr<pf32_ll::AcquisitionG2> acquisition_;

  std::unique_ptr<HDF5RawLogger> raw_logger_;

  proto::Packet g2_packet_;
  proto::Packet counts_packet_;

  common::StopSignal run_stop_signal_;
  common::ThreadContainer run_thread_;

  TimeShifter time_shifter_;
};

} // namespace pando
} // namespace pnd
