#include "basler_a2.h"
#include "reporter.h"

#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

BaslerA2::BaslerA2() try {
  // Open the first available camera with appropriate model name
  try {
    Pylon::CDeviceInfo di;
    di.SetModelName("a2A1920-160umPRO");
    camera_.Attach(Pylon::CTlFactory::GetInstance().CreateFirstDevice(di));
    camera_.Open();
  } catch (const GenICam_3_1_Basler_pylon::GenericException& e) {
    g_reporter->error(e.what());
    throw std::runtime_error("BaslerA2: Couldn't open camera.");
  }

  g_reporter->info("BaslerA2: Opened camera with SN {}", camera_.GetDeviceInfo().GetSerialNumber());

  // TODO support nondefault bit depths
  // camera_.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_Mono8);

  g_reporter->debug("BaslerA2: Pixel format is {}", camera_.PixelFormat.ToString());

  switch (camera_.PixelFormat.GetValue()) {
    case Basler_UniversalCameraParams::PixelFormat_Mono8:
      pixel_type_ = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8;
      break;
    case Basler_UniversalCameraParams::PixelFormat_Mono10:
    case Basler_UniversalCameraParams::PixelFormat_Mono12:
    case Basler_UniversalCameraParams::PixelFormat_Mono16:
      pixel_type_ = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT16;
      break;
    default:
      throw std::runtime_error("BaslerA2: Camera has unsupported pixel format");
  }

  // Set up Line1 as an input
  camera_.LineSelector.SetValue(Basler_UniversalCameraParams::LineSelector_Line1);
  camera_.LineMode.SetValue(Basler_UniversalCameraParams::LineMode_Input);
  camera_.BslInputFilterTime.SetValue(100.00);
  camera_.BslInputHoldOffTime.SetValue(1000.01);

  // Use Line1 as hardware frame trigger
  camera_.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
  camera_.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_Line1);
  camera_.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);

  // Configure Counter1 to count frame trigger pulses
  camera_.CounterSelector.SetValue(Basler_UniversalCameraParams::CounterSelector_Counter1);
  camera_.CounterEventSource.SetValue(Basler_UniversalCameraParams::CounterEventSource_Line1);

  // Enable transmission of Counter1 chunk
  camera_.ChunkModeActive.SetValue(true);
  camera_.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_CounterValue);
  camera_.ChunkEnable.SetValue(true);

  // Configure exposure mode (exposure time is set in StartImpl)
  camera_.ExposureMode.SetValue(Basler_UniversalCameraParams::ExposureMode_Timed);
  camera_.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Off);

  // Enable transmission of time stamp chunk
  camera_.BslChunkTimestampSelector.SetValue(
      Basler_UniversalCameraParams::BslChunkTimestampSelector_ExposureStart);
  camera_.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Timestamp);
  camera_.ChunkEnable.SetValue(true);

} catch (GenICam_3_1_Basler_pylon::GenericException& e) {
  g_reporter->critical(e.what());
  throw;
}

BaslerA2::~BaslerA2() {
  if (camera_.IsGrabbing())
    camera_.StopGrabbing();
}

void BaslerA2::StartImpl(
    int32_t experiment_id,
    PandoInterface::Config config,
    const char* raw_file_name) try {
  config_ = std::move(config);

  // Configure ROI
  if (config_.camera_enable_roi) {
    camera_.OffsetX.SetValue(config_.camera_roi_x_offset);
    camera_.OffsetY.SetValue(config_.camera_roi_y_offset);
    camera_.Width.SetValue(config_.camera_roi_width);
    camera_.Height.SetValue(config_.camera_roi_height);
  }

  width_ = static_cast<std::size_t>(camera_.Width.GetValue());
  height_ = static_cast<std::size_t>(camera_.Height.GetValue());
  g_reporter->debug("BaslerA2: resolution is {}x{}", width_, height_);

  camera_.ExposureTime.SetValue(config_.camera_exposure_time_us);

  // Report black level
  g_reporter->info(
      "BaslerA2: Currently configured black level is {}", camera_.BlackLevel.GetValue());

  if (config_.camera_enable_test_pattern)
    camera_.TestPattern.SetValue(Basler_UniversalCameraParams::TestPattern_Testimage2);
  else
    camera_.TestPattern.SetValue(Basler_UniversalCameraParams::TestPattern_Off);

  // Reset Counter1
  camera_.CounterSelector.SetValue(Basler_UniversalCameraParams::CounterSelector_Counter1);
  camera_.CounterReset.Execute();

  // Trigger Counter1 (necessary after reset)
  camera_.CounterTriggerSource.SetValue(
      Basler_UniversalCameraParams::CounterTriggerSource_SoftwareSignal1);
  camera_.SoftwareSignalSelector.SetValue(
      Basler_UniversalCameraParams::SoftwareSignalSelector_SoftwareSignal1);
  camera_.SoftwareSignalPulse();

  camera_.StartGrabbing();

  // If this is the "secondary" Pando instance (not in control of the sync box/frame trigger
  // generation), we need to make sure that the frame trigger isn't running yet, because that would
  // mean that we missed an unknown number of frame triggers before the capture started.

  // This should time out, becasue the frame trigger hasn't been started yet.
  if (camera_.GetGrabResultWaitObject().Wait(
          kGrabTimeoutMs + (config_.camera_frame_trigger_period_10ns / 100000)))
    throw std::runtime_error(
        "BaslerA2: Frame trigger was already running at experiment start, likely missed some triggers.");

  // Constructing frame_handler opens the raw H5 file (if raw logging is enabled). We do this here,
  // not in the grab thread, so that any exception thrown is immediately handled by the caller.
  auto frame_handler = std::make_shared<CameraFrameHandler>(
      raw_file_name,
      pixel_type_,
      width_,
      height_,
      experiment_id,
      config.log_raw_data,
      config_.publish_raw_data);

  run_thread_ = common::ThreadContainer(
      [this, frame_handler] { Grab(std::move(frame_handler)); }, "BaslerA2::Grab");
} catch (GenICam_3_1_Basler_pylon::GenericException& e) {
  g_reporter->critical(e.what());
  throw;
}

void BaslerA2::StopImpl() {
  if (camera_.IsGrabbing())
    camera_.StopGrabbing();
  run_thread_.Join();
}

void BaslerA2::Grab(std::shared_ptr<CameraFrameHandler> frame_handler) try {
  // This smart pointer will receive the grab result data.
  Pylon::CBaslerUniversalInstantCameraTraits::GrabResultPtr_t grab_result;

  int64_t frame_count = 0;
  const std::chrono::nanoseconds frame_period{10 * config_.camera_frame_trigger_period_10ns};
  std::chrono::nanoseconds prev_native_timestamp{-1};

  BlockingFuture<void> handle_done;

  bool dropped_one_frame{};

  while (true) {
    try {
      auto timeout_ms = static_cast<int>(
          frame_count == 0 ? kGrabTimeoutFirstFrameMs
                           : kGrabTimeoutMs + (config_.camera_frame_trigger_period_10ns / 100000));
      camera_.RetrieveResult(timeout_ms, grab_result, Pylon::TimeoutHandling_ThrowException);
    } catch (const GenICam_3_1_Basler_pylon::TimeoutException& e) {
      throw std::runtime_error(
          "BaslerA2::Grab: RetrieveResult timed out. Check the frame trigger.");
    }

    if (!grab_result.IsValid()) {
      if (camera_.IsGrabbing())
        throw std::runtime_error("BaslerA2: grab_result is empty");
      else
        break; // Grab finished cleanly, we're done.
    } else if (!grab_result->GrabSucceeded()) {
      g_reporter->error("BaslerA2: {}", grab_result->GetErrorDescription());
      throw std::runtime_error("BaslerA2: CGrabResultData::GrabSucceeded returned false");
    }

    grab_result->ChunkCounterSelector.SetValue(
        Basler_UniversalChunkDataParams::ChunkCounterSelector_Counter1);
    auto frame_trig_cnt = grab_result->ChunkCounterValue.GetValue();

    // Check for dropped frames (frame_trig_cnt starts at 1)
    if (frame_trig_cnt != ++frame_count) {
      g_reporter->debug("BaslerA2: frame_trig_cnt: {}, expected {}", frame_trig_cnt, frame_count);

      if (frame_trig_cnt == frame_count + 1) {
        if (dropped_one_frame)
          throw std::runtime_error("BaslerA2: Dropped two individual frames in a row");

        dropped_one_frame = true;
        ++frame_count;
        g_reporter->warn("BaslerA2: Detected a single dropped frame, continuing.");
      } else
        throw std::runtime_error(
            "BaslerA2: Detected dropped frame (check trigger period & exposure setting)");
    } else {
      dropped_one_frame = false;
    }

    // Check for correct FPS
    // a2A1920-160umPRO has 1e9 ticks/second, starts counting at power up
    std::chrono::nanoseconds native_timestamp{grab_result->BslChunkTimestampValue.GetValue()};
    if (prev_native_timestamp.count() >= 0 && !dropped_one_frame) {
      std::chrono::nanoseconds delta = native_timestamp - prev_native_timestamp;

      if (std::chrono::abs(frame_period - delta) > std::chrono::microseconds(10)) {
        g_reporter->debug(
            "Observed frame delta {}ns, configured frame_period is {}ns",
            delta.count(),
            frame_period.count());
        throw std::runtime_error(
            "BaslerA2: Observed frame delta is off by >10us. Is correct trigger connected, or is trigger too fast?");
      }
    }
    prev_native_timestamp = native_timestamp;

    assert(grab_result->GetWidth() == width_);
    assert(grab_result->GetHeight() == height_);

    if (handle_done.valid())
      handle_done.get();

    handle_done = frame_handler->Handle({
        // Use the aliasing constructor of shared_ptr to own the original
        // Pylon::CGrabResultPtr while pointing to the image buffer it owns.
        {std::make_shared<Pylon::CGrabResultPtr>(grab_result), grab_result->GetBuffer()},
        Pylon::ComputeBufferSize(
            grab_result->GetPixelType(),
            grab_result->GetWidth(),
            grab_result->GetHeight(),
            grab_result->GetPaddingX()),
        (frame_trig_cnt - 1) * frame_period,
        std::chrono::duration<double, std::micro>{config_.camera_exposure_time_us},
    });
  }

} catch (GenICam_3_1_Basler_pylon::GenericException& e) {
  g_reporter->critical(e.what());
  throw;
}

} // namespace pando
} // namespace pnd
