#pragma once
#include "pando.pb.h"

#include <zmq.hpp>

#include <cstddef>
#include <cstdint>

namespace pnd {
namespace pando {

/** Encapulates the task of publishing data over a ZMQ socket. */
class Publisher {
 public:
  Publisher();
  void PublishZmqMessage(const char* topic, size_t topic_size, zmq::message_t& payload_msg);
  void SendZmqEvent(
      proto::EventType event_type,
      int32_t experiment_id,
      int64_t event_seq,
      std::chrono::nanoseconds timestamp);

 private:
  std::shared_ptr<zmq::context_t> context_;
  zmq::socket_t pub_socket_;
};

} // namespace pando
} // namespace pnd