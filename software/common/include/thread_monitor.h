#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <string>

namespace pnd {
namespace common {
class ThreadMonitor {
 public:
  static std::shared_ptr<ThreadMonitor> Get();

  void HandleThreadCrashed(std::string thread_name) {
    std::unique_lock<std::mutex> list_lock(crashed_threads_mutex_);
    crashed_threads_.push_back(thread_name);
  }

  std::list<std::string> GetCrashedThreadNames() {
    std::unique_lock<std::mutex> list_lock(crashed_threads_mutex_);
    return crashed_threads_;
  }

  size_t GetCrashedThreadCount() {
    std::unique_lock<std::mutex> list_lock(crashed_threads_mutex_);
    return crashed_threads_.size();
  }

  void Reset() {
    std::unique_lock<std::mutex> list_lock(crashed_threads_mutex_);
    crashed_threads_.clear();
  }

 private:
  std::mutex crashed_threads_mutex_;
  std::list<std::string> crashed_threads_;
  static std::mutex creation_mutex_;
  static std::weak_ptr<ThreadMonitor> monitor_wp_;
};

} // namespace common
} // namespace pnd
