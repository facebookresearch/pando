#include "counter.h"
#include "histogram.h"
#include "types.h"

#include <vector>
#include "gtest/gtest.h"

namespace pnd {
namespace pando {

class CounterTest : public ::testing::Test {
 public:
  CounterTest() : hist1_(MacroTime{10}), hist2_(MacroTime{10}), hist3_(MacroTime{10}){};

 protected:
  MacroTimes makeMTimes(std::vector<int> values) {
    MacroTimes mtimes;
    for (auto it = values.begin(); it != values.end(); it++) {
      mtimes.push_back(MacroTime{*it});
    }
    return mtimes;
  }

  MicroTimes makeUTimes(std::vector<int> values) {
    MicroTimes utimes;
    for (auto it = values.begin(); it != values.end(); it++) {
      utimes.push_back(MicroTime{*it});
    }
    return utimes;
  }

  ChannelTimestamps makeChannelTimestamps(
      std::vector<int> utime_values,
      std::vector<int> mtime_values,
      MacroTime span_from,
      MacroTime span_until) {
    ChannelTimestamps timestamps;
    timestamps.micro_times() = makeUTimes(utime_values);
    timestamps.macro_times() = makeMTimes(mtime_values);
    timestamps.SetSpan(span_from, span_until);
    return timestamps;
  }

  void SetUp() override {
    auto utime1 = std::vector<int>{7, 8, 8, 3, 6, 4, 8, 1, 1, 6};
    auto mtime1 = std::vector<int>{6, 8, 18, 22, 23, 24, 34, 60, 68, 69};

    auto utime2 = std::vector<int>{10, 3, 9, 5, 8, 10, 10, 3, 3, 5, 3};
    auto mtime2 = std::vector<int>{71, 72, 73, 76, 78, 86, 99, 100, 102, 111, 135};

    auto utime3 = std::vector<int>{8, 3, 6, 2, 9, 3, 1, 2, 1};
    auto mtime3 = std::vector<int>{140, 157, 165, 180, 183, 185, 190, 200, 201};

    base_period_ = MacroTime{70};

    ts1_ = makeChannelTimestamps(utime1, mtime1, MacroTime{0}, 1 * base_period_);
    ts2_ = makeChannelTimestamps(utime2, mtime2, MacroTime{0}, 2 * base_period_);
    ts3_ = makeChannelTimestamps(utime3, mtime3, MacroTime{0}, 3 * base_period_);

    hist1_.BinMacroTimes(ts1_.macro_times(), 0, 7);
    //  hist1_ expected [2, 1, 3, 1, 0, 0, 3]
    hist2_.BinMacroTimes(ts2_.macro_times(), 7, 14);
    // hist2_ expected [5, 1, 1, 2, 1, 0, 1]
    hist3_.BinMacroTimes(ts3_.macro_times(), 14, 21);
    // hist3_ expected [1, 1, 1, 0, 3, 1, 1]
  }

  ChannelTimestamps ts1_, ts2_, ts3_;
  MacroTime base_period_;
  Histogram<MacroTime> hist1_, hist2_, hist3_;

  TimestampCounter timestamp_counter_;
  ROICounter roi_counter_;
  HistogramCounter hist_counter_;
};

// Tests initialization
TEST_F(CounterTest, DoNothing) {}

// Tests that taking count at base integration period yields expected results
TEST_F(CounterTest, CountTimestamps_IntegrationPeriod_Default) {
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  timestamp_counter_.CountTimestamps(ts1_, base_period_);
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriodStartTime().count(), 0);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), ts1_.macro_times().size());

  timestamp_counter_.CountTimestamps(ts2_, base_period_);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), ts2_.macro_times().size());

  timestamp_counter_.CountTimestamps(ts3_, base_period_);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), ts3_.macro_times().size());
}

// Should end up with 2 complete integration periods from 3 timestamp batches
TEST_F(CounterTest, CountTimestamps_IntegrationPeriod_DefaultAndAHalf) {
  auto period = MacroTime(static_cast<int>(base_period_.count() * 1.5));
  timestamp_counter_.CountTimestamps(ts1_, period);
  EXPECT_FALSE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), ts1_.macro_times().size());

  timestamp_counter_.CountTimestamps(ts2_, period);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(
      timestamp_counter_.GetCount(),
      ts1_.macro_times().size() + ts2_.macro_times().size() - 2); // hand-calculated

  timestamp_counter_.CountTimestamps(ts3_, period);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), ts3_.macro_times().size() + 2); // hand-calculated
}

// Changes the integration period from 1.5x default to default on the second call to
// CountTimestamps, should end up with 1 complete period at 1.5x and 1 complete period at 1x, with
// some leftovers.
TEST_F(CounterTest, CountTimestamps_IntegrationPeriod_DefaultAndAHalf_Changed) {
  auto period = MacroTime(static_cast<int>(base_period_.count() * 1.5));
  timestamp_counter_.CountTimestamps(ts1_, period);
  EXPECT_FALSE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), ts1_.macro_times().size());

  timestamp_counter_.CountTimestamps(ts2_, base_period_);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(
      timestamp_counter_.GetCount(),
      ts1_.macro_times().size() + ts2_.macro_times().size() - 2); // hand-calculated

  timestamp_counter_.CountTimestamps(ts3_, base_period_);
  EXPECT_TRUE(timestamp_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(timestamp_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(timestamp_counter_.GetCount(), 5); // hand-calculated
}

// Expect 3 full integration periods with the same ROI for each.
TEST_F(CounterTest, CountROI_IntegrationPeriod_Default) {
  auto roi_min = MicroTime{3};
  auto roi_max = MicroTime{8};

  roi_counter_.CountROI(ts1_, base_period_, roi_min, roi_max);
  EXPECT_EQ(roi_counter_.GetIntegrationPeriodStartTime().count(), 0);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(roi_counter_.GetCount(), 8); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_counter_.CountROI(ts2_, base_period_, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(roi_counter_.GetCount(), 7); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_counter_.CountROI(ts3_, base_period_, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(roi_counter_.GetCount(), 4); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);
}

// Expect 3 full integration periods with a different ROI for each
TEST_F(CounterTest, CountROI_IntegrationPeriod_Default_ChangeROI) {
  auto roi_min = MicroTime{3};
  auto roi_max = MicroTime{8};

  roi_counter_.CountROI(ts1_, base_period_, roi_min, roi_max);
  EXPECT_EQ(roi_counter_.GetIntegrationPeriodStartTime().count(), 0);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(roi_counter_.GetCount(), 8); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_min = MicroTime{3};
  roi_max = MicroTime{3};

  roi_counter_.CountROI(ts2_, base_period_, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(roi_counter_.GetCount(), 4); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_min = MicroTime{1};
  roi_max = MicroTime{10};

  roi_counter_.CountROI(ts3_, base_period_, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(roi_counter_.GetCount(), ts3_.macro_times().size()); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);
}

// Expect 2 full integration periods with the same ROI for each
TEST_F(CounterTest, CountROI_IntegrationPeriod_DefaultAndAHalf) {
  auto roi_min = MicroTime{3};
  auto roi_max = MicroTime{8};

  auto period = MacroTime(static_cast<int>(base_period_.count() * 1.5));
  roi_counter_.CountROI(ts1_, period, roi_min, roi_max);
  EXPECT_FALSE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(), 8);
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_counter_.CountROI(ts2_, period, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(),
            13); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_counter_.CountROI(ts3_, period, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(), 6); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);
}

// Expect that changing the ROI in the last call to Count does nothing (since the 3rd integration
// period was already started at the end of the 2nd call)
TEST_F(CounterTest, CountROI_IntegrationPeriod_DefaultAndAHalf_ChangeROILast) {
  auto roi_min = MicroTime{3};
  auto roi_max = MicroTime{8};

  auto period = MacroTime(static_cast<int>(base_period_.count() * 1.5));
  roi_counter_.CountROI(ts1_, period, roi_min, roi_max);
  EXPECT_FALSE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(), 8);
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_counter_.CountROI(ts2_, period, roi_min, roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(),
            13); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  auto new_roi_min = MicroTime{1};
  auto new_roi_max = MicroTime{10};

  roi_counter_.CountROI(ts3_, period, new_roi_min, new_roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(), 6); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);
}

// Expect that changing the ROI in the second call to Count does not change the first int periods
// ROI bounds, but does for the second
TEST_F(CounterTest, CountROI_IntegrationPeriod_DefaultAndAHalf_ChangeROISecond) {
  auto roi_min = MicroTime{3};
  auto roi_max = MicroTime{8};

  auto period = MacroTime(static_cast<int>(base_period_.count() * 1.5));
  roi_counter_.CountROI(ts1_, period, roi_min, roi_max);
  EXPECT_FALSE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(), 8);
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  auto new_roi_min = MicroTime{1};
  auto new_roi_max = MicroTime{10};

  roi_counter_.CountROI(ts2_, period, new_roi_min, new_roi_max);
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(),
            13); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), roi_max);

  roi_counter_.CountROI(
      ts3_,
      period,
      roi_min,
      roi_max); // changing them again here shouldn't make a difference
  EXPECT_TRUE(roi_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(roi_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(roi_counter_.GetCount(), ts3_.macro_times().size() + 2); // hand-calculated
  EXPECT_EQ(roi_counter_.GetROIMin(), new_roi_min);
  EXPECT_EQ(roi_counter_.GetROIMax(), new_roi_max);
}

TEST_F(CounterTest, CountHistogram_IntegrationPeriod_Default) {
  hist_counter_.CountHistogram(hist1_, base_period_);
  EXPECT_EQ(hist_counter_.GetIntegrationPeriodStartTime().count(), 0);
  EXPECT_TRUE(hist_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(hist_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(hist_counter_.GetCount(), ts1_.macro_times().size());

  hist_counter_.CountHistogram(hist2_, base_period_);
  EXPECT_EQ(hist_counter_.GetIntegrationPeriodStartTime().count(), 70);
  EXPECT_TRUE(hist_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(hist_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(hist_counter_.GetCount(), ts2_.macro_times().size());

  hist_counter_.CountHistogram(hist3_, base_period_);
  EXPECT_EQ(hist_counter_.GetIntegrationPeriodStartTime().count(), 140);
  EXPECT_TRUE(hist_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(hist_counter_.GetIntegrationPeriod().count(), base_period_.count());
  EXPECT_EQ(hist_counter_.GetCount(), ts3_.macro_times().size());
}

TEST_F(CounterTest, CountHistogram_IntegrationPeriod_Incorrect) {
  auto period = MacroTime(static_cast<int>(base_period_.count() * 1.5));
  EXPECT_THROW(hist_counter_.CountHistogram(hist1_, period), std::invalid_argument);
}

TEST_F(CounterTest, CountHistogram_IntegrationPeriod_80ps) {
  auto period = MacroTime{80};

  hist_counter_.CountHistogram(hist1_, period);
  EXPECT_EQ(hist_counter_.GetIntegrationPeriodStartTime().count(), 0);
  EXPECT_FALSE(hist_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(hist_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(hist_counter_.GetCount(), ts1_.macro_times().size());

  hist_counter_.CountHistogram(hist2_, period);
  EXPECT_EQ(hist_counter_.GetIntegrationPeriodStartTime().count(), 0);
  EXPECT_TRUE(hist_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(hist_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(hist_counter_.GetCount(), ts1_.macro_times().size() + 5); // hand-calculated

  hist_counter_.CountHistogram(hist3_, period);
  EXPECT_EQ(hist_counter_.GetIntegrationPeriodStartTime().count(), 80);
  EXPECT_TRUE(hist_counter_.IntegrationPeriodComplete());
  EXPECT_EQ(hist_counter_.GetIntegrationPeriod().count(), period.count());
  EXPECT_EQ(hist_counter_.GetCount(), ts2_.macro_times().size() - 5 + 2); // hand-calculated
}

} // namespace pando
} // namespace pnd
