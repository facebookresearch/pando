#pragma once

#include "device_base.h"
#include "ring_buffer.h"
#include "thread_container.h"
#include "types.h"

#include <fmt/format.h>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <stdexcept>

#include "reporter.h"

namespace pnd {
namespace pando {

class XHarpBase : public DeviceBase {
 public:
  class XHarpException : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  virtual ~XHarpBase() = default;

 protected:
  // 20M records, 10x larger than device buffer
  static constexpr size_t kReadBufferSize = 20 * 1024 * 1024;

  using Record = uint32_t;

  /** The element type of our RingBuffer */
  struct RecordBuffer {
    size_t occupancy = 0; /**< # of records stored in data field */
    Record data[kReadBufferSize]; /**< Array of records */
  };

  /** Start acquiring data */
  void StartAcquisition() final;

  /** Stop Acquiring data */
  void StopAcquisition() final;

  DeviceType GetDeviceType() final {
    return DeviceType::TIMETAGGER;
  }

  virtual void LogStartStopData() {}

  uint64_t GetMicrotimeResolutionPs() final {
    return picosecondsPerTick_;
  }

  void CheckLaserSyncPeriod(long calculated_sync_period_ps) {
    // Check the laser sync period is within +/- 5% of calculated period
    auto ratio = static_cast<double>(abs(
                     static_cast<int>(calculated_sync_period_ps - config_.laser_sync_period_ps))) /
        static_cast<double>(calculated_sync_period_ps);
    if (ratio > 0.05) {
      auto msg = fmt::format(
          "Specified laser sync period {} differs from expected sync period {} by more than 5%.",
          config_.laser_sync_period_ps,
          calculated_sync_period_ps);
      throw std::runtime_error(msg);
    }
  }

  virtual void Configure() = 0;
  virtual void ResetProc() = 0; /**< Reset state of UpdateRawData implementation */
  virtual void Acquire() = 0;

  RingBuffer<RecordBuffer, 3> ring_buffer_;

  common::StopSignal acquisition_stop_signal_;
  common::ThreadContainer acquisition_thread_;
  uint64_t picosecondsPerTick_ = 1;
};

} // namespace pando
} // namespace pnd
