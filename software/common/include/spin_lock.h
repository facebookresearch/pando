#pragma once

#include <atomic>
#include <thread>

namespace pnd {
namespace common {

/** A simple userspace spinlock using std::atomic_flag. */
class SpinLock {
 public:
  void lock() {
    while (!try_lock()) {
      std::this_thread::yield();
    }
  }

  bool try_lock() {
    return !flag_.test_and_set(std::memory_order_acquire);
  }

  void unlock() {
    flag_.clear(std::memory_order_release);
  }

 private:
  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

} // namespace common
} // namespace pnd
