#pragma once

#include <memory>
#include <vector>

#include <zmq.hpp>

#include "pandoboxd_interface.h"
#include "peripherals/peripheral_base.h"
#include "report_accumulator.h"
#include "thread_container.h"
#include "zmq_pubsub_proxy.h"

namespace pnd {
namespace pandoboxd {
enum class AdcChannels {
  kPulseOx = 0,
  kHeartRate,
  kRespBelt,
  kPowerMeter,
};

class Pandoboxd : public PandoboxdInterface {
 public:
  Pandoboxd();
  ~Pandoboxd();

 private:
  ErrorNum Ping() final;
  ErrorNum Start(int32_t experiment_id, const Config& config) final;
  ErrorNum Stop() final;
  ErrorNum GetReports(std::list<Report>& dest) final;
  void SamplePandoBox();

  common::ZmqPubSubProxy proxy_;

  std::unique_ptr<peripherals::PeripheralBase> digital_inputs_0_;
  std::unique_ptr<peripherals::PeripheralBase> traffic_gen_;
  std::unique_ptr<peripherals::PeripheralBase> imu_;
  std::unique_ptr<peripherals::PeripheralBase> pulse_ox_;
  std::unique_ptr<peripherals::PeripheralBase> hrm_;
  std::unique_ptr<peripherals::PeripheralBase> resp_belt_;
  std::unique_ptr<peripherals::PeripheralBase> power_meter_;
  std::unique_ptr<peripherals::PeripheralBase> camera_frame_trigger_;
  std::vector<common::SessionHandle> peripheral_session_handles_;

  std::shared_ptr<libpandobox::PandoBoxInterface> pando_box_;

  static constexpr char kReportAccumulatorInprocEp[] = "inproc://report_accumulator";
  static constexpr char kReportTopicFormatString[] = "pandoboxd.report.%l";
  static constexpr char kReportTopicBase[] = "pandoboxd.report";

  ReportAccumulator report_accumulator_{kReportAccumulatorInprocEp, kReportTopicBase};

  bool running_ = false;

  common::ThreadContainer sample_thread_;
  common::StopSignal stop_signal_;
};

} // namespace pandoboxd
} // namespace pnd
