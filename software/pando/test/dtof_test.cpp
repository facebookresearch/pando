#include "dtof.h"

#include <chrono>
#include <iostream>
#include "gtest/gtest.h"

namespace pnd {
namespace pando {

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

auto micro_times_1 = std::vector<int>{8, 4, 4, 16, 12, 20, 20, 20, 32, 8, 12, 20, 24, 24, 16, 24};
auto macro_times_1 = std::vector<int>{1, 4, 5, 6, 9, 11, 14, 15, 18, 21, 22, 25, 28, 33, 35, 38};
auto macro_times_1_end_time = MacroTime{40};

auto micro_times_2 = std::vector<int>{20, 24, 32, 32, 8, 20, 24, 24, 16, 16};
auto macro_times_2 = std::vector<int>{1, 3, 4, 5, 9, 10, 13, 19, 21, 23};
auto macro_times_2_end_time = MacroTime{24};

auto micro_times_3 = std::vector<int>{20, 24, 32, 32, 8, 20, 24, 24, 16, 36};
auto macro_times_3 = std::vector<int>{24, 25, 26, 29, 31, 34, 35, 37, 44, 47};
auto macro_times_3_end_time = MacroTime{48};

auto micro_times_4 = std::vector<int>{8, 24, 4, 32, 8, 20, 24, 24, 16, 4};
auto macro_times_4 = std::vector<int>{48, 49, 51, 52, 55, 58, 60, 64, 69, 70};
auto macro_times_4_end_time = MacroTime{72};

TEST(DtofTest, DoNothing) {
  // this test does nothing.
}

/**
 * Tests that Dtof initializes
 */
TEST(DtofTest, InitializationCorrect) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{0}, MicroTime{12});
  EXPECT_EQ(hist.GetRangeMin(), MicroTime{0});
  EXPECT_EQ(hist.GetRangeMax(), MicroTime{12});
  EXPECT_EQ(hist.GetDtofSize(), 4);
  EXPECT_EQ(hist.GetDataResolution(), MicroTime{4});
}

/**
 * Tests that Dtof initializes with bin_size-multiple range
 */
TEST(DtofTest, InitializationFixesRange) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{3}, MicroTime{11});
  EXPECT_EQ(hist.GetRangeMin(), MicroTime{0});
  EXPECT_EQ(hist.GetRangeMax(), MicroTime{12});
  EXPECT_EQ(hist.GetDtofSize(), 4);
  EXPECT_EQ(hist.GetDataResolution(), MicroTime{4});
}

/**
 * Tests that Dtof constructor throws an error if range max is less than min
 */
TEST(DtofTest, InitializationBackwardsRange) {
  ASSERT_THROW(Dtof(MicroTime{4}, MicroTime{4}, MicroTime{8}, MicroTime{4}), std::invalid_argument);
}

/**
 * Tests that Dtof constructor throws an error if the data resolution is larger than the bin size
 */
TEST(DtofTest, InitializationDataResolutionTooCoarse) {
  ASSERT_THROW(
      Dtof(MicroTime{2}, MicroTime{4}, MicroTime{0}, MicroTime{12}), std::invalid_argument);
}

/**
 * Tests that Dtof constructor throws an error if the bin size is incompatible with the data
 * resolution
 */
TEST(DtofTest, InitializationIncompatibleBinSize) {
  ASSERT_THROW(
      Dtof(MicroTime{7}, MicroTime{4}, MicroTime{0}, MicroTime{12}), std::invalid_argument);
}

/**
 * Tests that a Dtof with correctly-set bin size creates a correct Dtof. Single timestamp set
 * completes integration period
 */
TEST(DtofTest, GenerateWholeIntegrationPeriod) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{0}, MicroTime{32});
  auto timestamps =
      makeChannelTimestamps(micro_times_1, macro_times_1, MacroTime{0}, macro_times_1_end_time);
  auto expected = std::vector<uint32_t>{0, 2, 2, 2, 2, 4, 3, 0, 1};

  hist.Generate(timestamps, macro_times_1_end_time);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), macro_times_1_end_time);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  ASSERT_EQ(hist.size(), expected.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected[bin]);
  }
}

/**
 * Tests that a Dtof with a larger bin size generates the correct dtof.
 */
TEST(DtofTest, GenerateLargerBinSizeWholeIntegrationPeriod) {
  auto hist = Dtof(MicroTime{8}, MicroTime{4}, MicroTime{0}, MicroTime{32});

  auto timestamps =
      makeChannelTimestamps(micro_times_1, macro_times_1, MacroTime{0}, macro_times_1_end_time);
  auto expected = std::vector<uint32_t>{2, 4, 6, 3, 1};

  hist.Generate(timestamps, macro_times_1_end_time);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), macro_times_1_end_time);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  ASSERT_EQ(hist.size(), expected.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected[bin]);
  }
}

/**
 * Tests that trying to generate a dtof with an integration period less than the timestamp period
 * throws
 */
TEST(DtofTest, GenerateIntegrationPeriodTooSmall) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{0}, MicroTime{32});

  auto timestamps =
      makeChannelTimestamps(micro_times_1, macro_times_1, MacroTime{0}, macro_times_1_end_time);

  ASSERT_THROW(hist.Generate(timestamps, macro_times_1_end_time / 2), std::invalid_argument);
}

/**
 * Tests that a Dtof with the a smaller range than the range of the microtimes generates the
 * expected dtof
 */
TEST(DtofTest, GenerateRangeSubsetWholeIntegrationPeriod) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{12}, MicroTime{28});

  auto timestamps =
      makeChannelTimestamps(micro_times_1, macro_times_1, MacroTime{0}, macro_times_1_end_time);
  auto expected = std::vector<uint32_t>{2, 2, 4, 3, 0};

  hist.Generate(timestamps, macro_times_1_end_time);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), macro_times_1_end_time);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  ASSERT_EQ(hist.size(), expected.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected[bin]);
  }
}

/**
 * Tests that a Dtof with the a smaller range and larger bin size generates the expected dtof
 */
TEST(DtofTest, GenerateRangeSubsetLargeBinSizeWholeIntegrationPeriod) {
  auto hist = Dtof(MicroTime{8}, MicroTime{4}, MicroTime{8}, MicroTime{24});

  auto timestamps =
      makeChannelTimestamps(micro_times_1, macro_times_1, MacroTime{0}, macro_times_1_end_time);
  auto expected = std::vector<uint32_t>{4, 6, 3};

  hist.Generate(timestamps, macro_times_1_end_time);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), macro_times_1_end_time);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  ASSERT_EQ(hist.size(), expected.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected[bin]);
  }
}

/**
 * Tests that an integration period 1.5x times the size of the base period generates a full dtof
 * after two calls to generate.
 */
TEST(DtofTest, GenerateWholePlusHalfIntegrationPeriodsOnce) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{0}, MicroTime{36});

  auto timestamps1 =
      makeChannelTimestamps(micro_times_2, macro_times_2, MacroTime{0}, macro_times_2_end_time);
  auto timestamps2 =
      makeChannelTimestamps(micro_times_3, macro_times_3, MacroTime{0}, macro_times_3_end_time);
  auto period = MacroTime(static_cast<int>(macro_times_2_end_time.count() * 1.5));

  auto expected = std::vector<uint32_t>{0, 0, 2, 0, 2, 4, 5, 0, 4, 0};

  hist.Generate(timestamps1, period);

  ASSERT_FALSE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), period);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  hist.Generate(timestamps2, period);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), period);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  ASSERT_EQ(hist.size(), expected.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected[bin]);
  }
}

/**
 * Test that an integratio period 1.5x times the size of the base period generates
 * two full dtof after three calls to generate.
 */
TEST(DtofTest, GenerateWholePlusHalfIntegrationPeriodsTwice) {
  auto hist = Dtof(MicroTime{4}, MicroTime{4}, MicroTime{0}, MicroTime{36});

  auto timestamps1 =
      makeChannelTimestamps(micro_times_2, macro_times_2, MacroTime{0}, macro_times_2_end_time);
  auto timestamps2 =
      makeChannelTimestamps(micro_times_3, macro_times_3, MacroTime{0}, macro_times_3_end_time);
  auto timestamps3 =
      makeChannelTimestamps(micro_times_4, macro_times_4, MacroTime{0}, macro_times_4_end_time);
  auto period = MacroTime(static_cast<int>(macro_times_2_end_time.count() * 1.5));

  auto expected = std::vector<uint32_t>{0, 0, 2, 0, 2, 4, 5, 0, 4, 0};
  auto expected2 = std::vector<uint32_t>{0, 2, 2, 0, 2, 1, 4, 0, 1, 1};

  hist.Generate(timestamps1, period);

  ASSERT_FALSE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), period);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  hist.Generate(timestamps2, period);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), period);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), MacroTime{0});

  ASSERT_EQ(hist.size(), expected.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected[bin]);
  }

  hist.Generate(timestamps3, period);

  ASSERT_TRUE(hist.IntegrationPeriodComplete());
  ASSERT_EQ(hist.GetIntegrationPeriod(), period);
  ASSERT_EQ(hist.GetIntegrationPeriodStartTime(), period);

  ASSERT_EQ(hist.size(), expected2.size());
  for (uint32_t bin = 0; bin < hist.size(); bin++) {
    EXPECT_EQ(hist[bin], expected2[bin]);
  }
}

} // namespace pando
} // namespace pnd
