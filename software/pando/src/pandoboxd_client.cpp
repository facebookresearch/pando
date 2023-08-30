#include "pandoboxd_client.h"

#include "pando.h"
#include "protobuf_util.h"
#include "reporter.h"

#include <fmt/format.h>

namespace pnd {
namespace pando {

PandoboxdClient::PandoboxdClient(const std::string& pandoboxd_ip_addr) {
  g_reporter->info("PandoboxdClient: Connecting to Pandoboxd RPC interface @{}", pandoboxd_ip_addr);

  // set up socket
  auto pandoboxd_rpc_ep = fmt::format("tcp://{}:{}", pandoboxd_ip_addr, kPandoboxdRpcPort);
  cli_socket_.setsockopt(ZMQ_RCVTIMEO, 1000);
  cli_socket_.setsockopt(ZMQ_LINGER, 0);
  cli_socket_.connect(pandoboxd_rpc_ep);

  // Confirm that we have successfully connected to Pandoboxd
  proto::PandoboxdTransaction trx_pb;
  trx_pb.mutable_ping()->mutable_req();
  try {
    DoTransaction(trx_pb);
  } catch (const TimeoutException&) {
    throw std::runtime_error("PandoboxdClient: Pandoboxd did not respond to ping. Is it running?");
  }

  if (trx_pb.status() != proto::PandoboxdTransaction::STATUS_OK) {
    auto msg =
        fmt::format("PandoboxdClient: Ping transaction failed (status = {})", trx_pb.status());
    throw std::runtime_error(msg);
  }

  // Issue a gratuitous Stop command, just in case Pandoboxd is somehow already running (Could
  // happen if Pando crashes hard, or if other RPC clients are involved.)
  trx_pb.mutable_stop()->mutable_req();
  DoTransaction(trx_pb);
  if (trx_pb.status() == proto::PandoboxdTransaction::STATUS_ERR_ALREADY_STOPPED) {
    // Do nothing.
    // We got this status code because the Stop command was indeed gratuitous.
  } else if (trx_pb.status() == proto::PandoboxdTransaction::STATUS_OK) {
    // Either the stop command actually did something or the Pandoboxd version doesn't produce
    // STATUS_ERR_ALREADY_STOPPED statuses.
    g_reporter->warn(
        "PandoboxdClient: Either Pandoboxd was in the middle of an experiment when we connected to it (in which case that experiment has just been stopped), or this is an old version of Pandoboxd.");
  } else {
    auto msg =
        fmt::format("PandoboxdClient: Stop transaction failed (status = {})", trx_pb.status());
    throw std::runtime_error(msg);
  }

  // Start monitor thread
  monitor_thread_ = common::ThreadContainer(
      [&] { Monitor(); }, "PandoboxdClient::Monitor", &monitor_stop_signal_);
}

PandoboxdClient::~PandoboxdClient() {
  g_reporter->info("PandoboxdClient: Disconnecting from Pandoboxd RPC interface");
}

void PandoboxdClient::StartImpl(int32_t experiment_id, PandoInterface::Config config) {
  proto::PandoboxdTransaction trx_pb;

  auto& req = *trx_pb.mutable_start()->mutable_req();

  req.set_experiment_id(experiment_id);
  req.set_camera_frame_trigger_0_period_10ns(config.camera_frame_trigger_period_10ns);
  req.set_camera_frame_trigger_1_period_10ns(config.aux_frame_trigger_period_10ns);

  DoTransaction(trx_pb);
  if (trx_pb.status() != proto::PandoboxdTransaction::STATUS_OK) {
    auto msg =
        fmt::format("PandoboxdClient: Start transaction failed (status = {})", trx_pb.status());
    throw std::runtime_error(msg);
  }
}

void PandoboxdClient::StopImpl() {
  proto::PandoboxdTransaction trx_pb;

  trx_pb.mutable_stop()->mutable_req();
  DoTransaction(trx_pb);
  if (trx_pb.status() == proto::PandoboxdTransaction::STATUS_ERR_ALREADY_STOPPED) {
    g_reporter->warn("PandoboxdClient::StopImpl: Pandoboxd was already stopped");
  } else if (trx_pb.status() != proto::PandoboxdTransaction::STATUS_OK) {
    auto msg =
        fmt::format("PandoboxdClient: Stop transaction failed (status = {})", trx_pb.status());
    throw std::runtime_error(msg);
  }
}

void PandoboxdClient::Monitor() {
  proto::PandoboxdTransaction trx_pb;

  zmq::message_t req_msg, rep_msg;
  while (!monitor_stop_signal_.ShouldStop()) {
    trx_pb.mutable_get_reports()->mutable_req();
    DoTransaction(trx_pb);
    if (trx_pb.status() != proto::PandoboxdTransaction::STATUS_OK) {
      auto msg = fmt::format(
          "PandoboxdClient: GetReports transaction failed (status = {})", trx_pb.status());
      throw std::runtime_error(msg);
    }

    if (trx_pb.get_reports().direction_case() !=
        proto::PandoboxdTransaction::GetReports::DirectionCase::kRep) {
      throw std::runtime_error("PandoboxdClient: wrong field set in the response.");
    }

    // process reports
    for (const auto& report : trx_pb.get_reports().rep().reports()) {
      auto level = spdlog::level::from_str(report.severity());
      g_reporter->log(level, report.message());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void PandoboxdClient::DoTransaction(proto::PandoboxdTransaction& trx_pb) {
  auto request_type = trx_pb.type_case();

  // Serialize outgoing transaction
  zmq::message_t req_msg;
  common::ProtobufSerialize(trx_pb, req_msg);

  // Send request, receive reply
  bool recv_ok;
  zmq::message_t rep_msg;
  {
    std::lock_guard<std::mutex> l{cli_socket_m_};

    cli_socket_.send(req_msg);
    recv_ok = cli_socket_.recv(&rep_msg);
  }

  if (!recv_ok) {
    auto msg = fmt::format("PandoboxdClient: Transaction type {} timed out!", request_type);
    throw TimeoutException(msg);
  }

  // Deserialize incoming transaction
  common::ProtobufDeserialize(rep_msg, trx_pb);

  // Check that the correct oneof field is set in the response
  auto response_type = trx_pb.type_case();
  if (response_type != request_type) {
    auto msg = fmt::format(
        "PandoboxdClient: Got response type {} for request type {} ", response_type, request_type);
    throw std::runtime_error(msg);
  }
}

} // namespace pando
} // namespace pnd
