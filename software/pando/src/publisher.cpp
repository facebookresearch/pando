#include "publisher.h"

#include "pando.h"
#include "protobuf_util.h"
#include "singleton_context.h"

namespace pnd {
namespace pando {

Publisher::Publisher()
    : context_{common::SingletonContext::Get()}, pub_socket_{*context_, zmq::socket_type::pub} {
  {
    // For inproc transport, the effective send hwm is ZMQ_SNDHWM plus the ZMQ_RCVHWM of the socket
    // on the other end of the connection (in this case, the ingress port of the proxy).
    pub_socket_.setsockopt(ZMQ_SNDHWM, 1);
    pub_socket_.setsockopt(ZMQ_SNDTIMEO, 0); // Do not block on send
    pub_socket_.setsockopt(ZMQ_XPUB_NODROP, 1); // Do not silently drop messages
    pub_socket_.connect(PandoInterface::kProxyIngressInprocEp);
  }
}

void Publisher::PublishZmqMessage(
    const char* topic,
    size_t topic_size,
    zmq::message_t& payload_msg) {
  bool ret = pub_socket_.send(topic, topic_size, ZMQ_SNDMORE);
  ret &= pub_socket_.send(payload_msg);
  if (!ret) {
    throw std::overflow_error("Publisher::PublishZmqMessage: ZMQ send operation failed");
  }
}

void Publisher::SendZmqEvent(
    proto::EventType event_type,
    int32_t experiment_id,
    int64_t event_seq,
    std::chrono::nanoseconds timestamp) {
  proto::Packet event_pb;
  zmq::message_t payload_msg;

  // Create header for protobuf'd data
  proto::Packet::Header header;
  header.set_experiment_id(experiment_id);
  header.set_sequence_number(event_seq);
  header.set_timestamp(timestamp.count());

  *event_pb.mutable_header() = header;
  auto& event_data_pb = (*event_pb.mutable_payload()->mutable_event());
  event_data_pb.set_event(event_type);

  common::ProtobufSerialize(event_pb, payload_msg);

  static constexpr char ev_topic[] = "pando.event";
  PublishZmqMessage(ev_topic, sizeof(ev_topic), payload_msg);
}

} // namespace pando
} // namespace pnd