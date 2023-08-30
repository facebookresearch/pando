#pragma once

#include "pando.h"

#include "device_interface.h"
#include "hdf5_logger.h"
#include "pandoboxd_client.h"
#include "thread_container.h"
#include "thread_monitor.h"
#include "zmq_pubsub_proxy.h"

#include <zmq.hpp>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace pnd {
namespace pando {

class Pando : public PandoInterface {
  friend class SingletonPando;

 public:
  ~Pando() override;

  void Configure(const Config& config) final;
  Config GetConfig() final;
  int GetProxyEgressPort() final {
    return proxy_tcp_egress_port_;
  }
  bool IsHealthy() final;

  /** Start an experiment.
   * 1. Create HDF5 files
   * 2. Start logging protobuf messages to those files
   * 3. Start the photon detector device
   * 4. Start the remote Pandoboxd instance
   */
  void StartImpl() final;

  /** Stop an experiment.
   * 1. Stop the device
   * 2. Stop Pandoboxd
   * 3. Stop the logger (blocks until all in-flight messages are consumed)
   * 4. Close HDF5 files
   */
  void StopImpl() final;

 private:
  static constexpr const char* kAnalyzedTopicsToLog[] = {"pando.counts",
                                                         "pando.g2",
                                                         "pando.g2i",
                                                         "pando.event",
                                                         "pando.dtof",
                                                         "pando.cri",
                                                         "pando.power_meter",
                                                         "pando.pps_stats"};
  static constexpr const char* kPandoboxdTopicsToLog[] = {"pandoboxd.hrm",
                                                          "pandoboxd.pulse_ox",
                                                          "pandoboxd.resp_belt",
                                                          "pandoboxd.imu",
                                                          "pandoboxd.digital_in",
                                                          "pandoboxd.power_meter"};

  static constexpr char kReportTopicFormatString[] = "pando.report.%l";

  common::ZmqPubSubProxy proxy_;
  std::string pandoboxd_pub_ep_;

  std::unique_ptr<DeviceInterface> device_;
  common::SessionHandle device_session_handle_;

  std::unique_ptr<PandoboxdClient> pandoboxd_client_;
  common::SessionHandle pandoboxd_session_handle_;

  std::unique_ptr<HDF5Logger> hdf5_logger_;
  common::StopSignal logger_stop_signal_;
  common::ThreadContainer logger_thread_;

  std::shared_ptr<common::ThreadMonitor> experiment_monitor_;

  Config config_;
  std::mutex config_lock_;

  int32_t next_experiment_ = 0;

  Pando();

  std::mutex start_lock_;
  std::condition_variable start_cv_;
  bool logger_running_ = false;
  int proxy_tcp_egress_port_ = 0;
  void Log();
};

} // namespace pando
} // namespace pnd
