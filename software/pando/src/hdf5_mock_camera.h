#include "camera_frame_handler.h"
#include "device_interface.h"
#include "hdf5_comptype_accessor.h"
#include "hdf5_table.h"
#include "mutex.h"
#include "pando.h"
#include "thread_container.h"

#include <string>

namespace pnd {
namespace pando {

class HDF5MockCamera : public DeviceInterface {
 public:
  HDF5MockCamera(std::string file_path);

 private:
  void StartImpl(int32_t experiment_id, PandoInterface::Config config, const char* raw_file_name)
      final;
  void StopImpl() final;

  void Grab(std::shared_ptr<CameraFrameHandler> frame_handler);

  H5LockAdapter<H5::H5File> file_;
  HDF5Table image_table_;
  HDF5Table metadata_table_;

  PandoInterface::Config config_;

  HDF5CompTypeFieldDescriptor::CppType pixel_type_;
  std::size_t width_;
  std::size_t height_;

  common::StopSignal stop_signal_;
  common::ThreadContainer run_thread_;
};

} // namespace pando
} // namespace pnd
