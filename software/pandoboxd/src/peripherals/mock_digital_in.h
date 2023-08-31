#pragma once

#include "peripheral_base.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

/** Peripheral that generates mock DigitalIn messages. */
class MockDigitalIn : public PeripheralBase {
  using PeripheralBase::PeripheralBase;
  void Configure(const PandoboxdInterface::Config& config) final;

  /** Produce a fake PPS signal that has T_high=10ms. */
  void Run() final;
};

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
