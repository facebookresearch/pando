#pragma once

#include <memory>

#include <zmq.hpp>

#include "pandoboxd_rpc.pb.h"

#include "pandoboxd_interface.h"
#include "singleton_context.h"
#include "thread_container.h"

namespace pnd {
namespace pandoboxd {

class RpcServer {
 public:
  static constexpr char kServerEp[] = "tcp://*:5063";

  RpcServer();

 private:
  void Serve();

  proto::PandoboxdTransaction::Status RunCommand(proto::PandoboxdTransaction::Ping* cmd);
  proto::PandoboxdTransaction::Status RunCommand(proto::PandoboxdTransaction::Start* cmd);
  proto::PandoboxdTransaction::Status RunCommand(proto::PandoboxdTransaction::Stop* cmd);
  proto::PandoboxdTransaction::Status RunCommand(proto::PandoboxdTransaction::GetReports* cmd);

  std::unique_ptr<PandoboxdInterface> pandoboxd_;

  common::StopSignal serve_stop_signal_;
  common::ThreadContainer serve_thread_;
};

} // namespace pandoboxd
} // namespace pnd
