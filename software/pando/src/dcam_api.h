#pragma once

#include <cstdint>
#include <memory>
#include <utility>

#include <type_traits>

namespace pnd {
namespace pando {
namespace dcam {

namespace detail {
template <class Fn>
struct FuncArg0Impl;

template <class Ret, class A, class... An>
struct FuncArg0Impl<Ret(__cdecl*)(A, An...)> {
  using Type = A;
};

/** Type trait that determines the type of the first argument of a function. */
template <class Fn>
using FuncArg0 = typename FuncArg0Impl<Fn>::Type;
} // namespace detail

/** Singleton instance of DCAMAPI_INIT w/ RAII around dcamapi_init() & dcamapi_uninit(). */
struct DcamApi {
 public:
  virtual ~DcamApi() = 0;

  /** Getter for the global singleton DcamApi instance.
   *  Constructs a new instance (initializing the API) one if it doesn't exist. */
  static std::shared_ptr<DcamApi> Instance();

  /** Getter for iDeviceCount member of DCAMAPI_INIT. */
  virtual int32_t DeviceCount() = 0;
};

/** Instance of HDCAM.
 * Implements RAII (using shared_ptr<void> with a custom deleter) around:
 *   1. dcamdev_open()/dcamdev_close()
 *   2. dcamwait_open()/dcamwait_close()
 *   3. dcambuf_alloc()/dcambuf_release()
 *
 * User of this class will call methods from dcamapi4.h using CallWithHDCam and CallWithHDCamWait.
 * These wrappers handle supplying the first argument and translate return codes into exceptions.
 */
class Cam {
 public:
  Cam(int32_t dev_index = 0, int32_t n_buffers = 10);

  /** Helper to call API methods that take HDCAM as their first argument.
   * The supplied API method must:
   *   1. Take HDCAM as its first argument
   *   2. return DCAMERR.
   */
  template <class Fn, class... Args>
  void CallWithHDCam(Fn m, Args&&... args) {
    // FuncArg0<Fn> evaluates to HDCAM, but this allows us to not #include <dcamapi4.h> yet
    CheckRet(m(static_cast<detail::FuncArg0<Fn>>(hdcam_.get()), std::forward<Args>(args)...));
  }

  /** Helper to call API methods that take HDCAMWAIT as their first argument.
   * The supplied API method must:
   *   1. Take HDCAMWAIT as its first argument
   *   2. return DCAMERR.
   */
  template <class Fn, class... Args>
  void CallWithHDCamWait(Fn m, Args&&... args) {
    // FuncArg0<Fn> evaluates to HDCAM, but this allows us to not #include <dcamapi4.h> yet
    CheckRet(m(static_cast<detail::FuncArg0<Fn>>(hwait_.get()), std::forward<Args>(args)...));
  }

 private:
  /** Translates DCAMERR return values into exceptions. */
  void CheckRet(int err_i);

  std::shared_ptr<DcamApi> api_;
  std::shared_ptr<void> hdcam_;
  std::shared_ptr<void> hwait_;
  std::shared_ptr<void> dcambuff_handle_;
};

} // namespace dcam
} // namespace pando
} // namespace pnd