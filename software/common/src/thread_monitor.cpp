#include <thread>

#include "thread_monitor.h"

namespace pnd {
namespace common {

std::shared_ptr<ThreadMonitor> ThreadMonitor::Get() {
  while (true) {
    std::shared_ptr<ThreadMonitor> monitor_sp = monitor_wp_.lock();
    if (monitor_sp) {
      // monitor already exists, return it
      return monitor_sp;
    } else if (auto l = std::unique_lock<std::mutex>(creation_mutex_, std::try_to_lock)) {
      // no monitor exists, create & return it
      monitor_sp = std::make_shared<ThreadMonitor>();
      monitor_wp_ = monitor_sp;
      return monitor_sp;
    } else {
      // Another thread is already trying to create a monitor. Wait.
      std::this_thread::yield();
    }
  }
}

std::mutex ThreadMonitor::creation_mutex_;
std::weak_ptr<ThreadMonitor> ThreadMonitor::monitor_wp_;

} // namespace common
} // namespace pnd
