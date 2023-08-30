#pragma once

#include <fmt/format.h>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include "histogram.h"
#include "types.h"

namespace pnd {
namespace pando {

class Counter {
 public:
  Counter(MacroTime first_integ_period_start_time = {})
      : current_integ_period_start_time_(first_integ_period_start_time), current_integ_period_(0){};

  bool IntegrationPeriodComplete() const {
    return current_integ_period_complete_;
  };

  uint64_t GetCount() const {
    return count_;
  };

  MacroTime GetIntegrationPeriod() const {
    return current_integ_period_;
  };

  MacroTime GetIntegrationPeriodStartTime() const {
    return current_integ_period_start_time_;
  };

 protected:
  uint64_t count_ = 0;
  bool current_integ_period_complete_ = true;
  MacroTime current_integ_period_start_time_;
  MacroTime current_integ_period_;
  uint64_t leftover_count_ = 0;
};

class TimestampCounter : public Counter {
 public:
  using Counter::Counter;
  void CountTimestamps(const ChannelTimestamps& timestamps, MacroTime target_integ_period);
};

class HistogramCounter : public Counter {
 public:
  using Counter::Counter;

  template <class BinDuration>
  void CountHistogram(const Histogram<BinDuration>& histogram, MacroTime target_integ_period) {
    auto bin_size_ps = std::chrono::duration_cast<MacroTime>(histogram.GetBinSize());

    if (current_integ_period_complete_) {
      current_integ_period_complete_ = false;
      current_integ_period_start_time_ += current_integ_period_;
      if ((target_integ_period % bin_size_ps).count() != 0) {
        throw std::invalid_argument(fmt::format(
            "Requested count rate integration period is not "
            "evenly divisble into histogram bin size! A count rate integration"
            " period of {}ps was requested for a bin size of {}ps. "
            "the closest valid count rate integration periods are {}ps and {}ps. "
            "Please change Count Rate Integration Period and try again.",
            target_integ_period.count(),
            bin_size_ps.count(),
            (target_integ_period / bin_size_ps * bin_size_ps).count(),
            (target_integ_period / bin_size_ps * bin_size_ps + bin_size_ps).count()));
      }
      current_integ_period_ = target_integ_period;

      // check if integration period is legal
      if (current_integ_period_start_time_ + 2 * current_integ_period_ <=
          histogram.GetEndBinIdx() * bin_size_ps) {
        throw std::invalid_argument(
            "HistogramCounter: Target integration period is less than the histogram period.");
      }

      count_ = leftover_count_;
      leftover_count_ = 0;
    }

    auto histogram_end_time = histogram.GetEndBinIdx() * bin_size_ps;
    if (histogram_end_time <= current_integ_period_start_time_ + current_integ_period_) {
      // this entire histogram is within the current integration period
      count_ += std::accumulate(histogram.cbegin(), histogram.cend(), (uint64_t)0);
      if (histogram_end_time == current_integ_period_start_time_ + current_integ_period_) {
        // end of this histogram is at the end of the current integration period
        current_integ_period_complete_ = true;
      }
    } else { // part of this histogram is within the current integration period
      auto integ_period_end_bin =
          (current_integ_period_start_time_ + current_integ_period_) / bin_size_ps;
      auto integ_period_end_it =
          histogram.cbegin() + (integ_period_end_bin - histogram.GetBeginBinIdx());
      count_ += std::accumulate(histogram.cbegin(), integ_period_end_it, (uint64_t)0);
      leftover_count_ += std::accumulate(integ_period_end_it, histogram.cend(), (uint64_t)0);
      current_integ_period_complete_ = true;
    }
  }
};

class ROICounter : public Counter {
 public:
  using Counter::Counter;

  MicroTime GetROIMin() const {
    return roi_min_;
  };
  MicroTime GetROIMax() const {
    return roi_max_;
  };

  void CountROI(
      const ChannelTimestamps& timestamps,
      MacroTime target_integ_period,
      MicroTime roi_min,
      MicroTime roi_max);

 private:
  MicroTime roi_min_, new_roi_min_;
  MicroTime roi_max_, new_roi_max_;
};

} // namespace pando
} // namespace pnd
