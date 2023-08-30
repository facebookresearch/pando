#pragma once

#include "reporter.h"
#include "x_harp_base.h"

namespace pnd {
namespace pando {

/** Templated base class for PicoQuant device classes implementing DeviceBase::UpdateRawData.
 * This is a CRTP style template. Impl must implement ConsumeRecord and inherit from
 * XHarpProcBase<Impl>.
 */
template <class Impl>
class XHarpProcBase : virtual public XHarpBase {
 protected:
  virtual int GetExcessDelayFactor() {
    return 1;
  }
  void UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) override {
    std::chrono::nanoseconds bin_size{config_.bin_size_ns};
    auto from = bin_size * begin_bin_idx;
    auto until = bin_size * end_bin_idx;

    // We store pointers to the mapped elements of dest.timestamps in timestamps_lut to provide
    // constant-time lookup by channel index in the loop below.
    std::array<ChannelTimestamps*, kMaxChannels> timestamps_lut{};
    for (auto& timestamps_kv : dest.timestamps) {
      // Clear the macro and micro time vectors for each channel without deallocating any memory.
      timestamps_kv.second.macro_times().clear();
      timestamps_kv.second.micro_times().clear();

      // Record the span of the timestamps we're about to store
      timestamps_kv.second.SetSpan(from, until);

      // Store a pointer to the timestamps for this enabled channel in our LUT
      timestamps_lut.at(timestamps_kv.first) = &timestamps_kv.second;
    }

    for (auto& timestamps_kv : dest.marker_timestamps) {
      // clear marker timestamps
      timestamps_kv.second.macro_times().clear();
      timestamps_kv.second.micro_times().clear();

      // Record the span of the timestamps we're about to store
      timestamps_kv.second.SetSpan(from, until);
    }

    // Pop tags until the macrotimes stored in dest.timestamps span a duration of [from, until)
    auto max_delay = (until - from) + kMaxExcessDelayBase * this->GetExcessDelayFactor();
    std::chrono::milliseconds total_delay(0);
    do {
      if (stashed_timestamp_) {
        stashed_timestamp_ = false;

        if (last_timestamp_macro_time_ < from) {
          throw XHarpException(
              "XHarpProcBase::UpdateRawData: first available timestamp falls before 'from' arg");
        }

        if (ChannelTimestamps* channel_timestamps = timestamps_lut.at(last_channel_)) {
          channel_timestamps->macro_times().push_back(last_timestamp_macro_time_);
          channel_timestamps->micro_times().push_back(last_micro_time_);
        } // else channel_timestamps was nullptr, discard stashed timestamps for non-enabled
          // channel.
      } else if (stashed_markers_) {
        stashed_markers_ = false;
        for (auto& ch_it : dest.marker_timestamps) {
          auto& ch_idx = ch_it.first;
          auto& ch_timestamps = ch_it.second;
          if (last_channel_ & (1 << ch_idx)) {
            ch_timestamps.macro_times().push_back(last_marker_macro_time_);
          }
        }
      }

      // Flip the buffer if we've consumed all available tags
      if (!proc_buffer_ || proc_buffer_idx_ == proc_buffer_->occupancy) {
        while (true) {
          proc_buffer_ = ring_buffer_.AdvanceRead();

          if (proc_buffer_ && proc_buffer_->occupancy > 0) {
            // We got some records.
            break;
          } else {
            // No records are available from the producer thread.
            if (total_delay > max_delay) {
              throw XHarpException(
                  "XHarpProcBase::UpdateRawData blocked for >500ms longer than requested window time. Are count rates zero?");
            }

            std::this_thread::sleep_for(kIncrementalDelay);
            total_delay += kIncrementalDelay;
          }
        }

        proc_buffer_idx_ = 0;
      }

      // ConsumeRecord stashes timestamps in last_timestamp_macro_time_
      static_cast<Impl*>(this)->ConsumeRecord(proc_buffer_->data[proc_buffer_idx_++]);

    } while (!stashed_timestamp_ || last_timestamp_macro_time_ < until);
  }

  bool stashed_timestamp_ = false;
  bool stashed_markers_ = false;
  uint8_t last_channel_;
  picoseconds last_micro_time_;
  picoseconds last_timestamp_macro_time_;
  picoseconds last_marker_macro_time_;
  uint64_t overflow_state_;

 private:
  static constexpr std::chrono::milliseconds kMaxExcessDelayBase{500};
  static constexpr std::chrono::milliseconds kIncrementalDelay{50};
  static constexpr size_t kMaxChannels{16};

  void ResetProc() override {
    proc_buffer_ = nullptr;
    proc_buffer_idx_ = 0;

    stashed_timestamp_ = false;
    stashed_markers_ = false;
    last_timestamp_macro_time_ = last_timestamp_macro_time_.zero();
    last_marker_macro_time_ = last_marker_macro_time_.zero();
    last_micro_time_ = last_micro_time_.zero();
    overflow_state_ = 0;
  }

  const RecordBuffer* proc_buffer_;
  size_t proc_buffer_idx_;
};

} // namespace pando
} // namespace pnd
