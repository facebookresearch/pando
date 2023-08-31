#include "camera_frame_handler.h"
#include "device_interface.h"
#include "pando.h"
#include "thread_container.h"

#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/PylonIncludes.h>

#include <memory>

namespace pnd {
namespace pando {

class BaslerA2 : public DeviceInterface {
 public:
  BaslerA2();
  ~BaslerA2();

 private:
  static constexpr uint32_t kGrabTimeoutFirstFrameMs = 20000;
  static constexpr uint32_t kGrabTimeoutMs = 250;

  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final;
  void StopImpl() final;

  void Grab(std::shared_ptr<CameraFrameHandler> frame_handler);

  Pylon::PylonAutoInitTerm auto_init_term_; // library handle

  PandoInterface::Config config_;

  Pylon::CBaslerUniversalInstantCamera camera_;

  HDF5CompTypeFieldDescriptor::CppType pixel_type_;
  std::size_t width_;
  std::size_t height_;

  common::ThreadContainer run_thread_;
};

} // namespace pando
} // namespace pnd
