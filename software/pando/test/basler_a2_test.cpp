#include "basler_a2.h"
#include "pandoboxd_client.h"

#include "gtest/gtest.h"

#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

namespace pnd {
namespace pando {

class BaslerA2Test : public ::testing::Test {
 protected:
  BaslerA2Test() {
    g_reporter = common::Reporter::Create("BaslerA2_test");

    camera_ = std::make_unique<BaslerA2>();
  }

  std::unique_ptr<DeviceInterface> camera_;
};

/** Construct a BaslerA2 instance. */
TEST_F(BaslerA2Test, DISABLED_Initialization) {}

/** Acquire data for 30 seconds. */
TEST_F(BaslerA2Test, DISABLED_Run) {
  static constexpr char raw_fname[] = "basler_a2_test_raw.h5";
  std::remove(raw_fname);

  PandoInterface::Config config;
  config.log_raw_data = true;
  config.publish_raw_data = true;
  config.use_pandoboxd = true;
  config.camera_exposure_time_us = 6000;
  config.camera_frame_trigger_period_10ns = 641025; // 356 fps
  // Need Pandoboxd to generate frame trigger
  PandoboxdClient clem(config.pandoboxd_ip_addr);

  common::SessionHandle cam_hnd = camera_->Start(0, config, raw_fname);
  common::SessionHandle clem_hnd = clem.Start(0, config);

  std::this_thread::sleep_for(std::chrono::seconds(5));
}

} // namespace pando
} // namespace pnd
