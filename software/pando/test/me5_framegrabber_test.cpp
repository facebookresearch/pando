#include "me5_framegrabber.h"

#include "gtest/gtest.h"

namespace pnd {
namespace pando {

class Me5FrameGrabberTest : public ::testing::Test {
 protected:
  Me5FrameGrabberTest() {
    fg_ = Me5FrameGrabber::Create(1);
  }

  std::unique_ptr<Me5FrameGrabber> fg_;
};

/** Construct a Me5FrameGrabber instance. */
TEST_F(Me5FrameGrabberTest, DISABLED_Initialization) {}

/** Start & Stop an acqisition */
TEST_F(Me5FrameGrabberTest, DISABLED_StartStop) {
  fg_->Start(
      0,
      false,
      false,
      nullptr,
      2048,
      1088,
      std::chrono::nanoseconds{2941176},
      std::chrono::duration<double, std::micro>{2000});
  fg_->Stop();
}

} // namespace pando
} // namespace pnd
