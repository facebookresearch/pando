#include "pandoboxd.h"
#include "peripherals/camera_frame_trigger.h"
#include "peripherals/digital_in.h"
#include "peripherals/hrm.h"
#include "peripherals/imu.h"
#include "peripherals/power_meter.h"
#include "peripherals/pulse_ox.h"
#include "peripherals/resp_belt.h"
#include "peripherals/traffic_gen.h"

#include "log_exception.h"
#include "pando_box_factory.h"
#include "reporter.h"
#include "singleton_context.h"
#include "version.h"

namespace pnd {
namespace pandoboxd {

Pandoboxd::Pandoboxd() : pando_box_{libpandobox::PandoBoxFactory::Create(false)} {
  // Report the current version of pandoboxd & the version number baked into the FPGA
  g_reporter->info(
      "Running Pandoboxd Version: {}, FPGA build: {}",
      CLEMENTINE_VERSION_STRING,
      pando_box_->GetVersion());

  // Bind the Proxy to its static endpoints, then start it.
  proxy_.BindIngress(kProxyIngressInprocEp);
  proxy_.BindEgress(kProxyEgressTcpEp);
  proxy_.Start();

  common::Reporter::CreateZmqSink(kReportAccumulatorInprocEp, kReportTopicFormatString);

  digital_inputs_0_ = std::make_unique<peripherals::DigitalIn>(pando_box_);
  traffic_gen_ = std::make_unique<peripherals::TrafficGen>(pando_box_);
  imu_ = std::make_unique<peripherals::Imu>(pando_box_);
  pulse_ox_ = std::make_unique<peripherals::PulseOx>(pando_box_);
  hrm_ = std::make_unique<peripherals::HRM>(pando_box_);
  resp_belt_ = std::make_unique<peripherals::RespBelt>(pando_box_);
  power_meter_ = std::make_unique<peripherals::PowerMeter>(pando_box_);
  camera_frame_trigger_ = std::make_unique<peripherals::CameraFrameTrigger>(pando_box_);
}

Pandoboxd::~Pandoboxd() {
  common::Reporter::DestroyZmqSink();
}

Pandoboxd::ErrorNum Pandoboxd::Ping() {
  return ErrorNum::NONE;
}

void Pandoboxd::SamplePandoBox() {
  while (true) {
    auto samples_consumed = pando_box_->ConsumeSamples(
        [this](auto samples) {
          for (auto sample : samples) {
            switch (static_cast<libpandobox::sample_format::Magic>(sample->type)) {
              case libpandobox::sample_format::Magic::kTrafficGen:
                traffic_gen_->Enqueue(sample);
                break;
              case libpandobox::sample_format::Magic::kDigitalInput:
                digital_inputs_0_->Enqueue(sample);
                break;
              case libpandobox::sample_format::Magic::kImu:
                imu_->Enqueue(sample);
                break;
              case libpandobox::sample_format::Magic::kAnalogInput:
                switch (static_cast<AdcChannels>(sample->device_id)) {
                  case AdcChannels::kPulseOx:
                    pulse_ox_->Enqueue(sample);
                    break;
                  case AdcChannels::kHeartRate:
                    hrm_->Enqueue(sample);
                    break;
                  case AdcChannels::kRespBelt:
                    resp_belt_->Enqueue(sample);
                    break;
                  case AdcChannels::kPowerMeter:
                    power_meter_->Enqueue(sample);
                    break;
                  default:
                    g_reporter->info("Pandoboxd::Dispatch - Unknown device id");
                    break;
                }
                break;
              default:
                g_reporter->info("Pandoboxd::Dispatch - Unsupported");
                break;
            }
          }
        },
        64,
        1);

    if (stop_signal_.ShouldStop() && samples_consumed == 0) {
      break;
    }
  }
}

Pandoboxd::ErrorNum Pandoboxd::Start(int32_t experiment_id, const Config& config) try {
  g_reporter->info("Pandoboxd::Start()");

  g_reporter->debug(
      "Running Pandoboxd Version: {}, FPGA build: {}",
      CLEMENTINE_VERSION_STRING,
      pando_box_->GetVersion());

  if (running_) {
    return ErrorNum::ALREADY_STARTED;
  }

  digital_inputs_0_->Configure(config);
  traffic_gen_->Configure(config);
  imu_->Configure(config);
  pulse_ox_->Configure(config);
  hrm_->Configure(config);
  resp_belt_->Configure(config);
  power_meter_->Configure(config);
  camera_frame_trigger_->Configure(config);

  peripheral_session_handles_.push_back(digital_inputs_0_->Start(experiment_id));
  peripheral_session_handles_.push_back(traffic_gen_->Start(experiment_id));
  peripheral_session_handles_.push_back(imu_->Start(experiment_id));
  peripheral_session_handles_.push_back(pulse_ox_->Start(experiment_id));
  peripheral_session_handles_.push_back(hrm_->Start(experiment_id));
  peripheral_session_handles_.push_back(resp_belt_->Start(experiment_id));
  peripheral_session_handles_.push_back(power_meter_->Start(experiment_id));
  peripheral_session_handles_.push_back(camera_frame_trigger_->Start(experiment_id));

  pando_box_->StartDma();
  pando_box_->SetRun(true);

  sample_thread_ = common::ThreadContainer(
      [this] { SamplePandoBox(); }, "Pandoboxd::SamplePandoBox", &stop_signal_);

  running_ = true;

  return ErrorNum::NONE;

} catch (...) {
  common::LogException(false);
  Stop();
  return ErrorNum::UNSPECIFIED;
}

Pandoboxd::ErrorNum Pandoboxd::Stop() try {
  g_reporter->info("Pandoboxd::Stop()");

  if (!running_) {
    return ErrorNum::ALREADY_STOPPED;
  }

  pando_box_->SetRun(false);

  sample_thread_.Stop();
  sample_thread_.Join();

  pando_box_->StopDma();

  peripheral_session_handles_.clear();

  running_ = false;

  return ErrorNum::NONE;

} catch (...) {
  common::LogException(false);
  return ErrorNum::UNSPECIFIED;
}

Pandoboxd::ErrorNum Pandoboxd::GetReports(std::list<Report>& dest) try {
  dest = std::move(report_accumulator_.GetReports());
  return ErrorNum::NONE;

} catch (...) {
  common::LogException(false);
  return ErrorNum::UNSPECIFIED;
}

} // namespace pandoboxd
} // namespace pnd
