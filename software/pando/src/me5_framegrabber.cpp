#include "me5_framegrabber.h"

#include "camera_frame_handler.h"
#include "reporter.h"
#include "thread_container.h"

#include <fgrab_prototyp.h>
#include <fmt/format.h>

#include <cstddef>
#include <utility>

namespace pnd {
namespace pando {

namespace {

class Me5FrameGrabberImpl : public Me5FrameGrabber {
 private:
  static constexpr uint32_t kGrabTimeoutFirstFrameSec = 11;
  static constexpr uint32_t kGrabTimeoutSec = 1;
  static constexpr int32_t kBufferCount = 16;
  static constexpr int32_t kDmaPort = 0;

 public:
  Me5FrameGrabberImpl(uint32_t board_index) {
    // Init framegrabber and select applet. Stored deleter eventually calls Fg_FreeGrabber().
    fg_.reset(Fg_Init("Acq_SingleFullAreaGray.dll", board_index), [](Fg_Struct* fg) {
      Fg_FreeGrabber(fg);
    });
    if (!fg_)
      ThrowLastError(fg_.get());

    // Configure 1X10 taps
    SetParameter(FG_CAMERA_LINK_CAMTYPE, FG_CL_FULL_10_TAP_8_BIT);
    SetParameter(FG_TAPGEOMETRY, FG_GEOMETRY_1X10);

    // Set pixel format to 8 bit greyscale
    SetParameter(FG_FORMAT, FG_GRAY);

    // Configure external trigger input from front GPI0 (on DSUB connector)
    SetParameter(FG_AREATRIGGERMODE, ATM_EXTERNAL);
    SetParameter(FG_TRIGGERIN_DEBOUNCE, 1.008);
    SetParameter(FG_TRIGGERIN_SRC, TRGINSRC_FRONT_GPI_0);
    SetParameter(FG_TRIGGERIN_STATS_SOURCE, TRGINSRC_FRONT_GPI_0);
    // Note: This just caps the input frame trigger frequency, so set it very high
    SetParameter(FG_TRIGGER_FRAMESPERSECOND, 9999.0);
    // Note: trigger signal is inverted by our optoisolator board
    SetParameter(FG_TRIGGERIN_POLARITY, LOW_ACTIVE);
    SetParameter(FG_TRIGGERIN_STATS_POLARITY, LOW_ACTIVE);

    // Configure Pulse Form Generator 0 to output on CC0
    SetParameter(FG_TRIGGER_PULSEFORMGEN0_DOWNSCALE, 1);
    SetParameter(FG_TRIGGER_PULSEFORMGEN0_DOWNSCALE_PHASE, 0);
    SetParameter(FG_TRIGGER_PULSEFORMGEN0_DELAY, 0.0);
    SetParameter(FG_TRIGGER_PULSEFORMGEN0_WIDTH, 100.0);
    SetParameter(FG_TRIGGERCC_SELECT0, CAM_A_PULSEGEN0);
    SetParameter(FG_TRIGGEROUT_STATS_SOURCE, PULSEGEN0);
    SetParameter(FG_TRIGGERSTATE, TS_ACTIVE);
  }

 private:
  class Acquisition {
   public:
    // Start acquisition
    Acquisition(const std::shared_ptr<Fg_Struct>& fg, const std::shared_ptr<dma_mem>& mem)
        : fg_{fg}, mem_{mem} {
      if (Fg_AcquireEx(fg_.get(), kDmaPort, GRAB_INFINITE, ACQ_BLOCK, mem_.get()) != FG_OK)
        ThrowLastError(fg_.get());
    }

    // Stop acquisition
    ~Acquisition() {
      if (Fg_stopAcquireEx(fg_.get(), kDmaPort, mem_.get(), STOP_ASYNC) != FG_OK)
        ThrowLastError(fg_.get());
    }

   private:
    // Non-copyable + Non-movabe
    Acquisition(const Acquisition&) = delete;
    Acquisition& operator=(const Acquisition&) = delete;

    std::shared_ptr<Fg_Struct> fg_;
    std::shared_ptr<dma_mem> mem_;
  };

  void Start(
      int32_t experiment_id,
      bool log_raw_data,
      bool publish_raw_data,
      const char* raw_file_name,
      uint32_t width,
      uint32_t height,
      std::chrono::nanoseconds frame_period,
      std::chrono::duration<double, std::micro> exposure_time) final {
    // Set frame width & height
    SetParameter(FG_WIDTH, width);
    SetParameter(FG_HEIGHT, height);

    // Constructing frame_handler opens the raw H5 file (if raw logging is enabled). We do this
    // here, not in the grab thread, so that any exception thrown is immediately handled by the
    // caller.
    auto frame_handler = std::make_shared<CameraFrameHandler>(
        raw_file_name,
        HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8,
        width,
        height,
        experiment_id,
        log_raw_data,
        publish_raw_data);

    // Allocate framebuffers
    mem_.reset(
        Fg_AllocMemEx(fg_.get(), width * height * kBufferCount, kBufferCount),
        [fg = fg_](dma_mem* m) { Fg_FreeMemEx(fg.get(), m); });
    if (!mem_)
      ThrowLastError(fg_.get());

    // Start acquiring frames, eventual destruciton of acq will end acquisition
    auto acq = std::make_shared<Acquisition>(fg_, mem_);

    // If this is the "secondary" Pando instance (not in control of the sync box/frame trigger
    // generation), we need to make sure that the frame trigger isn't running yet, because that
    // would mean that we missed an unknown number of frame triggers before the capture started.
    SetParameter(FG_TRIGGERIN_STATS_PULSECOUNT_CLEAR, FG_APPLY);
    std::this_thread::sleep_for(std::chrono::milliseconds(250) + frame_period);
    if (GetParameter<uint32_t>(FG_TRIGGERIN_STATS_PULSECOUNT) > 0)
      throw std::runtime_error(
          "Me5FrameGrabber: Frame trigger was already running at experiment start, likely missed some triggers.");

    run_thread_ = common::ThreadContainer(
        [this, frame_handler, acq, frame_period, exposure_time] {
          Grab(std::move(frame_handler), std::move(acq), frame_period, exposure_time);
        },
        "Me5FrameGrabber::Grab",
        &stop_signal_);
  }

  void Stop() final {
    run_thread_.Stop();
    run_thread_.Join();
  }

  void Grab(
      std::shared_ptr<CameraFrameHandler> frame_handler,
      std::shared_ptr<Acquisition> acq,
      std::chrono::nanoseconds frame_period,
      std::chrono::duration<double, std::micro> exposure_time) {
    BlockingFuture<void> handle_done;

    for (uint64_t frame_count = 0; !stop_signal_.ShouldStop(); ++frame_count) {
      // Get the buffer index of the next image & lock it
      auto buf_idx = Fg_getImageEx(
          fg_.get(),
          SEL_ACT_IMAGE,
          0,
          kDmaPort,
          frame_count == 0 ? kGrabTimeoutFirstFrameSec : kGrabTimeoutSec,
          mem_.get());
      if (buf_idx < 0)
        ThrowLastError(fg_.get());

      // Get a pointer to the image buffer & store it in a shared_ptr w/ custom deleter that will
      // eventually unlock the buffer.
      std::shared_ptr<void> img{
          Fg_getImagePtrEx(fg_.get(), buf_idx, kDmaPort, mem_.get()),
          [buf_idx, fg = fg_, mem = mem_](void*) {
            Fg_setStatusEx(fg.get(), FG_UNBLOCK, buf_idx, kDmaPort, mem.get());
          }

      };

      // Check for software framebuffer overflow
      auto n_lost = Fg_getStatusEx(fg_.get(), NUMBER_OF_BLOCK_LOST_IMAGES, 0, kDmaPort, mem_.get());
      if (n_lost == FG_INVALID_PARAMETER)
        ThrowLastError(fg_.get());
      else if (n_lost != 0)
        throw std::runtime_error(
            "Me5FrameGrabber: software framebuffer overflow (HDF5 logging or zmq publishing too slow).");

      if (frame_count == 0) {
        // Clear missing frame register (it always gets set on the first frame for some reason, but
        // no frame is actually lost)
        SetParameter(FG_MISSING_CAMERA_FRAME_RESPONSE_CLEAR, FG_APPLY);
      } else {
        // Check for framegrabber overflow
        if (GetParameter<uint32_t>(FG_OVERFLOW))
          throw std::runtime_error("Me5FrameGrabber: frame grabber reported internal overflow.");

        // Check for missing frames (grabber forwarded a trigger but didn't get a frame)
        if (GetParameter<int>(FG_MISSING_CAMERA_FRAME_RESPONSE) == FG_YES)
          throw std::runtime_error(
              "Me5FrameGrabber: Frame Grabber reports a trigger pulse didn't produce a frame (check trigger period & exposure setting)");

        // Check for correct FPS
        auto fps = GetParameter<double>(FG_TRIGGERIN_STATS_FREQUENCY);
        std::chrono::nanoseconds frame_period_observed{std::llround(1.0E9 / fps)};

        if (std::chrono::abs(frame_period - frame_period_observed) >
            std::chrono::microseconds(10)) {
          g_reporter->debug(
              "Observed frame delta {}ns, configured frame_period is {}ns",
              frame_period_observed.count(),
              frame_period.count());
          throw std::runtime_error(
              "Me5FrameGrabber: Observed frame delta is off by >10us. Is correct trigger connected?");
        }
      }

      handle_done = frame_handler->Handle({
          img,
          GetParameterEx<size_t>(FG_TRANSFER_LEN, buf_idx),
          frame_count * frame_period,
          exposure_time,
      });
    }
  }

  static void ThrowLastError(Fg_Struct* fg) {
    auto msg =
        fmt::format("Error ({}) {}", Fg_getLastErrorNumber(fg), Fg_getLastErrorDescription(fg));
    throw(FgLibException(msg));
  }

  template <class T>
  void SetParameter(int parameter, T value) {
    if (Fg_setParameterWithType(fg_.get(), parameter, value, kDmaPort) != FG_OK)
      ThrowLastError(fg_.get());
  }

  template <class T>
  T GetParameter(int parameter) {
    T value;
    if (Fg_getParameterWithType(fg_.get(), parameter, &value, kDmaPort) != FG_OK)
      ThrowLastError(fg_.get());
    return value;
  }

  template <class T>
  T GetParameterEx(int parameter, frameindex_t buf_idx) {
    T value;
    if (Fg_getParameterEx(fg_.get(), parameter, &value, kDmaPort, mem_.get(), buf_idx) != FG_OK)
      ThrowLastError(fg_.get());
    return value;
  }

  std::shared_ptr<Fg_Struct> fg_;

  std::shared_ptr<dma_mem> mem_;

  common::StopSignal stop_signal_;
  common::ThreadContainer run_thread_;
};

} // namespace

std::unique_ptr<Me5FrameGrabber> Me5FrameGrabber::Create(uint32_t board_index) {
  return std::make_unique<Me5FrameGrabberImpl>(board_index);
}

} // namespace pando
} // namespace pnd