#include "singleton_context.h"
#include "thread_container.h"

#include <zmq.hpp>

namespace pnd {
namespace common {

/** A class that manages a ZMQ proxy between an XSUB socket and an XPUB socket.
 * This is useful for aggregating multiple PUB sockets into one.
 */
class ZmqPubSubProxy {
 public:
  ZmqPubSubProxy();

  /** Start the proxy thread. */
  void Start();

  /** Stop the proxy thread (but don't unbind or disconnect sockets). */
  void Stop();

  /** Bind the ingress socket to an additional endpoint.
   * The proxy thread must be stopped before this function is called.
   */
  void BindIngress(const char* endpoint);

  /** Bind the egress socket to an additional endpoint.
   * The proxy thread must be stopped before this function is called.
   */
  void BindEgress(const char* endpoint);

  /** Bind the egress socket an additional endpoint at the first available port in the given range.
   * The proxy thread must be stopped before this function is called.
   */
  int BindEgress(int startport, int endport);

  /** Connect the ingress socket to an additional endpoint.
   * The proxy thread must be stopped before this function is called.
   */
  void ConnectIngress(const char* endpoint);

  /** Connect the egress socket to an additional endpoint.
   * The proxy thread must be stopped before this function is called.
   */
  void ConnectEgress(const char* endpoint);

  /** Disconnect the ingress socket from an endpoint it was previously connected to.
   * The proxy thread must be stopped before this function is called.
   */
  void DisconnectIngress(const char* endpoint);

  /** Disconnect the egress socket from an endpoint it was previously connected to.
   * The proxy thread must be stopped before this function is called.
   */
  void DisconnectEgress(const char* endpoint);

 private:
  void Run();

  std::shared_ptr<zmq::context_t> context_{common::SingletonContext::Get()};
  zmq::socket_t control_tx_socket_{*context_, zmq::socket_type::pair};
  zmq::socket_t control_rx_socket_{*context_, zmq::socket_type::pair};
  zmq::socket_t egress_socket_{*context_, zmq::socket_type::xpub};
  zmq::socket_t ingress_socket_{*context_, zmq::socket_type::xsub};

  bool running_ = false;

  StopSignal run_stop_signal_;
  ThreadContainer run_thread_;
};
} // namespace common
} // namespace pnd
