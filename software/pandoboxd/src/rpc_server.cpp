#include "rpc_server.h"

#include "pandoboxd.h"
#include "protobuf_util.h"

namespace pnd {
namespace pandoboxd {
RpcServer::RpcServer() : pandoboxd_{std::make_unique<Pandoboxd>()} {
  serve_thread_ =
      common::ThreadContainer([this] { Serve(); }, "RpcServer::Serve", &serve_stop_signal_);
}

void RpcServer::Serve() {
  auto context = common::SingletonContext::Get();
  zmq::socket_t socket{*context, zmq::socket_type::rep};

  socket.setsockopt(ZMQ_RCVTIMEO, 100);
  socket.bind(kServerEp);

  while (!serve_stop_signal_.ShouldStop()) {
    zmq::message_t request;

    if (socket.recv(&request)) {
      proto::PandoboxdTransaction trx_pb;
      common::ProtobufDeserialize(request, trx_pb);

      proto::PandoboxdTransaction::Status status;
      switch (trx_pb.type_case()) {
        case proto::PandoboxdTransaction::TypeCase::kPing:
          trx_pb.set_status(RunCommand(trx_pb.mutable_ping()));
          break;
        case proto::PandoboxdTransaction::TypeCase::kStart:
          trx_pb.set_status(RunCommand(trx_pb.mutable_start()));
          break;
        case proto::PandoboxdTransaction::TypeCase::kStop:
          trx_pb.set_status(RunCommand(trx_pb.mutable_stop()));
          break;
        case proto::PandoboxdTransaction::TypeCase::kGetReports:
          trx_pb.set_status(RunCommand(trx_pb.mutable_get_reports()));
          break;
        default:
          trx_pb.set_status(proto::PandoboxdTransaction::STATUS_ERR_UNIMPLEMENTED);
      }

      zmq::message_t response;
      common::ProtobufSerialize(trx_pb, response);
      socket.send(response);
    }
  }
}

proto::PandoboxdTransaction::Status RpcServer::RunCommand(proto::PandoboxdTransaction::Ping* cmd) {
  auto err = pandoboxd_->Ping();
  cmd->mutable_rep();
  return static_cast<proto::PandoboxdTransaction::Status>(err);
}

proto::PandoboxdTransaction::Status RpcServer::RunCommand(proto::PandoboxdTransaction::Start* cmd) {
  const auto& req = cmd->req();

  PandoboxdInterface::Config config{};
  config.camera_frame_trigger_0_period_10ns = req.camera_frame_trigger_0_period_10ns();
  config.camera_frame_trigger_1_period_10ns = req.camera_frame_trigger_1_period_10ns();

  auto err = pandoboxd_->Start(req.experiment_id(), config);
  cmd->mutable_rep();
  return static_cast<proto::PandoboxdTransaction::Status>(err);
}

proto::PandoboxdTransaction::Status RpcServer::RunCommand(proto::PandoboxdTransaction::Stop* cmd) {
  auto err = pandoboxd_->Stop();
  cmd->mutable_rep();
  return static_cast<proto::PandoboxdTransaction::Status>(err);
}

proto::PandoboxdTransaction::Status RpcServer::RunCommand(
    proto::PandoboxdTransaction::GetReports* cmd) {
  PandoboxdInterface::Reports reports;
  auto err = pandoboxd_->GetReports(reports);

  cmd->mutable_rep()->mutable_reports()->Reserve(reports.size());
  for (auto& report : reports) {
    auto report_pb = cmd->mutable_rep()->mutable_reports()->Add();
    report_pb->set_severity(std::move(report.severity));
    report_pb->set_message(std::move(report.message));
  }

  return static_cast<proto::PandoboxdTransaction::Status>(err);
}

} // namespace pandoboxd
} // namespace pnd
