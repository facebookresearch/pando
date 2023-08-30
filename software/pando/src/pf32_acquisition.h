#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>

#include "pf32_ll_types.h"
#include "reporter.h"
#include "spin_lock.h"
#include "storage_pool.h"
#include "thread_container.h"
#include "thread_prio.h"
#include "types.h"

namespace pnd {
namespace pando {
namespace pf32_ll {
/** Class representing a single continuous acquisition. */
template <typename BitMode>
class Acquisition {
 private:
  /** Number of frames to read at a time. */
  static constexpr size_t kFramesPerRead = 10 * 4096;
  /** Maximum number of outstanding deinterleave jobs. */
  static constexpr size_t kMaxDeinterleaveParallelism = 8;
  /** Maximum amount of time to wait for new frames.  */
  static constexpr std::chrono::milliseconds kMaxExcessDelay{500};
  /** Number of marker channels on PF32 */
  static constexpr size_t kMarkerChannelCount = 3;

 public:
  class PF32AcquisitionException : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  struct alignas(64) Frames {
    /** Sequence number of the first frame held in frames (starts at 0). */
    size_t begin_frame_idx;

    /** Sequence number of the "Past the end" frame (not) held in frames. */
    size_t end_frame_idx;

    /** 2D arrays where samples from a given channel are contiguous. */
    std::array<std::array<typename BitMode::Pixel, kFramesPerRead>, kChannelCount> pixels;
    std::array<std::array<bool, kFramesPerRead>, kMarkerChannelCount> markers;
  };

  Acquisition(
      Handle handle,
      std::chrono::nanoseconds frame_period,
      api::PF32_mode mode,
      const std::list<int32_t>& enabled_channels)
      : handle_(handle) {
    // Check that the correct firmware is loaded
    if (api::getBitMode(handle_.get()) != BitMode::kBitsPerPixel) {
      throw PF32AcquisitionException(
          "Loaded firmware does not return expected # of bits per pixel");
    }

    // Set the mode
    api::setMode(handle_.get(), mode);

    // Enable frame footers
    api::setEnablePositionalData(handle_.get(), true);
    if (!api::getEnablePositionalData(handle_.get()))
      throw std::runtime_error(
          "pf32_ll::Acquisition: Frame footers could not be enabled, so we have no way to detect corrupted or dropped frames. Please contact Photon Force about unlocking the positional data feature on this camera.");

    // Set the frame period
    auto frame_period_us =
        std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(frame_period);
    api::setExposure_us(handle_.get(), frame_period_us.count());
    api::setFramesToSum(handle_.get(), 1);

    // Read back the frame period
    frame_period_ps_ = std::chrono::round<picoseconds>(
        std::chrono::duration<double, std::micro>{api::getExposure_us(handle_.get())});
    if (frame_period_ps_ != frame_period) {
      g_reporter->debug(
          "pf32_ll::Acquisition: Reported frame period is {}ps, not {}ns as requested",
          frame_period_ps_.count(),
          frame_period.count());
      throw std::runtime_error("pf32_ll::Acquisition: Requested frame period was not achieved");
    }

    // Select enabled pixels
    enabled_rows_.fill(false);
    enabled_cols_.fill(false);
    for (auto ch : enabled_channels) {
      if (ch < 0 || ch >= kChannelCount) {
        throw std::out_of_range(
            "pf32_ll::Acquisition::SetEnabledChannels: invalid channel specified by user");
      }
      enabled_rows_[ch / kRowColCount] = true;
      enabled_cols_[ch % kRowColCount] = true;
    }

    // the ROI must include at least one row in both the top and bottom halves of the frame, or
    // else frames are corrupted and reads eventually fail completely. As a workaround, force
    // enable row 0 or 16 (or both) if doing so avoids the bug condition.
    if (std::none_of(enabled_rows_.cbegin(), enabled_rows_.cbegin() + kRowColCount / 2, [](bool v) {
          return v;
        })) {
      g_reporter->warn("pf32_ll::Acquisition: enabling row 0 to work around vendor bug");
      enabled_rows_[0] = true;
    }

    if (std::none_of(enabled_rows_.cbegin() + kRowColCount / 2, enabled_rows_.cend(), [](bool v) {
          return v;
        })) {
      g_reporter->warn("pf32_ll::Acquisition: enabling row 16 to work around vendor bug");
      enabled_rows_[kRowColCount / 2] = true;
    }

    api::setRegionsOfInterest(handle_.get(), enabled_cols_.data(), enabled_rows_.data());

    // Log enabled rows/cols
    char enabled_rows_s[kRowColCount + 1] = {0};
    char enabled_cols_s[kRowColCount + 1] = {0};
    for (auto idx = 0; idx < kRowColCount; ++idx) {
      enabled_rows_s[idx] = enabled_rows_[idx] ? '1' : '0';
      enabled_cols_s[idx] = enabled_cols_[idx] ? '1' : '0';
    }
    g_reporter->debug("pf32_ll::Acquisition: Enabled rows: {}", enabled_rows_s);
    g_reporter->debug("pf32_ll::Acquisition: Enabled cols: {}", enabled_cols_s);

    // Sanity checks
    assert(
        std::count(enabled_rows_.begin(), enabled_rows_.end(), true) ==
        api::getEnabledHeight(handle_.get()));
    assert(api::getWidth(handle_.get()) == kRowColCount);
    assert(api::getHeight(handle_.get()) == kRowColCount);

    // Calculate the size of each frame returned by getNextFrames_short
    frame_size_ =
        (api::getEnabledHeight(handle_.get()) * kRowColCount * sizeof(typename BitMode::Pixel)) /
            BitMode::kPixelsPerWord +
        sizeof(FrameFooter);
    g_reporter->debug("pf32_ll::Acquisition: frame_size_ is {} bytes", frame_size_);

    // Spew a bunch of debug information
    g_reporter->debug(
        "pf32_ll::Acquisition: getSensorClk_Hz -> {}", api::getSensorClk_Hz(handle_.get()));
    g_reporter->debug("pf32_ll::Acquisition: getSync_Hz -> {}", api::getSync_Hz(handle_.get()));
    g_reporter->debug(
        "pf32_ll::Acquisition: getLinkStatus -> {}", api::getLinkStatus(handle_.get()));
    g_reporter->debug(
        "pf32_ll::Acquisition: getExposure_us -> {}", api::getExposure_us(handle_.get()));
    g_reporter->debug(
        "pf32_ll::AcquisitionG2: getSensorClk_Hz -> {}", api::getSensorClk_Hz(handle_.get()));
    g_reporter->debug(
        "pf32_ll::AcquisitionG2: getBitsPerLine -> {}", api::getBitsPerLine(handle_.get()));
    g_reporter->debug(
        "pf32_ll::AcquisitionG2: getLinesPerFrame -> {}", api::getLinesPerFrame(handle_.get()));
    g_reporter->debug(
        "pf32_ll::Acquisition: getNoOfPixels -> {}", api::getNoOfPixels(handle_.get()));
    g_reporter->debug(
        "pf32_ll::Acquisition: getFramesToSum -> {}", api::getFramesToSum(handle_.get()));
    g_reporter->debug(
        "pf32_ll::Acquisition: getNoOfFramesInBuffer -> {}",
        api::getNoOfFramesInBuffer(handle_.get()));
    g_reporter->debug(
        "pf32_ll::Acquisition: getSPADEnable -> {}", api::getSPADEnable(handle_.get()));
    auto serial_number = std::make_unique<char[]>(api::MAX_SERIAL_NUMBER_LENGTH);
    api::GetSerialNumber(handle_.get(), serial_number.get());
    g_reporter->debug("pf32_ll::Acquisition: getSerialNumber -> {}", serial_number.get());
    g_reporter->debug(
        "pf32_ll::Acquisition: getNoOfTDCCodes -> {}", api::getNoOfTDCCodes(handle_.get()));

    common::thread_prio::SetProcessPriorityClass(common::thread_prio::PriorityClass::kRealtime);

    // Launch processing thread
    process_thread_ = common::ThreadContainer(
        [&] { Process(); }, "pf32_ll::Acquisition::Process", &process_stop_signal_);

    // Launch acquisition thread
    acquisition_thread_ = common::ThreadContainer(
        [&] { Acquire(); },
        "pf32_ll::Acquisition::Acquire",
        &acquisition_stop_signal_,
        common::thread_prio::ThreadPriority::kTimeCritical);
  }

  /** @return The configured frame period. */
  picoseconds GetFramePeriod() const {
    return frame_period_ps_;
  }

  storage_pool::FactoryHandle<Frames> GetFrames() {
    std::future<storage_pool::FactoryHandle<Frames>> fut;

    // Wait until a new future is available
    auto max_delay = frame_period_ps_ * kFramesPerRead + kMaxExcessDelay;
    {
      std::unique_lock<std::mutex> l{deint_futures_m};
      deint_futures_cv_.wait_for(l, max_delay, [&] { return !deint_futures_.empty(); });

      if (deint_futures_.empty()) {
        throw PF32AcquisitionException(
            "Acquisition::GetFrames: blocked for >500ms longer than requested window time.");
      }

      fut = std::move(deint_futures_.front());
      deint_futures_.pop();
    }

    // Wait until the future holds a result
    return fut.get();
  }

 private:
  static constexpr size_t kMaxFrameSize =
      (sizeof(typename BitMode::Pixel) * kChannelCount) / BitMode::kPixelsPerWord +
      sizeof(FrameFooter);
  struct alignas(64) RawFrames : std::array<uint8_t, kMaxFrameSize * kFramesPerRead> {
    uint64_t seq_num;
  };

  /** Helper function to seek to a frame in a buffer */
  const uint8_t* GetFrame(const uint8_t* buffer, size_t index) const {
    return buffer + frame_size_ * index;
  }

  /** Helper function to access a single frame footer in a buffer of frames*/
  const FrameFooter* GetFooter(const uint8_t* frame) const {
    return reinterpret_cast<const FrameFooter*>(frame + frame_size_ - sizeof(FrameFooter));
  }

  /** Thread function that continuously acquires raw frames from the PF32. */
  void Acquire() {
    for (uint64_t seq_num = 0; !acquisition_stop_signal_.ShouldStop(); ++seq_num) {
      // Get some pre-allocated space to read data into
      storage_pool::FactoryHandle<RawFrames> raw_frames;
      try {
        raw_frames = raw_frames_storage_pool_.CreateForOverwrite();
      } catch (const std::bad_alloc&) {
        throw PF32AcquisitionException("Software buffer overflow (Deinterleaving too slow)");
      }

      // Read kFramesPerRead frames from the camera into raw_frames
      auto ret = api::getNextFrames(
          handle_.get(), raw_frames->data(), kFramesPerRead, false, seq_num == 0);
      if (!ret) {
        throw PF32AcquisitionException("getNextFrames returned false");
      }

      raw_frames->seq_num = seq_num;

      {
        // We expect to acquire the lock without blocking
        std::unique_lock l{raw_frames_m_, std::try_to_lock};
        if (!l) {
          g_reporter->warn("Waiting on pf32_ll::Process, this may cause dropped frames");
          l.lock();
        }

        if (raw_frames_) {
          throw PF32AcquisitionException("pf32_ll::Process didn't consume raw_frames_");
        }

        raw_frames_ = std::move(raw_frames);
      }

      raw_frames_cv_.notify_one();
    }
  }

  void Process() {
    auto max_delay = frame_period_ps_ * kFramesPerRead + kMaxExcessDelay;

    std::unique_lock l{raw_frames_m_};

    while (true) {
      if (raw_frames_cv_.wait_for(
              l, max_delay, [this]() { return static_cast<bool>(raw_frames_); })) {
        ProcessRawFrames(std::move(raw_frames_));
      } else if (process_stop_signal_.ShouldStop()) {
        break;
      }
    }
  }

  void ProcessRawFrames(storage_pool::FactoryHandle<RawFrames> raw_frames) {
    storage_pool::FactoryHandle<Frames> deinterleaved_frames;
    try {
      deinterleaved_frames = deint_frames_storage_pool_.CreateForOverwrite();
    } catch (const std::bad_alloc&) {
      throw PF32AcquisitionException("Software buffer overflow (Processing too slow)");
    }

    deinterleaved_frames->begin_frame_idx = raw_frames->seq_num * kFramesPerRead;
    deinterleaved_frames->end_frame_idx = (raw_frames->seq_num + 1) * kFramesPerRead;

    // The very first frame from the camera has a nonzero frame_count field
    if (raw_frames->seq_num == 0) {
      first_frame_count_ = GetFooter(raw_frames->data())->frame_count;
    }

    // Serially inspect every frame footer
    for (size_t frame_idx = 0; frame_idx < kFramesPerRead; ++frame_idx) {
      auto frame = GetFrame(raw_frames->data(), frame_idx);

      // Check for dropped frames
      auto footer = GetFooter(frame);
      FrameCount expected_frame_count =
          first_frame_count_ + raw_frames->seq_num * kFramesPerRead + frame_idx;
      if (footer->frame_count != expected_frame_count) {
        g_reporter->debug(
            "pf32_ll::Acquisition::ProcessRawFrames expected_frame_count={} actual={} seq_num={} frame_idx={}",
            expected_frame_count,
            footer->frame_count,
            raw_frames->seq_num,
            frame_idx);
        throw PF32AcquisitionException("Detected dropped frames (Acquisition too slow)");
      }

      // Extract marker channel events
      if (footer->frame_count != first_frame_count_) {
        deinterleaved_frames->markers[2][frame_idx] = x_ != footer->x_val;
        deinterleaved_frames->markers[1][frame_idx] = y_ != footer->y_val;
        deinterleaved_frames->markers[0][frame_idx] = z_ != footer->z_val;
      } else {
        deinterleaved_frames->markers[2][frame_idx] = false;
        deinterleaved_frames->markers[1][frame_idx] = false;
        deinterleaved_frames->markers[0][frame_idx] = false;
      }
      x_ = footer->x_val;
      y_ = footer->y_val;
      z_ = footer->z_val;
    }

    // Launch asynchronous & parallel deinterleaving of the raw data
    auto f = std::async(
        std::launch::async,
        &Acquisition::Deinterleave,
        this,
        std::move(raw_frames),
        std::move(deinterleaved_frames));

    // Queue the future
    {
      std::unique_lock<std::mutex> l{deint_futures_m};
      deint_futures_.push(std::move(f));
    }
    deint_futures_cv_.notify_one();
  }

  storage_pool::FactoryHandle<Frames> Deinterleave(
      storage_pool::FactoryHandle<RawFrames> raw_frames,
      storage_pool::FactoryHandle<Frames> deinterleaved_frames) const {
    // Deinterleave frames from raw_frames into deinterleaved_frames
    size_t row_offset = 0;
    for (size_t row_idx = 0; row_idx < kRowColCount; ++row_idx) {
      if (!enabled_rows_[row_idx]) {
        continue;
      }

      std::array<typename BitMode::Pixel, kRowColCount> row_buff;
      for (size_t frame_idx = 0; frame_idx < kFramesPerRead; ++frame_idx) {
        auto frame = GetFrame(raw_frames->data(), frame_idx);
        auto row = reinterpret_cast<const typename BitMode::Pixel*>(frame) + row_offset;

        for (size_t col_idx = 0; col_idx < kRowColCount; ++col_idx) {
          // "The upper bits are the leftmost pixel, and the lower bits are the rightmost pixel. For
          // example in 4-bit mode pixel 0 is the 4 MSBs and pixel 1 is the 4 LSBs." -- Richard
          // Walker
          auto word = row[col_idx / BitMode::kPixelsPerWord];
          auto offset = (sizeof(typename BitMode::Pixel) * 8 - BitMode::kBitsPerPixel) -
              BitMode::kBitsPerPixel * (col_idx % BitMode::kPixelsPerWord);
          auto pixel = (word >> offset) & ((1 << BitMode::kBitsPerPixel) - 1);

          deinterleaved_frames->pixels[row_idx * kRowColCount + col_idx][frame_idx] = pixel;
        }
      }
      row_offset += kRowColCount / BitMode::kPixelsPerWord;
    }

    return deinterleaved_frames;
  }

  storage_pool::Factory<RawFrames> raw_frames_storage_pool_{kMaxDeinterleaveParallelism};
  storage_pool::Factory<Frames> deint_frames_storage_pool_{kMaxDeinterleaveParallelism};

  std::mutex deint_futures_m;
  std::condition_variable deint_futures_cv_;
  std::queue<std::future<storage_pool::FactoryHandle<Frames>>> deint_futures_;

  Handle handle_;
  picoseconds frame_period_ps_;
  size_t frame_size_;
  std::array<bool, kRowColCount> enabled_rows_, enabled_cols_;

  uint16_t x_ = 0;
  uint16_t y_ = 0;
  uint16_t z_ = 0;

  FrameCount first_frame_count_;

  // Used for passing raw frames from acquisition_thread_ to process_thread_
  storage_pool::FactoryHandle<RawFrames> raw_frames_;
  common::SpinLock raw_frames_m_;
  std::condition_variable_any raw_frames_cv_;

  common::StopSignal process_stop_signal_;
  common::ThreadContainer process_thread_;

  common::StopSignal acquisition_stop_signal_;
  common::ThreadContainer acquisition_thread_;
};

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
