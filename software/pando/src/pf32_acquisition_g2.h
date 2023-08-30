#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>

#include "pf32_ll_types.h"
#include "storage_pool.h"
#include "thread_container.h"
#include "types.h"

namespace pnd {
namespace pando {
namespace pf32_ll {
/** Class representing a single continuous acquisition using native g2 calculation mode. */
class AcquisitionG2 {
 private:
  /** Maximum number of outstanding CorrelatorResult to buffer for caller of GetResult. */
  static constexpr std::size_t kResultBufferDepth = 3;
  /** Maximum amount of time to wait for new frames.  */
  static constexpr std::chrono::milliseconds kMaxExcessDelay{500};

  /** Maximum number of tau values per pixel */
  static constexpr std::size_t kMaxNPoints = 64;

  /** 1 indexed tau indicies of the first point in each rebinning region.
   *
   * From the coorelator user guide v1.3:
   * At  the  moment  there  aretwo  rebinning  points  in  the  Artix-7  32-Tau implementation,
   * and four  rebinning  pointsin  the  Artix-7  64  Tau  implementation.  Before  the  first
   * rebinning point the rebinning width will always be 1. Regions are between the following range
   * of tau values: [9:24], [25:40], [41:56] and [56:60].
   */
  static constexpr std::array<std::size_t, 4> kRebinningIndices{8, 24, 40, 56};

 public:
  /** Number of marker channels on PF32 */
  static constexpr std::size_t kMarkerChannelCount = 4;
  static constexpr uint32_t kFixedPattern1 = 0xAAAAAAAA;
  static constexpr uint32_t kFixedPattern2 = 0x55555555;

  class Exception : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  struct FrameFooterG2 {
    uint32_t first_frame_id; /**< Frame ID of the first frame in the integration period. */
    uint32_t markers; /** Bitmask of flags. Bit 3: BLK, Bit 2: FRM, Bit 1: LINE, Bit 0: PIXEL. */
    uint32_t tint_n_frames; /**< The length (in frames) of the t_int. */
    uint32_t fixed_pattern_1; /**< Always 0xAAAAAAAA, for now. */
    uint32_t fixed_pattern_2; /**< Always 0x55555555, for now. */
    uint32_t correlation_id; /**< Integration period sequence number (starts at 1). */
    uint32_t _reserved[250]; /**< Always all-zeros, for now. */
  };

  static_assert(
      sizeof(FrameFooterG2) == api::SIZE_OF_CORRELATOR_FOOTER_BYTES,
      "sizeof(FrameFooterG2) !== SIZE_OF_CORRELATOR_FOOTER_BYTES");

  struct RawCorrelatorResult {
    static constexpr std::size_t kMaxCorrelatorDataSize =
        kMaxNPoints * kChannelCount * sizeof(uint32_t);

    std::array<uint8_t, kMaxCorrelatorDataSize> correlator_data;
    FrameFooterG2 footer;
    std::array<uint32_t, kChannelCount> intensity_map;
  };

  struct alignas(64) CorrelatorResult {
    /** Sequence number of the first frame in the integration period (starts at 0). */
    std::size_t begin_frame_idx;

    /** The number of frames used to compute the result (ie. the integration period). */
    std::size_t frame_count;

    /** The number of valid points stored in fields tau_k and g2. */
    std::size_t n_points;

    /** g2 X values (lag times). */
    std::array<std::chrono::nanoseconds, kMaxNPoints> tau_k;

    /** g2 Y values.
     * Indices are like so: g2[row_idx][tau_idx][col_idx]
     */
    std::array<std::array<std::array<double, kRowColCount>, kMaxNPoints>, kRowColCount> g2;

    /** Per-pixel photon counts. */
    std::array<uint32_t, kChannelCount> intensity_map;

    std::array<bool, kMarkerChannelCount> markers;
  };

  AcquisitionG2(
      Handle handle,
      std::chrono::nanoseconds frame_period,
      unsigned long frame_count,
      bool burst_mode,
      std::array<int, 4> rebin_factors,
      const std::list<int32_t>& enabled_channels);

  /** @return The configured frame period. */
  picoseconds GetFramePeriod() const {
    return frame_period_ps_;
  }

  storage_pool::FactoryHandle<CorrelatorResult> GetResult();

  /** Reimplementation of api::convertCorrelatorOutput.
   * Should behave identically, but our build uses AVX2 so this is 2-4x faster.
   * See benchmarking & testing in pf32_acquisition_g2_test.
   */
  static void ConvertCorrelatorOutput(
      const uint8_t* raw,
      double* converted,
      std::size_t enabledHeight,
      std::size_t width);

 private:
  /** Thread function that continuously acquires raw correlator results from the PF32. */
  void Acquire();

  storage_pool::FactoryHandle<CorrelatorResult> ProcessRawResult(
      storage_pool::FactoryHandle<RawCorrelatorResult> raw_result);

  storage_pool::Factory<RawCorrelatorResult> raw_results_storage_pool_{kResultBufferDepth};
  storage_pool::Factory<CorrelatorResult> results_storage_pool_{kResultBufferDepth};

  std::queue<std::future<storage_pool::FactoryHandle<CorrelatorResult>>> result_futures_;
  std::mutex result_futures_m_;
  std::condition_variable result_futures_cv_;

  Handle handle_;
  picoseconds frame_period_ps_;
  std::size_t frame_size_;
  std::size_t enabled_height_;
  std::size_t n_points_;
  unsigned int spacing_;
  std::array<bool, kRowColCount> enabled_rows_, enabled_cols_;
  std::size_t frame_count_;
  std::array<int, 4> rebin_factors_;

  FrameCount first_first_frame_id_;
  decltype(FrameFooterG2::markers) markers_prev_{};

  common::StopSignal acquisition_stop_signal_;
  common::ThreadContainer acquisition_thread_;
};

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
