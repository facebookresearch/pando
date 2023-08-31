#pragma once

#include "peripheral_base.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

/** Peripheral that controls camera frame trigger. */
class CameraFrameTrigger : public PeripheralBase {
  using PeripheralBase::PeripheralBase;
  void Configure(const PandoboxdInterface::Config& config) final;
  void Run() final;
  void Enable() final;
  void Disable() final;
};

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
