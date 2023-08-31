#include "singleton_context.h"

#include <algorithm>
#include <thread>

namespace pnd {
namespace common {

std::shared_ptr<zmq::context_t> SingletonContext::Get() {
  while (true) {
    std::shared_ptr<zmq::context_t> context_sp = context_wp_.lock();

    if (context_sp) {
      // A context already exists, return it
      return context_sp;
    } else if (auto l = std::unique_lock<std::mutex>(creation_mutex_, std::try_to_lock)) {
      // No context exists, make a new one and return it
      context_sp = std::make_shared<zmq::context_t>(1);
      context_wp_ = context_sp;
      return context_sp;
    } else {
      // No context exists but another thread is already making one. Wait.
      std::this_thread::yield();
    }
  }
}

std::mutex SingletonContext::creation_mutex_;
std::weak_ptr<zmq::context_t> SingletonContext::context_wp_;

} // namespace common
} // namespace pnd
