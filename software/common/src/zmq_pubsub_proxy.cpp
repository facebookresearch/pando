#include "zmq_pubsub_proxy.h"

#include "reporter.h"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fmt/ostream.h>

#include <cassert>
#include <cerrno>
#include <string>

namespace pnd {
namespace common {

namespace {
/** Helper function to generate a random & unique inproc endpoint.
 * @return a string such as "inproc://711c8b5c-5dc2-4c3a-8f45-73739fcfe932"
 */
std::string RandomInprocEp() {
  boost::uuids::random_generator uuid_gen;
  boost::uuids::uuid uuid = uuid_gen();

  return fmt::format("inproc://{}", uuid);
}
} // namespace

ZmqPubSubProxy::ZmqPubSubProxy() {
  // Set up both ends of the proxy control interface
  auto control_ep = RandomInprocEp();
  // g_reporter->debug("ZmqPubSubProxy: Control EP is {}", control_ep);
  control_tx_socket_.bind(control_ep);
  control_rx_socket_.connect(control_ep);
}

void ZmqPubSubProxy::Start() {
  assert(!running_);
  run_thread_ = ThreadContainer([&] { Run(); }, "ZmqPubSubProxy::Run", &run_stop_signal_);
  running_ = true;
}

void ZmqPubSubProxy::Stop() {
  assert(running_);

  // Note that zmq_proxy_steerable accepts a "PAUSE" command, but that's not what we're doing here.
  // We need to actually join the proxy thread in order to safely modify the sockets it was using.
  run_thread_.Stop();
  run_thread_.Join();
  running_ = false;
}

void ZmqPubSubProxy::BindIngress(const char* endpoint) {
  assert(!running_);
  g_reporter->debug("ZmqPubSubProxy: Ingress socket binding to {}", endpoint);
  ingress_socket_.bind(endpoint);
}

void ZmqPubSubProxy::BindEgress(const char* endpoint) {
  assert(!running_);
  g_reporter->debug("ZmqPubSubProxy: Egress socket binding to {}", endpoint);
  egress_socket_.bind(endpoint);
}

int ZmqPubSubProxy::BindEgress(int startport, int endport) {
  assert(!running_);
  std::string full_endpoint;
  for (int p = startport; p <= endport; p++) {
    try {
      full_endpoint = fmt::format("tcp://*:{}", p);
      g_reporter->debug("ZmqPubSubProxy: Egress socket binding to {}", full_endpoint.data());
      egress_socket_.bind(full_endpoint.data());
      return p;
    } catch (zmq::error_t error) {
      g_reporter->debug(
          "ZmqPubSubProxy: Egress socket binding to {} with error {}",
          full_endpoint,
          error.what(),
          error.num());
      if (error.num() != EADDRINUSE || p == endport) {
        throw error;
      }
    }
  }
}

void ZmqPubSubProxy::ConnectIngress(const char* endpoint) {
  assert(!running_);
  g_reporter->debug("ZmqPubSubProxy: Ingress socket connecting to {}", endpoint);
  ingress_socket_.connect(endpoint);
}

void ZmqPubSubProxy::ConnectEgress(const char* endpoint) {
  assert(!running_);
  g_reporter->debug("ZmqPubSubProxy: Egress socket connecting to {}", endpoint);
  egress_socket_.connect(endpoint);
}

void ZmqPubSubProxy::DisconnectIngress(const char* endpoint) {
  assert(!running_);
  g_reporter->debug("ZmqPubSubProxy: Ingress disconnecting from {}", endpoint);
  ingress_socket_.disconnect(endpoint);
}

void ZmqPubSubProxy::DisconnectEgress(const char* endpoint) {
  assert(!running_);
  g_reporter->debug("ZmqPubSubProxy: Egress socket disconnecting from {}", endpoint);
  egress_socket_.disconnect(endpoint);
}

void ZmqPubSubProxy::Run() {
  auto stopper_f = [&] {
    // Wait for the stop signal
    while (!run_stop_signal_.ShouldStop(std::chrono::hours(1))) {
      continue;
    }

    // Send TERMINATE string to end the proxy
    static constexpr char kTermCmd[] = "TERMINATE";
    control_tx_socket_.send(kTermCmd, sizeof(kTermCmd) - 1);
  };

  common::ThreadContainer stopper_t(stopper_f, "ZmqPubSubProxy::Run::stopper_t");

  zmq::proxy_steerable(egress_socket_, ingress_socket_, nullptr, control_rx_socket_);
}

} // namespace common
} // namespace pnd
