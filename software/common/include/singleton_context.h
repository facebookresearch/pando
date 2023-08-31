#pragma once

#include <memory>
#include <mutex>

#include <zmq.hpp>

namespace pnd {
namespace common {

/** Helper class for managing a global zmq::context_t. */
class SingletonContext {
 public:
  /** Get a shared_ptr to the global zmq::context_t, creating one if it doesn't exist. */
  static std::shared_ptr<zmq::context_t> Get();

 private:
  static std::mutex creation_mutex_;
  static std::weak_ptr<zmq::context_t> context_wp_;
};

} // namespace common
} // namespace pnd
