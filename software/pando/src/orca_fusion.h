#include "camera_frame_handler.h"
#include "dcam_api.h"
#include "device_interface.h"
#include "pando.h"
#include "thread_container.h"

#include <cstddef>
#include <memory>
#include <optional>

namespace pnd {
namespace pando {

class OrcaFusion : public DeviceInterface {
 public:
  OrcaFusion();

 private:
  static constexpr uint32_t kGrabTimeoutFirstFrameMs = 20000;
  static constexpr uint32_t kGrabTimeoutMs = 250;
  static constexpr int32_t kRingBufferNFrames = 16;

  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final;
  void StopImpl() final;

  void Grab(std::shared_ptr<CameraFrameHandler> frame_handler);

  PandoInterface::Config config_;

  std::shared_ptr<dcam::DcamApi> api_;
  std::optional<dcam::Cam> cam_;

  HDF5CompTypeFieldDescriptor::CppType pixel_type_;
  std::size_t width_;
  std::size_t height_;

  common::ThreadContainer run_thread_;
};

} // namespace pando
} // namespace pnd