#pragma once

#include "pando.pb.h"
#include "session_manager.h"
#include "thread_container.h"

#include "pando_box_interface.h"
#include "reporter.h"

#include <google/protobuf/message_lite.h>
#include <zmq.hpp>

#include "locked_ring_buffer.h"
#include "pandoboxd_interface.h"
#include "session_manager.h"
#include "singleton_context.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

/** Base class for all peripherals. */
class PeripheralBase : public common::SessionManager<int32_t> {
 public:
  PeripheralBase(std::shared_ptr<libpandobox::PandoBoxInterface>);
  virtual ~PeripheralBase() = default;

  virtual void Configure(const PandoboxdInterface::Config& config) = 0;
  void Enqueue(const libpandobox::sample_format::PandoBox*);
  static constexpr int kQueueSize = 128;

 protected:
  /** Serialize a protobuf message and publish it on Pandoboxd's XPUB socket. */
  void Publish(const std::string& topic, const google::protobuf::MessageLite& payload);
  const libpandobox::sample_format::PandoBox* Dequeue(int timeout);

  virtual void Run() = 0;
  virtual void Enable() = 0;
  virtual void Disable() = 0;

  int32_t experiment_id_;
  int64_t seq_num_;

  common::StopSignal run_stop_signal_;
  common::ThreadContainer run_thread_;

  std::shared_ptr<libpandobox::PandoBoxInterface> pando_box_;

 private:
  void StartImpl(int32_t experiment_id) final;
  void StopImpl() final;

  std::shared_ptr<zmq::context_t> context_{common::SingletonContext::Get()};
  zmq::socket_t pub_socket_{*context_, zmq::socket_type::pub};
  LockedRingBuffer<libpandobox::sample_format::PandoBox, kQueueSize> buffer_;
  libpandobox::sample_format::PandoBox* next_sample_;
};

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
