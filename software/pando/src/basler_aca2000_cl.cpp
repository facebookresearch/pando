#include "basler_aca2000_cl.h"

#include "me5_framegrabber.h"
#include "pando.h"
#include "reporter.h"

#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/PylonIncludes.h>

#include <memory>
#include <regex>
#include <utility>

namespace pnd {
namespace pando {

namespace {

class BaslerAca2000CLImpl : public BaslerAca2000CL {
 public:
  BaslerAca2000CLImpl() try {
    // Open the first available camera with appropriate model name
    try {
      Pylon::CDeviceInfo di;
      di.SetModelName("acA2000-340kmNIR");
      camera_.Attach(Pylon::CTlFactory::GetInstance().CreateFirstDevice(di));
      camera_.Open();
    } catch (const GenICam_3_1_Basler_pylon::GenericException& e) {
      g_reporter->error(e.what());
      throw std::runtime_error("BaslerAca2000CL: Couldn't open camera.");
    }

    g_reporter->info(
        "BaslerAca2000CL: Opened camera with SN {}", camera_.GetDeviceInfo().GetSerialNumber());

    // Open the framegrabber that this camera is connected to
    grabber_ = Me5FrameGrabber::Create(GetBoardId());

    camera_.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_Mono8);

    // Set up CC1 as an input
    camera_.LineSelector.SetValue(Basler_UniversalCameraParams::LineSelector_CC1);
    camera_.LineMode.SetValue(Basler_UniversalCameraParams::LineMode_Input);
    camera_.LineDebouncerTimeAbs.SetValue(5);

    // Use CC1 as hardware frame trigger
    camera_.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
    camera_.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_CC1);
    camera_.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);

    // Configure exposure mode (exposure time is set in StartImpl)
    camera_.ExposureMode.SetValue(Basler_UniversalCameraParams::ExposureMode_Timed);
    camera_.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Off);

    // Configure tap geometry
    camera_.ClTapGeometry.SetValue(Basler_UniversalCameraParams::ClTapGeometry_Geometry1X10_1Y);

  } catch (GenICam_3_1_Basler_pylon::GenericException& e) {
    g_reporter->critical(e.what());
    throw;
  }

 private:
  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final try {
    if (config.camera_enable_roi) {
      camera_.OffsetX.SetValue(config.camera_roi_x_offset);
      camera_.OffsetY.SetValue(config.camera_roi_y_offset);
      camera_.Width.SetValue(config.camera_roi_width);
      camera_.Height.SetValue(config.camera_roi_height);
    }

    camera_.ExposureTimeAbs.SetValue(config.camera_exposure_time_us);

    // Report black level
    g_reporter->info(
        "BaslerAca2000CL: Currently configured black level (Raw) is {}",
        camera_.BlackLevelRaw.GetValue());

    if (config.camera_enable_test_pattern)
      camera_.TestImageSelector.SetValue(
          Basler_UniversalCameraParams::TestImageSelector_Testimage2);
    else
      camera_.TestImageSelector.SetValue(Basler_UniversalCameraParams::TestImageSelector_Off);

    auto width = static_cast<uint32_t>(camera_.Width.GetValue());
    auto height = static_cast<uint32_t>(camera_.Height.GetValue());

    g_reporter->debug("BaslerAca2000CL: resolution is {}x{}", width, height);

    grabber_->Start(
        experiment_id,
        config.log_raw_data,
        config.publish_raw_data,
        raw_file_name,
        width,
        height,
        std::chrono::nanoseconds{10 * config.camera_frame_trigger_period_10ns},
        std::chrono::duration<double, std::micro>{config.camera_exposure_time_us});
  } catch (GenICam_3_1_Basler_pylon::GenericException& e) {
    g_reporter->critical(e.what());
    throw;
  }

  void StopImpl() final {
    grabber_->Stop();
  }

  uint32_t GetBoardId() {
    std::string port_id(camera_.GetDeviceInfo().GetPortID().c_str());

    g_reporter->debug("BaslerAca2000CL: PortId is {}", port_id);

    // Port ID string looks like "Silicon Software GmbH#board0_port0"
    // We want to extract the "0" from "board0"
    std::regex re("board([0-9]+)_port([0-9]+)");
    std::smatch m;
    if (!std::regex_search(port_id, m, re))
      throw std::runtime_error("BaslerAca2000CL: Failed to parse PortId");

    return std::stoi(m[1]);
  }

  Pylon::PylonAutoInitTerm auto_init_term_; // library handle

  Pylon::CBaslerUniversalInstantCamera camera_;

  std::unique_ptr<Me5FrameGrabber> grabber_;
};

} // namespace

std::unique_ptr<BaslerAca2000CL> BaslerAca2000CL::Create() {
  return std::make_unique<BaslerAca2000CLImpl>();
}

} // namespace pando
} // namespace pnd
