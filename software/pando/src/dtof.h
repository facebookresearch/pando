#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include "types.h"

namespace pnd {
namespace pando {

/**
 * Class representing a DTOF of MicroTimes
 */
class Dtof : private std::vector<uint32_t> {
 public:
  using std::vector<uint32_t>::operator[];
  using std::vector<uint32_t>::cbegin;
  using std::vector<uint32_t>::begin;
  using std::vector<uint32_t>::end;
  using std::vector<uint32_t>::cend;
  using std::vector<uint32_t>::data;
  using std::vector<uint32_t>::size;

  Dtof(
      MicroTime bin_size,
      MicroTime data_resolution,
      MicroTime dtof_range_min,
      MicroTime dtof_range_max,
      MacroTime first_integ_period_start_time = {})
      : bin_size_{bin_size},
        data_resolution_{data_resolution},
        current_integ_period_(0),
        current_integ_period_start_time_(first_integ_period_start_time) {
    // check if bin_size and data_resolution are compatible
    // data resolution must be finer or equal to the desired bin size
    // additionally, data_resolution must evenly divide into bin size
    if (bin_size < data_resolution) {
      throw std::invalid_argument("Dtof: bin_size is smaller than data resolution.");
    }
    if ((bin_size % data_resolution).count() != 0) {
      throw std::invalid_argument("Dtof: bin_size is not divisible into the data resolution.");
    }

    // check if dtof range is sane
    if (dtof_range_max < dtof_range_min) {
      throw std::invalid_argument("dtof_range_max is less than dtof_range_min");
    }
    // sets dtof_range_min_ to the closest bin less than or equal to given value
    dtof_range_min_ = (dtof_range_min / bin_size_) * bin_size_;

    // sets dtof_range_max_ to the closest bin greater than or equal to given value
    if ((dtof_range_max % bin_size_).count() == 0) {
      dtof_range_max_ = dtof_range_max;
    } else {
      dtof_range_max_ = ((dtof_range_max / bin_size_) + 1) * bin_size_;
    }

    // number of bins in the dtof
    dtof_size_ = (dtof_range_max_ - dtof_range_min_) / bin_size_ + 1;
  };

  /**
   * Generates the DTOF with given microtimes
   */
  void Generate(const ChannelTimestamps& timestamps, MacroTime target_integ_period) {
    // setup next integation period
    if (current_integ_period_complete_) {
      current_integ_period_complete_ = false;

      // update start of integration period
      current_integ_period_start_time_ += current_integ_period_;
      // update desired integration period
      current_integ_period_ = target_integ_period;

      // check if integration period is legal (greater than or equal to the timestamp period)
      if (current_integ_period_start_time_ + 2 * current_integ_period_ <=
          timestamps.GetSpanUntil()) {
        throw std::invalid_argument(
            "DTOF: Target integration period is less than the timestamp period.");
      }

      // add leftover values if they exist
      if (leftover_dtof_values_.size()) {
        // put leftovers in the dtof
        this->swap(leftover_dtof_values_);
        // erase the leftovers
        leftover_dtof_values_.clear();
      } else {
        // clear the dtof
        this->clear();
        // set the size
        this->resize(dtof_size_, 0);
      }
    }

    if (timestamps.GetSpanUntil() <= current_integ_period_start_time_ + current_integ_period_) {
      // this entire set of timestamps is contained within the current integration period
      // add micro_times to DTOF
      std::for_each(
          timestamps.micro_times().begin(), timestamps.micro_times().end(), [&](MicroTime utime) {
            BinMicroTime(utime);
          });
      // check if integration period completes at the end of these timestamps
      if (timestamps.GetSpanUntil() == current_integ_period_start_time_ + current_integ_period_) {
        current_integ_period_complete_ = true;
      }

    } else {
      // part of these timestamps are contained within the current integration period
      auto this_integ_period_end_mtime = std::lower_bound(
          timestamps.macro_times().begin(),
          timestamps.macro_times().end(),
          current_integ_period_start_time_ + current_integ_period_,
          std::less_equal<MacroTime>{});
      auto this_integ_period_end_utime = timestamps.micro_times().begin() +
          std::distance(timestamps.macro_times().begin(), this_integ_period_end_mtime);

      std::for_each(
          timestamps.micro_times().begin(), this_integ_period_end_utime, [&](MicroTime utime) {
            BinMicroTime(utime);
          });

      // initialize leftover_dtof_values_
      leftover_dtof_values_.resize(dtof_size_, 0);
      std::for_each(
          this_integ_period_end_utime, timestamps.micro_times().end(), [&](MicroTime utime) {
            BinLeftoverMicroTime(utime);
          });

      current_integ_period_complete_ = true;
    }
  }

  MicroTime GetBinSize() const {
    return bin_size_;
  }

  MicroTime GetDataResolution() const {
    return bin_size_;
  }

  MicroTime GetRangeMin() const {
    return dtof_range_min_;
  }

  MicroTime GetRangeMax() const {
    return dtof_range_max_;
  }

  int GetDtofSize() const {
    return dtof_size_;
  }

  bool IntegrationPeriodComplete() const {
    return current_integ_period_complete_;
  }

  MacroTime GetIntegrationPeriodStartTime() const {
    return current_integ_period_start_time_;
  }

  MacroTime GetIntegrationPeriod() const {
    return current_integ_period_;
  }

 private:
  void BinMicroTime(MicroTime utime) {
    if (utime < dtof_range_max_ + bin_size_ && utime >= dtof_range_min_) {
      auto bin = (utime - dtof_range_min_) / bin_size_;
      this->operator[](bin) += 1;
    }
  }

  void BinLeftoverMicroTime(MicroTime utime) {
    // dtof_range_max_ is the maximum utime value that the user would like to appear in the dtoff
    // so it is the start of the last dtof bin. If the bin size is larger than the data reslution,
    // this results in the last bin containing values in range [dtof_range_max_, dtof_range_max_ +
    // bin_size_)
    if (utime < dtof_range_max_ + bin_size_ && utime >= dtof_range_min_) {
      auto bin = (utime - dtof_range_min_) / bin_size_;
      leftover_dtof_values_[bin] += 1;
    }
  }

  MicroTime bin_size_;
  MicroTime data_resolution_;
  MicroTime dtof_range_min_;
  MicroTime dtof_range_max_;
  int dtof_size_;

  std::vector<uint32_t> leftover_dtof_values_ = {};
  MacroTime current_integ_period_start_time_;
  MacroTime current_integ_period_;
  bool current_integ_period_complete_ = true;
};

} // namespace pando
} // namespace pnd
