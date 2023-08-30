#include "basler_aca2000_cl.h"

#include "gtest/gtest.h"

#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/PylonIncludes.h>
#include <iostream>

namespace pnd {
namespace pando {

class BaslerAca2000CLTest : public ::testing::Test {
 protected:
  BaslerAca2000CLTest() {
    bas_ = BaslerAca2000CL::Create();
  }

  std::unique_ptr<BaslerAca2000CL> bas_;
};

/** Construct a BaslerAca2000CL instance. */
TEST_F(BaslerAca2000CLTest, DISABLED_Initialization) {}

/** Start & Stop an acqisition */
TEST_F(BaslerAca2000CLTest, DISABLED_StartStop) {
  PandoInterface::Config config;

  bas_->Start(0, config, nullptr);
}

} // namespace pando
} // namespace pnd
