#pragma once

#include "reporter.h"

#include <cassert>
#include <chrono>
#include <optional>

namespace pnd {
namespace pando {

class TimeShifter {
 public:
  TimeShifter(std::chrono::milliseconds pps_holdoff = std::chrono::milliseconds(500))
      : pps_holdoff_{pps_holdoff} {}
  struct AdjustStats {
    std::chrono::nanoseconds offset;
    std::chrono::nanoseconds jitter;
  };

  template <class Rep, class Period>
  std::chrono::nanoseconds Shift(std::chrono::duration<Rep, Period> device_time) {
    return std::chrono::round<std::chrono::nanoseconds>(device_time) - offset_;
  }

  template <class Rep, class Period>
  std::optional<AdjustStats> Adjust(std::chrono::duration<Rep, Period> pps_time) {
    auto rounded_pps_time = std::chrono::round<std::chrono::nanoseconds>(pps_time);
    assert(rounded_pps_time >= last_pps_time_);

    if (primed_ && rounded_pps_time - last_pps_time_ < pps_holdoff_) {
      // Ignore pulses that occur during the holdoff period
      auto ms_since_last =
          std::chrono::duration_cast<std::chrono::milliseconds>(rounded_pps_time - last_pps_time_)
              .count();
      g_reporter->warn(
          "TimeShifter: Rejected PPS pulse inside holdoff interval. Pulse occurred {} ms after previous one",
          ms_since_last);
      return {};
    } else {
      auto expected_pps_time = last_pps_time_ + kPpsInterval;
      auto jitter = rounded_pps_time - expected_pps_time;

      auto n_missed = std::chrono::round<std::chrono::seconds>(jitter).count();
      if (primed_ && n_missed > 0) {
        g_reporter->warn("TimeShifter: Detected {} missing PPS edges", n_missed);

        if (n_missed == 1)
          // We allow recovery after a single missed PPS edge
          jitter -= kPpsInterval;
        if (n_missed > 1)
          throw std::runtime_error("TimeShifter: Multiple PPS edges missing");
      }

      offset_ += jitter;
      last_pps_time_ = rounded_pps_time;
      primed_ = true;

      return AdjustStats{offset_, jitter};
    }
  }

  bool IsPrimed() {
    return primed_;
  }

 private:
  static constexpr std::chrono::seconds kPpsInterval{1};
  std::chrono::milliseconds pps_holdoff_;
  std::chrono::nanoseconds last_pps_time_{-kPpsInterval};
  std::chrono::nanoseconds offset_{};
  bool primed_ = false;
};

} // namespace pando
} // namespace pnd
