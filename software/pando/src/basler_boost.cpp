#include "basler_boost.h"

#include "camera_frame_handler.h"
#include "pando.h"
#include "reporter.h"
#include "thread_container.h"

#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/PylonIncludes.h>

#include <list>
#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

namespace {

class BaslerBoostImpl : public BaslerBoost {
 public:
  BaslerBoostImpl() try {
    try {
      // Open CXP12 framegrabber
      Pylon::CInterfaceInfo info;
      info.SetDeviceClass(Pylon::BaslerGenTlCxpDeviceClass);
      Pylon::CUniversalInstantInterface iface(info);
      iface.Open();

      // Enumerate cameras attached to the framegrabber
      Pylon::DeviceInfoList_t devices;
      const auto n_devices = iface.EnumerateDevices(devices);
      g_reporter->info("BaslerBoost: Enumerated {} cameras", n_devices);

      // Open each camera
      for (size_t i = 0; i < n_devices; ++i) {
        auto& camera = cameras_.emplace_back(iface.CreateDevice(devices[i]));
        camera.Open();
        auto dev_info = camera.GetDeviceInfo();
        g_reporter->info(
            "BaslerBoost: Opened {} with SN {}",
            dev_info.GetModelName(),
            dev_info.GetSerialNumber());
      }

    } catch (const GenICam_3_1_Basler_pylon::GenericException& e) {
      g_reporter->error(e.what());
      throw std::runtime_error("BaslerBoost: Couldn't open cameras.");
    }

    // TODO support nondefault bit depths
    pixel_type_ = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8;
    for (auto& camera : cameras_) {
      camera.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_Mono8);
      g_reporter->debug("BaslerBoost: Pixel format is {}", camera.PixelFormat.ToString());
    }

    // Configure frame grabber external trigger input from front GPI0 (on DSUB connector)
    for (auto& camera : cameras_) {
      Pylon::CBaslerUniversalInstantCamera::TlParams_t& tl_params = camera.GetTLParams();
      // Configure trigger input on GPI0
      tl_params.AreaTriggerMode.SetValue(Basler_UniversalTLParams::AreaTriggerMode_External);
      tl_params.TriggerInDebounce.SetValue(1.008);
      tl_params.TriggerInSource.SetValue(
          Basler_UniversalTLParams::TriggerInSource_TriggerInSourceFrontGPI0);
      tl_params.TriggerInStatisticsSource.SetValue(
          Basler_UniversalTLParams::TriggerInStatisticsSource_TriggerInSourceFrontGPI0);
      // Note: This just caps the input frame trigger frequency, so set it very high
      tl_params.TriggerOutputFrequency.SetValue(9990.0);
      // Note: trigger signal is inverted by our optoisolator board
      tl_params.TriggerInPolarity.SetValue(Basler_UniversalTLParams::TriggerInPolarity_LowActive);
      tl_params.TriggerInStatisticsPolarity.SetValue(
          Basler_UniversalTLParams::TriggerInStatisticsPolarity_LowActive);

      // Configure Pulse Form Generator 0
      tl_params.TriggerPulseFormGenerator3Downscale.SetValue(1);
      tl_params.TriggerPulseFormGenerator0DownscalePhase.SetValue(0);
      tl_params.TriggerPulseFormGenerator0Delay.SetValue(0.0);
      tl_params.TriggerPulseFormGenerator0Width.SetValue(100.0);
      tl_params.TriggerCameraOutSelect.SetValue(
          Basler_UniversalTLParams::TriggerCameraOutSelect_PulseGenerator0);
      tl_params.TriggerOutStatisticsSource.SetValue(
          Basler_UniversalTLParams::TriggerOutStatisticsSource_PulseGenerator0);
      tl_params.TriggerState.SetValue(Basler_UniversalTLParams::TriggerState_Active);

      tl_params.AutomaticFormatControl.SetValue(true);
      tl_params.AutomaticROIControl.SetValue(false); // racey, we'll do it ourselves

      // Use CXP1 as hardware frame trigger
      camera.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
      camera.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_CxpTrigger1);
      camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);

      // Configure Counter1 to count frame trigger pulses
      camera.CounterSelector.SetValue(Basler_UniversalCameraParams::CounterSelector_Counter1);
      camera.CounterEventSource.SetValue(
          Basler_UniversalCameraParams::CounterEventSource_CxpTrigger0);

      // Configure exposure mode (exposure time is set in StartImpl)
      camera.ExposureMode.SetValue(Basler_UniversalCameraParams::ExposureMode_Timed);
      camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Off);
    }

  } catch (GenICam_3_1_Basler_pylon::GenericException& e) {
    g_reporter->critical(e.what());
    throw;
  }

  ~BaslerBoostImpl() {
    for (auto& camera : cameras_) {
      if (camera.IsGrabbing())
        camera.StopGrabbing();
    }
  }

 private:
  static constexpr uint32_t kGrabTimeoutFirstFrameMs = 20000;
  static constexpr uint32_t kGrabTimeoutMs = 250;

  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final try {
    config_ = std::move(config);

    width_ = 0;
    height_ = 0;

    for (auto& camera : cameras_) {
      Pylon::CBaslerUniversalInstantCamera::TlParams_t& tl_params = camera.GetTLParams();

      // Configure ROI
      if (config_.camera_enable_roi) {
        SetRoi(
            camera,
            config_.camera_roi_width,
            config_.camera_roi_height,
            config_.camera_roi_x_offset,
            config_.camera_roi_y_offset);
      } else {
        SetRoi(camera, camera.SensorWidth.GetValue(), camera.SensorHeight.GetValue());
      }

      width_ = std::max(width_, static_cast<std::size_t>(tl_params.Width.GetValue()));
      height_ += static_cast<std::size_t>(tl_params.Height.GetValue());

      camera.ExposureTime.SetValue(config_.camera_exposure_time_us);

      // Report black level
      g_reporter->info(
          "BaslerBoost: Currently configured black level is {}", camera.BlackLevel.GetValue());

      if (config_.camera_enable_test_pattern)
        camera.TestPattern.SetValue(Basler_UniversalCameraParams::TestPattern_Testimage2);
      else
        camera.TestPattern.SetValue(Basler_UniversalCameraParams::TestPattern_Off);

      // Reset Counter1
      camera.CounterSelector.SetValue(Basler_UniversalCameraParams::CounterSelector_Counter1);
      camera.CounterReset.Execute();

      // Trigger Counter1 (necessary after reset)
      camera.CounterTriggerSource.SetValue(
          Basler_UniversalCameraParams::CounterTriggerSource_SoftwareSignal1);
      camera.SoftwareSignalSelector.SetValue(
          Basler_UniversalCameraParams::SoftwareSignalSelector_SoftwareSignal1);
      camera.SoftwareSignalPulse();
    }

    g_reporter->debug("BaslerBoost: composite resolution is {}x{}", width_, height_);

    for (auto& camera : cameras_) {
      camera.StartGrabbing();
    }
    // If this is the "secondary" Pando instance (not in control of the sync box/frame trigger
    // generation), we need to make sure that the frame trigger isn't running yet, because that
    // would mean that we missed an unknown number of frame triggers before the capture started.

    for (auto& camera : cameras_) {
      camera.GetTLParams().TriggerOutStatisticsPulseCountClear.Execute();
    }

    // This should time out, becasue the frame trigger hasn't been started yet.
    auto got_frame = cameras_.front().GetGrabResultWaitObject().Wait(
        kGrabTimeoutMs + (config_.camera_frame_trigger_period_10ns / 100000));
    auto trigout_cnt = cameras_.front().GetTLParams().TriggerOutStatisticsPulseCount.GetValue();
    if (got_frame || trigout_cnt > 0) {
      g_reporter->debug("got_frame = {}, trigout_cnt = {}", got_frame, trigout_cnt);
      throw std::runtime_error(
          "BaslerBoost: Frame trigger was already running at experiment start, likely missed some triggers.");
    }

    for (auto& camera : cameras_) {
      camera.GetTLParams().MissingCameraFrameResponseClear.Execute();
    }

    // Constructing frame_handler opens the raw H5 file (if raw logging is enabled). We do this
    // here, not in the grab thread, so that any exception thrown is immediately handled by the
    // caller.
    auto frame_handler = std::make_shared<CameraFrameHandler>(
        raw_file_name,
        pixel_type_,
        width_,
        height_,
        experiment_id,
        config.log_raw_data,
        config_.publish_raw_data);

    run_thread_ = common::ThreadContainer(
        [this, frame_handler] { Grab(std::move(frame_handler)); }, "BaslerBoost::Grab");
  } catch (GenICam_3_1_Basler_pylon::GenericException& e) {
    g_reporter->critical(e.what());
    throw;
  }

  void StopImpl() final {
    for (auto& camera : cameras_) {
      if (camera.IsGrabbing())
        camera.StopGrabbing();
    }
    run_thread_.Join();
  }

  void Grab(std::shared_ptr<CameraFrameHandler> frame_handler) try {
    const std::chrono::nanoseconds frame_period{10 * config_.camera_frame_trigger_period_10ns};

    BlockingFuture<void> handle_done;

    // ping-pong buffers for a single composite frame
    std::shared_ptr<uint8_t[]> buff_a = std::make_unique<uint8_t[]>(width_ * height_);
    std::shared_ptr<uint8_t[]> buff_b = std::make_unique<uint8_t[]>(width_ * height_);

    for (uint64_t frame_count = 0;; ++frame_count) {
      auto* frame_dst = buff_a.get();
      const auto dst_width = width_;

      for (auto& camera : cameras_) {
        Pylon::CBaslerUniversalInstantCamera::TlParams_t& tl_params = camera.GetTLParams();
        Pylon::CBaslerUniversalInstantCameraTraits::GrabResultPtr_t grab_result;

        try {
          auto timeout_ms = static_cast<int>(
              frame_count == 0
                  ? kGrabTimeoutFirstFrameMs
                  : kGrabTimeoutMs + (config_.camera_frame_trigger_period_10ns / 100000));
          camera.RetrieveResult(timeout_ms, grab_result, Pylon::TimeoutHandling_ThrowException);
        } catch (const GenICam_3_1_Basler_pylon::TimeoutException& e) {
          throw std::runtime_error(
              "BaslerBoost::Grab: RetrieveResult timed out. Check the frame trigger.");
        }

        if (!grab_result.IsValid()) {
          if (camera.IsGrabbing())
            throw std::runtime_error("BaslerBoost: grab_result is empty");
          else
            return; // Grab finished cleanly, we're done.
        } else if (!grab_result->GrabSucceeded()) {
          g_reporter->error("BaslerBoost: {}", grab_result->GetErrorDescription());
          throw std::runtime_error("BaslerBoost: CGrabResultData::GrabSucceeded returned false");
        }

        if (frame_count == 0) {
          // Clear missing frame register (it always gets set on the first frame for some reason,
          // but no frame is actually lost)
          tl_params.MissingCameraFrameResponseClear.Execute();
        } else {
          // Check for framegrabber overflow
          if (tl_params.Overflow.GetValue())
            throw std::runtime_error("BaslerBoost:  frame grabber reported internal overflow.");

          // Check for missing frames (grabber forwarded a trigger but didn't get a frame)
          if (tl_params.MissingCameraFrameResponse.GetValue())
            throw std::runtime_error(
                "BaslerBoost: Frame Grabber reports a trigger pulse didn't produce a frame (check trigger period & exposure setting)");

          // Check for correct FPS
          auto fps = tl_params.TriggerInStatisticsFrequency.GetValue();
          std::chrono::nanoseconds frame_period_observed{std::llround(1.0E9 / fps)};
          if (std::chrono::abs(frame_period - frame_period_observed) >
              std::chrono::microseconds(100)) {
            g_reporter->debug(
                "Observed frame delta {}ns, configured frame_period is {}ns",
                frame_period_observed.count(),
                frame_period.count());
            throw std::runtime_error(
                "BaslerBoost: Observed frame delta is off by >10us. Is correct trigger connected?");
          }
        }

        // Append the frame to buff_a line by line
        const auto src_width = grab_result->GetWidth();
        const auto src_height = grab_result->GetHeight();
        uint8_t const* src = static_cast<uint8_t*>(grab_result->GetBuffer());
        for (std::size_t row_idx = 0; row_idx < src_height; ++row_idx) {
          std::copy(src, src + src_width, frame_dst);
          src += src_width;
          frame_dst += dst_width;
        }
      }

      // Flip image buffer
      if (handle_done.valid())
        handle_done.get();
      std::swap(buff_a, buff_b);

      handle_done = frame_handler->Handle({
          buff_b,
          width_ * height_,
          frame_count * frame_period,
          std::chrono::duration<double, std::micro>{config_.camera_exposure_time_us},
      });
    }
  } catch (GenICam_3_1_Basler_pylon::GenericException& e) {
    g_reporter->critical(e.what());
    throw;
  }

  void SetRoi(
      Pylon::CBaslerUniversalInstantCamera& camera,
      int64_t width,
      int64_t height,
      int64_t x_offset = 0,
      int64_t y_offset = 0) {
    Pylon::CBaslerUniversalInstantCamera::TlParams_t& tl_params = camera.GetTLParams();

    auto camera_width_step = camera.Width.GetInc();
    auto tl_width_step = tl_params.Width.GetInc();
    auto width_step = std::max(camera_width_step, tl_width_step);

    auto camera_height_step = camera.Height.GetInc();
    auto tl_height_step = tl_params.Height.GetInc();
    auto height_step = std::max(camera_height_step, tl_height_step);

    auto x_offset_step = camera.OffsetX.GetInc();
    auto y_offset_step = camera.OffsetY.GetInc();

    width -= width % width_step;
    height -= height % height_step;
    x_offset -= x_offset % x_offset_step;
    y_offset -= y_offset % y_offset_step;

    camera.Width.SetValue(width);
    camera.Height.SetValue(height);
    camera.OffsetX.SetValue(x_offset);
    camera.OffsetY.SetValue(y_offset);
    tl_params.Width.SetValue(width);
    tl_params.Height.SetValue(height);
  }

  Pylon::PylonAutoInitTerm auto_init_term_; // library handle

  PandoInterface::Config config_;

  std::list<Pylon::CBaslerUniversalInstantCamera> cameras_;

  HDF5CompTypeFieldDescriptor::CppType pixel_type_;
  std::size_t width_;
  std::size_t height_;

  common::ThreadContainer run_thread_;
};

} // namespace

std::unique_ptr<BaslerBoost> BaslerBoost::Create() {
  return std::make_unique<BaslerBoostImpl>();
}

} // namespace pando
} // namespace pnd
