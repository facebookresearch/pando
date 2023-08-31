#include "dcam_api.h"
#include "dcam_err.h"

#include <dcamapi4.h>

#include <mutex>
#include <thread>

namespace pnd {
namespace pando {
namespace dcam {

class DcamApiImpl : public DcamApi {
 public:
  DcamApiImpl() {
    init_.size = sizeof(init_);

    DCAMERR err = dcamapi_init(&init_);

    if (failed(err))
      throw Exception(err);
  }

  ~DcamApiImpl() {
    dcamapi_uninit();
  }

 private:
  int32_t DeviceCount() final {
    return init_.iDeviceCount;
  }

  DCAMAPI_INIT init_{};
};

DcamApi::~DcamApi() = default;

std::shared_ptr<DcamApi> DcamApi::Instance() {
  static std::mutex creation_mutex;
  static std::weak_ptr<DcamApi> instance_wp;

  while (true) {
    std::shared_ptr<DcamApi> instance_sp = instance_wp.lock();

    if (instance_sp) {
      // An instance already exists, return it
      return instance_sp;
    } else if (std::unique_lock l{creation_mutex, std::try_to_lock}) {
      // No instance exists, make a new one and return it
      instance_sp = std::make_shared<DcamApiImpl>();
      instance_wp = instance_sp;
      return instance_sp;
    } else {
      // No instance exists but another thread is already making one. Wait.
      std::this_thread::yield();
    }
  }
}

Cam::Cam(int32_t dev_index, int32_t n_buffers) : api_{DcamApi::Instance()} {
  DCAMDEV_OPEN dev_open{};
  dev_open.size = sizeof(dev_open);
  dev_open.index = dev_index;
  CheckRet(dcamdev_open(&dev_open));
  hdcam_ = std::shared_ptr<tag_dcam>(dev_open.hdcam, dcamdev_close);

  // Call dcamwait_open() to get a HDCAMWAIT, and create a handle that will eventually call
  // dcamwait_close() upon destruction
  DCAMWAIT_OPEN wait_open{};
  wait_open.size = sizeof(wait_open);
  wait_open.hdcam = dev_open.hdcam;
  CheckRet(dcamwait_open(&wait_open));
  hwait_ = std::shared_ptr<DCAMWAIT>(wait_open.hwait, dcamwait_close);

  // Call dcambuf_alloc(), and create a handle that will eventually call dcamwait_close() upon
  // destructrion
  CheckRet(dcambuf_alloc(dev_open.hdcam, n_buffers));
  dcambuff_handle_ = std::shared_ptr<tag_dcam>(dev_open.hdcam, [](HDCAM c) { dcambuf_release(c); });
}

void Cam::CheckRet(int err_i) {
  auto err = static_cast<DCAMERR>(err_i);
  if (failed(err))
    throw Exception(err, static_cast<HDCAM>(hdcam_.get()));
}

} // namespace dcam

} // namespace pando
} // namespace pnd
