#include "camera_frame_trigger.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void CameraFrameTrigger::Configure(const PandoboxdInterface::Config& config) {
  pando_box_->SetTrigGen0Period(config.camera_frame_trigger_0_period_10ns);
  pando_box_->SetTrigGen0Width(config.camera_frame_trigger_0_period_10ns / 2);

  pando_box_->SetTrigGen1Period(config.camera_frame_trigger_1_period_10ns);
  pando_box_->SetTrigGen1Width(config.camera_frame_trigger_1_period_10ns / 2);
}

void CameraFrameTrigger::Run() {
  // return immediately
}

void CameraFrameTrigger::Enable() {
  pando_box_->SetTrigGen0Enabled(true);
  pando_box_->SetTrigGen1Enabled(true);
}

void CameraFrameTrigger::Disable() {
  pando_box_->SetTrigGen0Enabled(false);
  pando_box_->SetTrigGen1Enabled(false);
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
