#include "time_shifter.h"

#include "gtest/gtest.h"

namespace pnd {
namespace pando {

/** Test evolution of jitter and offset through multiple calls to Adjust. */
TEST(TimeShifterTest, Adjust) {
  TimeShifter ts;

  // First PPS happens at exactly t=0 in the source time domain.
  // Jitter is 0ns, and new offset is 0ns.
  auto stats_0 = ts.Adjust(std::chrono::seconds(0));
  ASSERT_TRUE(stats_0.has_value());
  EXPECT_EQ(stats_0->offset.count(), 0);
  EXPECT_EQ(stats_0->jitter.count(), 0);

  // Spurious pulse with the same timestap as the first one is ignored due to holdoff
  auto stats_1 = ts.Adjust(std::chrono::seconds(0));
  ASSERT_FALSE(stats_1.has_value());

  // Second PPS happens one nanosecond late (from the perspective of the source time domain)
  // Jitter is (positive) 1ns, and new offset is 1ns
  auto stats_2 = ts.Adjust(std::chrono::seconds(1) + std::chrono::nanoseconds{1});
  ASSERT_TRUE(stats_2.has_value());
  EXPECT_EQ(stats_2->offset.count(), 1);
  EXPECT_EQ(stats_2->jitter.count(), 1);

  // Third PPS happens exactly one second after the second one (from the perspective of the source
  // time domain) Jitter is therefore 0ns, but offset remains 1ns.
  auto stats_3 = ts.Adjust(std::chrono::seconds(2) + std::chrono::nanoseconds{1});
  ASSERT_TRUE(stats_3.has_value());
  EXPECT_EQ(stats_3->offset.count(), 1);
  EXPECT_EQ(stats_3->jitter.count(), 0);

  // Fourth PPS happens exactly two seconds after the third one (from the perspective of the source
  // time domain). The missed edge is detected, Jitter is therefore 0ns, and offset remains 1ns.
  auto stats_4 = ts.Adjust(std::chrono::seconds(4) + std::chrono::nanoseconds{1});
  ASSERT_TRUE(stats_4.has_value());
  EXPECT_EQ(stats_4->offset.count(), 1);
  EXPECT_EQ(stats_4->jitter.count(), 0);

  // Fifth PPS happens exactly three seconds after the fourth one (from the perspective of the
  // source time domain). An exception is thrown because 2 PPS edges were missed.
  EXPECT_THROW(
      ts.Adjust(std::chrono::seconds(7) + std::chrono::nanoseconds{1}), std::runtime_error);
}

/** Test evolution of jitter and offset through multiple calls to Adjust. */
TEST(TimeShifterTest, Shift) {
  TimeShifter ts;

  static constexpr std::chrono::nanoseconds offset{1234};

  // First PPS happens at a not insignificant offset
  auto stats_0 = ts.Adjust(offset);
  ASSERT_TRUE(stats_0.has_value());
  EXPECT_EQ(stats_0->offset, offset);
  EXPECT_EQ(stats_0->jitter, offset);

  EXPECT_EQ(ts.Shift(offset), std::chrono::nanoseconds::zero());
}

} // namespace pando
} // namespace pnd
