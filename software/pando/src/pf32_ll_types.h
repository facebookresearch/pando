#pragma once

#include <cstdint>
#include <memory>

namespace pnd {
namespace pando {
namespace pf32_ll {

namespace api {
#include "pf32_api/PF32_API.h"
#include "pf32_api/PF32_CORRELATOR_API.h"
} // namespace api

// PF32_HANDLE is just typedef void*
// We'll wrap the void* in a shared_ptr with PF32_destruct as a custom deleter.
using Handle = std::shared_ptr<std::remove_pointer_t<api::PF32_HANDLE>>;

constexpr size_t kRowColCount = 32; /**< Number of rows/cols */
constexpr size_t kChannelCount = kRowColCount * kRowColCount; /**< Number of pixels */

/** Type used for frame index */
using FrameCount = uint32_t;

/** Struct representing optional per-frame footer. */
struct FrameFooter {
  uint16_t reserved_1;
  uint16_t reserved_2;
  FrameCount frame_count;
  uint16_t scanner_status;
  uint16_t x_val;
  uint16_t y_val;
  uint16_t z_val;
};
static_assert(
    sizeof(FrameFooter) == api::NO_OF_BYTES_PER_POSITIONAL_FOOTER,
    "sizeof(FrameFooter) !== NO_OF_BYTES_PER_POSITIONAL_FOOTER");

/** Exports the values of enum PF32_mode from api namespace. */
struct Mode {
  static constexpr auto photon_counting = api::photon_counting;
  static constexpr auto TCSPC_laser_master = api::TCSPC_laser_master;
  static constexpr auto TCSPC_sys_master = api::TCSPC_sys_master;
  static constexpr auto raw_SPAD = api::raw_SPAD;
  static constexpr auto test_pulse_counting = api::test_pulse_counting;
  static constexpr auto test_data_1 = api::test_data_1;
  static constexpr auto test_data_2 = api::test_data_2;
};

/** Tag type specifying 4 bit mode */
struct BitMode4 {
  using Pixel = uint8_t;
  static constexpr size_t kBitsPerPixel = 4;
  static constexpr size_t kPixelsPerWord = 2;
};

/** Tag type specifying 8 bit mode */
struct BitMode8 {
  using Pixel = uint8_t;
  static constexpr size_t kBitsPerPixel = 8;
  static constexpr size_t kPixelsPerWord = 1;
};

/** Tag type specifying 16 bit mode */
struct BitMode16 {
  using Pixel = uint16_t;
  static constexpr size_t kBitsPerPixel = 16;
  static constexpr size_t kPixelsPerWord = 1;
};

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
