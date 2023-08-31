#include "pf32_acquisition.h"
#include "pf32_ll.h"

#include <cstdint>

#include "gtest/gtest.h"

#include <chrono>
#include <list>
#include <memory>

namespace pnd {
namespace pando {
namespace pf32_ll {

/** Test which frame rates are achieveable without dropping frames.
 * If we don't drop any frames, the frame_count field in each succesive frame will always increment
 * by one.
 */
class PF32LLTest : public ::testing::Test {
 protected:
  using BitMode = BitMode8;

  PF32LLTest() {
    // Populate enabled_channels_ with 0-1023
    for (int32_t ch = 0; ch < 1023; ++ch) {
      enabled_channels_.push_back(ch);
    }
  }

  // Implementation of the test, to be run after pf32_ll_ is constructed
  void RunTest() {
    // Retrieve and discard kNReads blocks of Acquisition::kFramesPerRead frames.
    // Acquisition::GetFrames rethrows any exception thrown during asynchronous frame deinterleaving
    // (For example, if dropped frames were detected.)
    for (auto i = 0; i < kNReads; ++i) {
      auto frames = acquisition_->GetFrames();
    }
  }

  static constexpr size_t kNReads = 100;

  std::list<int32_t> enabled_channels_;

  // Assumes we're running from the root of ovrsource
  PF32LL pf32_ll_{"./third-party/pf32_api/pf32_api/bitfiles/PF32_USB3_[1864-8bit].bit"};
  std::unique_ptr<pf32_ll::Acquisition<BitMode>> acquisition_;
};

/** Construct a PF32LL instance. */
TEST_F(PF32LLTest, DISABLED_Initialization) {}

/** Test reading with 10us frame period */
TEST_F(PF32LLTest, DISABLED_Read10us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{10000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 9us frame period */
TEST_F(PF32LLTest, DISABLED_Read9us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{9000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 8us frame period */
TEST_F(PF32LLTest, DISABLED_Read8us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{8000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 7us frame period */
TEST_F(PF32LLTest, DISABLED_Read7us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{7000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 6us frame period */
TEST_F(PF32LLTest, DISABLED_Read6us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{6000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 5us frame period */
TEST_F(PF32LLTest, DISABLED_Read5us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{5000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 4us frame period */
TEST_F(PF32LLTest, DISABLED_Read4us) {
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{4000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 3us frame period */
// TEST_F(PF32LLTest, Read3us) {
//   acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
//       std::chrono::nanoseconds{3000}, Mode::photon_counting, enabled_channels_);
//   RunTest();
// }

/** Test reading with 2us frame period */
// TEST_F(PF32LLTest, Read2us) {
//   acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
//       std::chrono::nanoseconds{2000}, Mode::photon_counting, enabled_channels_);
//   RunTest();
// }

/** Test reading with 2us frame period with only channel 239 enabled.
 * This should result in row 16 being force enabled.
 */
TEST_F(PF32LLTest, DISABLED_ReadChan239) {
  enabled_channels_ = {239};
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{2000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

/** Test reading with 2us frame period with only channel 784 enabled.
 * This should result in row 0 being force enabled.
 */
TEST_F(PF32LLTest, DISABLED_ReadChan784) {
  enabled_channels_ = {784};
  acquisition_ = pf32_ll_.CreateAcquisition<BitMode>(
      std::chrono::nanoseconds{2000}, Mode::photon_counting, enabled_channels_);
  RunTest();
}

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
