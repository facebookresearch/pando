#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "peripheral_registers.h"
#include "uio_device.h"

namespace pnd {
namespace libpandobox {

/** Set the RUN flag in the GLOBAL_FLAGS_0 register to 1 for 10 seconds. */
TEST(RegisterTest, GlobalRunSetClear) {
  UioRegisters<reg::Global::Flags0> global_config_regs{reg::Global::kUioDevName};

  global_config_regs->WriteRun(true);
  EXPECT_EQ(global_config_regs->ReadRun(), true);

  std::this_thread::sleep_for(std::chrono::seconds{10});

  global_config_regs->WriteRun(false);
  EXPECT_EQ(global_config_regs->ReadRun(), false);
}

} // namespace libpandobox
} // namespace pnd
