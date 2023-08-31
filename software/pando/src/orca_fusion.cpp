#include "orca_fusion.h"
#include "dcam_err.h"
#include "reporter.h"
#include "saturating_copy.h"

#include <dcamapi4.h>
#include <dcamprop.h>

#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

namespace pnd {
namespace pando {

OrcaFusion::OrcaFusion() : api_{dcam::DcamApi::Instance()} {
  if (api_->DeviceCount() > 1)
    throw std::runtime_error("DCAM API found more than 1 connected camera");

  cam_.emplace(0, kRingBufferNFrames);

  // Get Model number
  std::string model_number(256, '\0');
  DCAMDEV_STRING mn_param{
      sizeof(DCAMDEV_STRING),
      DCAM_IDSTR_MODEL,
      model_number.data(),
      model_number.size(),
  };
  cam_->CallWithHDCam(dcamdev_getstring, &mn_param);
  model_number.erase(model_number.find('\0'));

  // Get Serial number
  std::string serial_number(256, '\0');
  DCAMDEV_STRING sn_param{
      sizeof(DCAMDEV_STRING),
      DCAM_IDSTR_CAMERAID,
      serial_number.data(),
      serial_number.size(),
  };
  cam_->CallWithHDCam(dcamdev_getstring, &sn_param);
  serial_number.erase(serial_number.find('\0'));

  g_reporter->info("OrcaFusion: Opened camera with MN {} SN {}", model_number, serial_number);

  // TODO support nondefault bit depths

  double pt_f;
  cam_->CallWithHDCam(dcamprop_getvalue, DCAM_IDPROP_IMAGE_PIXELTYPE, &pt_f);
  switch (static_cast<DCAM_PIXELTYPE>(static_cast<int>(std::round(pt_f)))) {
    case DCAM_PIXELTYPE_MONO8:
      pixel_type_ = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8;
      break;
    case DCAM_PIXELTYPE_MONO12:
    case DCAM_PIXELTYPE_MONO16:
      pixel_type_ = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT16;
      break;
    default:
      throw std::runtime_error("OrcaFusion: Camera has unsupported pixel format");
  }

  // Set up external trigger
  cam_->CallWithHDCam(
      dcamprop_setvalue, DCAM_IDPROP_TRIGGERSOURCE, DCAMPROP_TRIGGERSOURCE__EXTERNAL);
  cam_->CallWithHDCam(dcamprop_setvalue, DCAM_IDPROP_TRIGGERACTIVE, DCAMPROP_TRIGGERACTIVE__EDGE);
  cam_->CallWithHDCam(
      dcamprop_setvalue, DCAM_IDPROP_TRIGGERPOLARITY, DCAMPROP_TRIGGERPOLARITY__POSITIVE);

  // Set up VSYNC output (one exposure length after trigger)
  cam_->CallWithHDCam(
      dcamprop_setvalue, DCAM_IDPROP_OUTPUTTRIGGER_KIND, DCAMPROP_OUTPUTTRIGGER_KIND__PROGRAMABLE);
  cam_->CallWithHDCam(
      dcamprop_setvalue, DCAM_IDPROP_OUTPUTTRIGGER_SOURCE, DCAMPROP_OUTPUTTRIGGER_SOURCE__VSYNC);
  cam_->CallWithHDCam(
      dcamprop_setvalue,
      DCAM_IDPROP_OUTPUTTRIGGER_POLARITY,
      DCAMPROP_OUTPUTTRIGGER_POLARITY__POSITIVE);

  // std::this_thread::sleep_for(std::chrono::seconds(1));

  // Verify readout speed.
  // For some reason, trying to set it here with dcamprop_setvalue leads to a "Status is not
  // stable!" error. But the default of 3.0 (fast) is what we want anyways.
  double readout_speed;
  cam_->CallWithHDCam(dcamprop_getvalue, DCAM_IDPROP_READOUTSPEED, &readout_speed);
  if (readout_speed != 3.0) {
    g_reporter->debug("DCAM_IDPROP_READOUTSPEED is {}", readout_speed);
    throw std::runtime_error("OrcaFusion: Readout speed is not set to fast as expected");
  }
}

void OrcaFusion::StartImpl(
    int32_t experiment_id,
    PandoInterface::Config config,
    const char* raw_file_name) {
  config_ = std::move(config);

  if (config_.camera_enable_roi) {
    // Round down all parameters to multiples of 4 as required by camera)
    cam_->CallWithHDCam(dcamprop_setvalue, DCAM_IDPROP_SUBARRAYMODE, DCAMPROP_MODE__OFF);
    cam_->CallWithHDCam(
        dcamprop_setvalue, DCAM_IDPROP_SUBARRAYHPOS, (config_.camera_roi_x_offset / 4) * 4);
    cam_->CallWithHDCam(
        dcamprop_setvalue, DCAM_IDPROP_SUBARRAYVPOS, (config_.camera_roi_y_offset / 4) * 4);
    cam_->CallWithHDCam(
        dcamprop_setvalue, DCAM_IDPROP_SUBARRAYHSIZE, (config_.camera_roi_width / 4) * 4);
    cam_->CallWithHDCam(
        dcamprop_setvalue, DCAM_IDPROP_SUBARRAYVSIZE, (config_.camera_roi_height / 4) * 4);
    cam_->CallWithHDCam(dcamprop_setvalue, DCAM_IDPROP_SUBARRAYMODE, DCAMPROP_MODE__ON);
  }

  // Get width & height
  double width_f, height_f;
  cam_->CallWithHDCam(dcamprop_getvalue, DCAM_IDPROP_IMAGE_WIDTH, &width_f);
  cam_->CallWithHDCam(dcamprop_getvalue, DCAM_IDPROP_IMAGE_HEIGHT, &height_f);

  width_ = static_cast<std::size_t>(std::round(width_f));
  height_ = static_cast<std::size_t>(std::round(height_f));
  g_reporter->debug("OrcaFusion: Current resolution is {}x{}", width_, height_);

  if (config_.camera_enable_test_pattern)
    throw std::runtime_error(
        "OrcaFusion: camera_enable_test_pattern is not supported for this device.");

  // Set the exposure
  cam_->CallWithHDCam(
      dcamprop_setvalue, DCAM_IDPROP_EXPOSURETIME, config_.camera_exposure_time_us * 1.0e-6);

  // Start the capture
  cam_->CallWithHDCam(dcamcap_start, DCAMCAP_START_SEQUENCE);

  // If this is the "secondary" Pando instance (not in control of the sync box/frame trigger
  // generation), we need to make sure that the frame trigger isn't running yet, because that would
  // mean that we missed an unknown number of frame triggers before the capture started.
  try {
    // dcamwait_start param
    DCAMWAIT_START wait_frame_ready{};
    wait_frame_ready.size = sizeof(wait_frame_ready);
    wait_frame_ready.eventmask = DCAMWAIT_CAPEVENT_FRAMEREADY;
    wait_frame_ready.timeout =
        static_cast<int32_t>(kGrabTimeoutMs + (config_.camera_frame_trigger_period_10ns / 100000));

    // This should time out, becasue the frame trigger hasn't been started yet.
    cam_->CallWithHDCamWait(dcamwait_start, &wait_frame_ready);

    throw std::runtime_error(
        "OrcaFusion: Frame trigger was already running at experiment start, likely missed some triggers.");
  } catch (const dcam::Exception& e) {
    if (e.err() != DCAMERR_TIMEOUT)
      throw;
  }

  // Constructing frame_handler opens the raw H5 file (if raw logging is enabled). We do this
  // here, not in the grab thread, so that any exception thrown is immediately handled by the
  // caller.
  auto frame_handler = std::make_shared<CameraFrameHandler>(
      raw_file_name,
      // NOTE: we convert all images to 8 bit before passing them to frame_handler
      // pixel_type_,
      HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8,
      width_,
      height_,
      experiment_id,
      config.log_raw_data,
      config_.publish_raw_data);

  run_thread_ = common::ThreadContainer(
      [this, frame_handler] { Grab(std::move(frame_handler)); }, "OrcaFusion::Grab");
}

void OrcaFusion::StopImpl() {
  cam_->CallWithHDCamWait(dcamwait_abort);
  run_thread_.Join();
}

void OrcaFusion::Grab(std::shared_ptr<CameraFrameHandler> frame_handler) {
  // Handle that will call dcamcap_stop() on thread exit
  struct CapStopper {
    CapStopper(dcam::Cam& cam) : cam_{cam} {}
    ~CapStopper() {
      cam_.CallWithHDCam(dcamcap_stop);
    }
    dcam::Cam& cam_;
  } cap_stopper{*cam_};

  DCAM_TIMESTAMP prev_frame_timestamp{};
  int32_t frame_count = 0;
  const std::chrono::nanoseconds frame_period{10 * config_.camera_frame_trigger_period_10ns};

  BlockingFuture<void> handle_done;

  // dcamwait_start param
  DCAMWAIT_START wait_frame_ready{};
  wait_frame_ready.size = sizeof(wait_frame_ready);
  wait_frame_ready.eventmask = DCAMWAIT_CAPEVENT_FRAMEREADY;

  // dcamcap_transferinfo param
  DCAMCAP_TRANSFERINFO transferinfo{};
  transferinfo.size = sizeof(transferinfo);

  // ping-pong buffers for a single frame
  std::shared_ptr<uint8_t[]> buff_a = std::make_unique<uint8_t[]>(width_ * height_);
  std::shared_ptr<uint8_t[]> buff_b = std::make_unique<uint8_t[]>(width_ * height_);

  while (true) {
    // Wait for a transfer to complete
    try {
      wait_frame_ready.timeout = static_cast<int32_t>(
          frame_count == 0 ? kGrabTimeoutFirstFrameMs
                           : kGrabTimeoutMs + (config_.camera_frame_trigger_period_10ns / 100000));

      cam_->CallWithHDCamWait(dcamwait_start, &wait_frame_ready);
    } catch (const dcam::Exception& e) {
      if (e.err() == DCAMERR_ABORT)
        break; // Grab finished cleanly, we're done.
      else
        throw;
    }

    // Get the number of new frames
    cam_->CallWithHDCam(dcamcap_transferinfo, &transferinfo);

    // nFrameCount and nNewestFrameIndex appear to be redundant?
    assert((transferinfo.nFrameCount - 1) % kRingBufferNFrames == transferinfo.nNewestFrameIndex);

    // auto n_new_frames = transferinfo.nFrameCount - frame_count;
    // if (n_new_frames > 1)
    //   g_reporter->debug("Got {} frames in one transfer", n_new_frames);

    // Process each new frame
    for (; frame_count < transferinfo.nFrameCount; ++frame_count) {
      // Lock the oldest frame
      DCAMBUF_FRAME frame{};
      frame.size = sizeof(frame);
      frame.iFrame = frame_count % kRingBufferNFrames;
      cam_->CallWithHDCam(dcambuf_lockframe, &frame);

      // Check for missed trigger pulses
      // If an external trigger comes before the camera is ready for it, it's ignored. The only way
      // to detect this is to compute the instantaneous frame period based on the camera-produced
      // timestamps. These are unix timestamps split into seconds and microseconds.
      if (prev_frame_timestamp.sec > 0) {
        std::chrono::microseconds delta =
            std::chrono::seconds(frame.timestamp.sec - prev_frame_timestamp.sec) +
            std::chrono::microseconds(frame.timestamp.microsec - prev_frame_timestamp.microsec);

        if (std::chrono::abs(frame_period - delta) > std::chrono::microseconds(10)) {
          g_reporter->debug(
              "Observed frame delta {}us, configured frame_period is {}ns",
              delta.count(),
              frame_period.count());
          throw std::runtime_error(
              "OrcaFusion: Observed frame delta is off by >10us. Is correct trigger connected, or is trigger too fast?");
        }
      }
      prev_frame_timestamp = frame.timestamp;

      // Check for dropped frame
      if (frame.framestamp != frame_count) {
        g_reporter->debug("framestamp={}, frame_count={}", frame.framestamp, frame_count);
        throw std::runtime_error("OrcaFusion: Detected dropped frame");
      }

      // Copy the frame
      const auto width = width_;
      const auto height = height_;
      if (pixel_type_ == HDF5CompTypeFieldDescriptor::CPPTYPE_UINT16) {
        for (int i = 0; i < height; ++i) {
          auto src_row_start =
              reinterpret_cast<uint16_t*>(static_cast<uint8_t*>(frame.buf) + i * frame.rowbytes);
          auto dst_row_start = buff_a.get() + i * width;
          SaturatingCopy(src_row_start, src_row_start + width, dst_row_start);
        }
      } else if (pixel_type_ == HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8) {
        for (int i = 0; i < height; ++i) {
          auto src_row_start = static_cast<uint8_t*>(frame.buf) + i * frame.rowbytes;
          auto dst_row_start = buff_a.get() + i * width;
          std::copy(src_row_start, src_row_start + width, dst_row_start);
        }
      }

      // Flip image buffer
      if (handle_done.valid())
        handle_done.get();
      std::swap(buff_a, buff_b);

      handle_done = frame_handler->Handle({
          buff_b,
          width_ * height_,
          frame.framestamp * frame_period,
          std::chrono::duration<double, std::micro>{config_.camera_exposure_time_us},
      });
    }
  }
}

} // namespace pando
} // namespace pnd
