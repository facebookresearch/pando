

#pragma once

#include <chrono>
#include <memory>

namespace pnd {
namespace pando {

/** Container for a single timestamped raw camera frame. */
struct CameraFrame {
  std::shared_ptr<void> data; /**< The raw image data. */
  std::size_t size; /**< The size of the image (in bytes). */
  std::chrono::nanoseconds timestamp;
  std::chrono::duration<double, std::micro> exposure_time;
};

} // namespace pando
} // namespace pnd
