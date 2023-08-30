#include "pando.h"
#include "pf32_dcs.h"

#include "pando.pb.h"
#include "reporter.h"

#include "gtest/gtest.h"

#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

namespace pnd {
namespace pando {
class PF32Test : public ::testing::Test {
 protected:
  PF32Test() {
    g_reporter = common::Reporter::Create("pf32_test");

    pando_ = SingletonPando::Get();

    PandoInterface::Config config;

    config.device = PandoInterface::DeviceType::PF32_DCS;
    config.pf32_bit_mode = PandoInterface::PF32BitMode::EIGHT;

    // Assumes we're running from the root of ovrsource
    config.pf32_firmware_path =
        "./third-party/pf32_api/pf32_api/bitfiles/PF32_USB3_[1864-8bit].bit";

    // Enable all 1024 channels
    config.enabled_channels.clear();
    for (int32_t ch = 0; ch < 1023; ++ch) {
      config.enabled_channels.push_back(ch);
    }
    pando_->Configure(config);
  }

  std::shared_ptr<PandoInterface> pando_;
};

/** Construct a PF32 instance. */
TEST_F(PF32Test, DISABLED_Initialization) {}

/** Acquire data for 30 seconds. */
TEST_F(PF32Test, DISABLED_Run) {
  auto config = pando_->GetConfig();

  // exponential binning
  // tau_k = 15us
  // tau_max = 0.945ms
  // t_int = 100.035ms
  config.bin_size_ns = 15000;
  config.points_per_level = 8;
  config.n_levels = 3;
  config.rebin_factor = 3;
  config.final_bin_count = 741;

  pando_->Configure(config);

  common::SessionHandle hnd = pando_->Start();
  std::this_thread::sleep_for(std::chrono::seconds(30));
  hnd.reset();
}

} // namespace pando
} // namespace pnd
