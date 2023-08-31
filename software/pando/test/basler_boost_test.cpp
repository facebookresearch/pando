#include "basler_boost.h"

#include "pandoboxd_client.h"
#include "reporter.h"

#include "gtest/gtest.h"

#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

namespace pnd {
namespace pando {

class BaslerBoostTest : public ::testing::Test {
 protected:
  BaslerBoostTest() {
    g_reporter = common::Reporter::Create("BaslerBoost_test");

    camera_ = BaslerBoost::Create();
  }

  std::unique_ptr<DeviceInterface> camera_;
};

/** Construct a BaslerBoost instance. */
TEST_F(BaslerBoostTest, DISABLED_Initialization) {}

/** Acquire data for 30 seconds. */
TEST_F(BaslerBoostTest, DISABLED_Run) {
  static constexpr char raw_fname[] = "basler_boost_test.h5";
  std::remove(raw_fname);

  PandoInterface::Config config;
  config.log_raw_data = true;
  config.publish_raw_data = false;
  config.use_pandoboxd = true;
  config.camera_exposure_time_us = 2600;
  config.camera_frame_trigger_period_10ns = 270270; // 370 fps
  // Need Pandoboxd to generate frame trigger
  PandoboxdClient clem(config.pandoboxd_ip_addr);

  common::SessionHandle cam_hnd = camera_->Start(0, config, raw_fname);
  common::SessionHandle clem_hnd = clem.Start(0, config);

  std::this_thread::sleep_for(std::chrono::seconds(5));
}

} // namespace pando
} // namespace pnd
