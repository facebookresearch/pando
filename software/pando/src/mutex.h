#pragma once

#include <mutex>
#include <utility>

namespace pnd {
namespace pando {

/** Global mutex for accessing the HDF5 library */
extern std::recursive_mutex g_hdf5_mutex;

/** std::unique_lock initialized with g_hdf5_mutex */
struct H5Lock : std::unique_lock<std::recursive_mutex> {
  H5Lock() : std::unique_lock<std::recursive_mutex>(g_hdf5_mutex){};
};

/** std::lock_guard initialized with g_hdf5_mutex */
struct H5LockGuard : std::lock_guard<std::recursive_mutex> {
  H5LockGuard() : std::lock_guard<std::recursive_mutex>(g_hdf5_mutex){};
};

/** Wrapper that locks g_hdf5_mutex during construction & destruction of the wrapped class.
 * Also locks during use of assignment operators. All other member function calls should be
 * explicitly protected using H5LockGuard.
 *
 *  The effect of inheriting from H5Lock first and the wrapped class second is:
 *   1. The mutex is locked before the wrapped class is constructed
 *   2. The mutex is unlocked after the wrapped class is destructed.
 */
template <class T>
class H5LockAdapter : private H5Lock, public T {
 public:
  template <class... Args>
  /** Delegating constructor with perfect forwarding. */
  H5LockAdapter(Args&&... args) : T(std::forward<Args>(args)...) {
    unlock(); // Unlock mutex after wrapped class is constructed
  }

  ~H5LockAdapter() {
    std::unique_lock<std::recursive_mutex>::lock(); // Lock mutex before wrapped class is destructed
  }

  /** Copy assignment operator. */
  H5LockAdapter& operator=(const H5LockAdapter& rhs) {
    H5LockGuard l;
    T::operator=(rhs);
    return *this;
  }

  /** Move assignment operator. */
  H5LockAdapter& operator=(H5LockAdapter&& rhs) {
    H5LockGuard l;
    T::operator=(std::move(rhs));
    return *this;
  }
};

} // namespace pando
} // namespace pnd
