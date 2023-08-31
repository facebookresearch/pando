#pragma once

#include "pandoboxd_rpc.pb.h"

#include "pando.h"
#include "session_manager.h"
#include "singleton_context.h"
#include "thread_container.h"

#include <mutex>
#include <stdexcept>
#include <string>

namespace pnd {
namespace pando {

class PandoboxdClient : public common::SessionManager<int32_t, PandoInterface::Config> {
 public:
  static constexpr int kPandoboxdRpcPort = 5063;
  static constexpr int kPandoboxdPubPort = 5064;

  struct TimeoutException : std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  PandoboxdClient(const std::string& pandoboxd_ip_addr);
  ~PandoboxdClient();

 protected:
  virtual void StartImpl(int32_t experiment_id, PandoInterface::Config config) final;
  virtual void StopImpl() final;

 private:
  /* Listens for incoming messages reported from Pandoboxd. Logs them through g_reporter to the
   * appropriate level. */
  void Monitor();

  void DoTransaction(proto::PandoboxdTransaction& trx_pb);

  std::shared_ptr<zmq::context_t> context_{common::SingletonContext::Get()};
  zmq::socket_t cli_socket_{*context_, zmq::socket_type::req};
  std::mutex cli_socket_m_;

  common::StopSignal monitor_stop_signal_;
  common::ThreadContainer monitor_thread_;
};

} // namespace pando
} // namespace pnd
