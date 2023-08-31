#include "dcam_api.h"
#include "orca_fusion.h"
#include "pandoboxd_client.h"

#include "gtest/gtest.h"

#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

namespace pnd {
namespace pando {

TEST(DcamApiTest, DISABLED_Initialize) {
  auto api = dcam::DcamApi::Instance();
}

TEST(DcamApiTest, DISABLED_OpenCamera) {
  dcam::Cam c;
}

class OrcaFusionTest : public ::testing::Test {
 protected:
  OrcaFusionTest() {
    camera_ = std::make_unique<OrcaFusion>();
  }

  std::unique_ptr<DeviceInterface> camera_;
};

/** Construct a OrcaFusion instance. */
TEST_F(OrcaFusionTest, DISABLED_Initialization) {}

/** Acquire data for 30 seconds. */
TEST_F(OrcaFusionTest, DISABLED_Run) {
  static constexpr char raw_fname[] = "orca_fusion_test_raw.h5";
  std::remove(raw_fname);

  PandoInterface::Config config;
  config.log_raw_data = true;
  config.publish_raw_data = true;
  config.use_pandoboxd = true;
  config.camera_exposure_time_us = 1260; // Right on the edge of achievable @80fps
  config.camera_frame_trigger_period_10ns = 1250000; // 80 fps

  // Need Pandoboxd to generate frame trigger
  PandoboxdClient clem(config.pandoboxd_ip_addr);

  common::SessionHandle cam_hnd = camera_->Start(0, config, raw_fname);
  common::SessionHandle clem_hnd = clem.Start(0, config);

  std::this_thread::sleep_for(std::chrono::seconds(5));
}

} // namespace pando
} // namespace pnd
