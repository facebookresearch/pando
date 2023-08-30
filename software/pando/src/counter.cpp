#include "counter.h"

namespace pnd {
namespace pando {

void TimestampCounter::CountTimestamps(
    const ChannelTimestamps& timestamps,
    MacroTime target_integ_period) {
  // set up the next integration period
  if (current_integ_period_complete_) {
    current_integ_period_complete_ = false;

    // update the integration period start
    current_integ_period_start_time_ += current_integ_period_;

    // update desired integration period
    current_integ_period_ = target_integ_period;

    // check if integration period is legal
    if (current_integ_period_start_time_ + 2 * current_integ_period_ <= timestamps.GetSpanUntil()) {
      throw std::invalid_argument(
          "TimestampCounter: Target integration period is less than the timestamp period.");
    }

    // deal with leftovers from the last batch of timestamps
    count_ = leftover_count_;
    leftover_count_ = 0;
  }

  // get the count
  if (timestamps.GetSpanUntil() <= current_integ_period_start_time_ + current_integ_period_) {
    // all of these timestamps are part of current integration period
    count_ += timestamps.macro_times().size();

    if (timestamps.GetSpanUntil() == current_integ_period_start_time_ + current_integ_period_) {
      // integration period ends at the end of these timestamps
      current_integ_period_complete_ = true;
    }
  } else { // integration period ends somewhere within these timestamps
    auto this_integ_period_end = std::lower_bound(
        timestamps.macro_times().begin(),
        timestamps.macro_times().end(),
        current_integ_period_start_time_ + current_integ_period_,
        std::less_equal<MacroTime>{});
    count_ += std::distance(timestamps.macro_times().begin(), this_integ_period_end);
    leftover_count_ = std::distance(this_integ_period_end, timestamps.macro_times().end());

    current_integ_period_complete_ = true;
  }
}

void ROICounter::CountROI(
    const ChannelTimestamps& timestamps,
    MacroTime target_integ_period,
    MicroTime roi_min,
    MicroTime roi_max) {
  // set up the next integration period
  if (current_integ_period_complete_) {
    current_integ_period_complete_ = false;

    // update the integration period start
    current_integ_period_start_time_ += current_integ_period_;

    // update desired integration period and ROI bounds
    current_integ_period_ = target_integ_period;
    // if there was leftover timestamps from the last batch, then the ROI has already been set for
    // this integration period and it shouldn't be changed even if the user has supplied a new
    // value. If there weren't any leftovers, than it can be set to the user-supplied value.

    // check if integration period is legal
    if (current_integ_period_start_time_ + 2 * current_integ_period_ <= timestamps.GetSpanUntil()) {
      throw std::invalid_argument(
          "ROICounter: Target integration period is less than the timestamp period.");
    }

    if (leftover_count_ == 0) {
      roi_min_ = roi_min;
      roi_max_ = roi_max;
    } else {
      roi_min_ = new_roi_min_;
      roi_max_ = new_roi_max_;
    }

    // deal with leftovers from the last batch of timestamps
    count_ = leftover_count_;
    leftover_count_ = 0;
  }

  // do the count
  if (timestamps.GetSpanUntil() <= current_integ_period_start_time_ + current_integ_period_) {
    // all of these timestamps are within the current integration period
    count_ += std::count_if(
        timestamps.micro_times().begin(), timestamps.micro_times().end(), [&](MicroTime t) {
          return (t >= roi_min_ && t <= roi_max_);
        });
    if (timestamps.GetSpanUntil() == current_integ_period_start_time_ + current_integ_period_) {
      // integration period ends at the end of these timestamps
      current_integ_period_complete_ = true;
    }
  } else {
    // if the integration period is ending in this batch of timestamps and the ROI has changed,
    // then it needs to be considered here so that leftover timestamps correspond to the new ROI.
    new_roi_min_ = roi_min;
    new_roi_max_ = roi_max;

    auto this_integ_period_end_macrotime = std::lower_bound(
        timestamps.macro_times().begin(),
        timestamps.macro_times().end(),
        current_integ_period_start_time_ + current_integ_period_,
        std::less_equal<MacroTime>{});
    auto this_integ_period_end_microtime = timestamps.micro_times().begin() +
        std::distance(timestamps.macro_times().begin(), this_integ_period_end_macrotime);
    count_ += std::count_if(
        timestamps.micro_times().begin(), this_integ_period_end_microtime, [&](MicroTime t) {
          return (t >= roi_min_ && t <= roi_max_);
        });
    leftover_count_ += std::count_if(
        this_integ_period_end_microtime, timestamps.micro_times().end(), [&](MicroTime t) {
          return (t >= new_roi_min_ && t <= new_roi_max_);
        });

    current_integ_period_complete_ = true;
  }
}
} // namespace pando
} // namespace pnd
