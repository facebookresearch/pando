#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace pnd {
namespace pando {

struct Me5FrameGrabber {
  struct FgLibException : std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  virtual ~Me5FrameGrabber() = default;

  virtual void Start(
      int32_t experiment_id,
      bool log_raw_data,
      bool publish_raw_data,
      const char* raw_file_name,
      uint32_t width,
      uint32_t height,
      std::chrono::nanoseconds frame_period,
      std::chrono::duration<double, std::micro> exposure_time) = 0;

  virtual void Stop() = 0;

  static std::unique_ptr<Me5FrameGrabber> Create(uint32_t board_index);
};

} // namespace pando
} // namespace pnd
