#include "peripheral_base.h"

#include <stdexcept>

#include "pandoboxd_interface.h"
#include "protobuf_util.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void PeripheralBase::Enqueue(const libpandobox::sample_format::PandoBox* sample) {
  *next_sample_ = *sample;

  auto err_reported = false;
  while (!run_stop_signal_.ShouldStop()) {
    next_sample_ = buffer_.AdvanceWrite(1);
    if (next_sample_) {
      return;
    }
    if (!err_reported) {
      g_reporter->error("PeripheralBase: Buffer overflow detected!");
      err_reported = true;
    }
  }
}

const libpandobox::sample_format::PandoBox* PeripheralBase::Dequeue(int timeout) {
  return buffer_.AdvanceRead(timeout);
}

void PeripheralBase::StartImpl(int32_t experiment_id) {
  experiment_id_ = experiment_id;
  seq_num_ = 0;
  buffer_.Reset();
  Enable();
  run_thread_ =
      common::ThreadContainer([this] { Run(); }, "PeripheralBase::Run", &run_stop_signal_);
}

void PeripheralBase::StopImpl() {
  Disable();
  run_thread_.Stop();
  run_thread_.Join();
}

PeripheralBase::PeripheralBase(std::shared_ptr<libpandobox::PandoBoxInterface> pando_box) {
  pando_box_ = pando_box;

  // We need to queue up the first packet that we'll encode into
  next_sample_ = buffer_.AdvanceWrite(-1);
  pub_socket_.connect(PandoboxdInterface::kProxyIngressInprocEp);
}

void PeripheralBase::Publish(
    const std::string& topic,
    const google::protobuf::MessageLite& payload) {
  zmq::message_t serialized_payload;
  common::ProtobufSerialize(payload, serialized_payload);

  bool ret = pub_socket_.send(topic.c_str(), topic.length() + 1, ZMQ_SNDMORE);
  ret &= pub_socket_.send(serialized_payload);
  if (!ret) {
    throw std::overflow_error("PeripheralBase::Publish: ZMQ send operation failed");
  }
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
