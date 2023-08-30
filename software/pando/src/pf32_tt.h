#pragma once

#include "device_base.h"
#include "pf32_acquisition.h"
#include "pf32_ll.h"
#include "storage_pool.h"

#include <cassert>
#include <chrono>
#include <memory>
#include <stdexcept>

namespace pnd {
namespace pando {
template <typename BitMode>
class PF32Tt : public DeviceBase {
 public:
  PF32Tt(const char* firmware_path) : pf32_ll_(firmware_path) {}

 private:
  static constexpr picoseconds kMicrotimeResolution{55};

  void StartAcquisition() final {
    std::chrono::nanoseconds frame_period(config_.bin_size_ns);
    auto mode = (config_.pf32_laser_master) ? pf32_ll::Mode::TCSPC_laser_master
                                            : pf32_ll::Mode::TCSPC_sys_master;

    acquisition_ =
        pf32_ll_.CreateAcquisition<BitMode>(frame_period, mode, config_.enabled_channels);
  }

  void StopAcquisition() final {
    frames_.reset();
    acquisition_.reset();
  }

  DeviceType GetDeviceType() final {
    return DeviceType::TIMETAGGER;
  }

  bool DeviceGeneratesMicrotimes() override {
    return true;
  }

  uint64_t GetMicrotimeResolutionPs() final {
    return kMicrotimeResolution.count();
  }

  void UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) final {
    assert(acquisition_);

    std::chrono::nanoseconds bin_size{config_.bin_size_ns};
    auto from = bin_size * begin_bin_idx;
    auto until = bin_size * end_bin_idx;

    for (auto& time_tags : dest.timestamps) {
      // Clear the macro and micro time vectors for each channel without deallocating any memory.
      time_tags.second.macro_times().clear();
      time_tags.second.micro_times().clear();

      // Record the span of the timestamps we're about to store
      time_tags.second.SetSpan(from, until);

      time_tags.second.macro_times().reserve(end_bin_idx - begin_bin_idx);
      time_tags.second.micro_times().reserve(end_bin_idx - begin_bin_idx);
    }

    for (auto& markers : dest.marker_timestamps) {
      // Clear the macro and micro time vectors for each channel without deallocating any memory.
      markers.second.macro_times().clear();
      markers.second.micro_times().clear();

      // Record the span of the timestamps we're about to store
      markers.second.SetSpan(from, until);

      markers.second.macro_times().reserve(end_bin_idx - begin_bin_idx);
      markers.second.micro_times().reserve(end_bin_idx - begin_bin_idx);
    }

    // Prime frames_ (Necessary in the first call to UpdateRawData in an experiment)
    if (!frames_) {
      frames_ = acquisition_->GetFrames();
    }

    size_t chunk_begin_bin_idx = begin_bin_idx;
    size_t chunk_end_bin_idx = begin_bin_idx;
    while (chunk_begin_bin_idx < end_bin_idx) {
      // Pop new buffers until frames_ contains chunk_begin_bin_idx
      while (chunk_begin_bin_idx >= frames_->end_frame_idx) {
        frames_ = acquisition_->GetFrames();
      }
      assert(chunk_begin_bin_idx >= frames_->begin_frame_idx);

      // The (last + 1) bin index to process in this loop iteration
      chunk_end_bin_idx = std::min(end_bin_idx, frames_->end_frame_idx);

      // The array indices (distinct from bin indices) of raw frames in frames_ to process in this
      // loop iteration
      auto src_begin_idx = chunk_begin_bin_idx - frames_->begin_frame_idx;
      auto src_end_idx = chunk_end_bin_idx - frames_->begin_frame_idx;

      for (auto& timestamps_kv : dest.timestamps) {
        auto ch_idx = timestamps_kv.first;
        auto& timestamps = timestamps_kv.second;

        assert(src_end_idx <= frames_->pixels[ch_idx].size());

        for (auto buf_idx = src_begin_idx; buf_idx < src_end_idx; ++buf_idx) {
          const auto& pixel = frames_->pixels[ch_idx][buf_idx];
          if (pixel > 0) {
            timestamps.macro_times().emplace_back(
                (frames_->begin_frame_idx + buf_idx) * acquisition_->GetFramePeriod());
            timestamps.micro_times().emplace_back(
                picoseconds{config_.laser_sync_period_ps} - pixel * kMicrotimeResolution);
          }
        }
      }

      // Segment marker events
      for (auto& timestamps_kv : dest.marker_timestamps) {
        auto ch_idx = timestamps_kv.first;
        auto& timestamps = timestamps_kv.second;

        if (ch_idx >= frames_->markers.size())
          continue;

        assert(src_end_idx <= frames_->markers[ch_idx].size());

        for (auto buf_idx = src_begin_idx; buf_idx < src_end_idx; ++buf_idx) {
          if (frames_->markers[ch_idx][buf_idx]) {
            timestamps.macro_times().emplace_back(
                (frames_->begin_frame_idx + buf_idx) * acquisition_->GetFramePeriod());
            timestamps.micro_times().emplace_back(picoseconds{0});
          }
        }
      }

      chunk_begin_bin_idx = chunk_end_bin_idx;
    }

    assert(chunk_end_bin_idx == end_bin_idx);
  }

  pf32_ll::PF32LL pf32_ll_;
  std::unique_ptr<pf32_ll::Acquisition<BitMode>> acquisition_;
  storage_pool::FactoryHandle<typename pf32_ll::Acquisition<BitMode>::Frames> frames_;
};

using PF32Tt4Bit = PF32Tt<pf32_ll::BitMode4>;
using PF32Tt8Bit = PF32Tt<pf32_ll::BitMode8>;
using PF32Tt16Bit = PF32Tt<pf32_ll::BitMode16>;

} // namespace pando
} // namespace pnd
