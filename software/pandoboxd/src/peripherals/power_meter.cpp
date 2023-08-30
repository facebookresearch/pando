#include "power_meter.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void PowerMeter::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("PowerMeter::Configure()");
}

void PowerMeter::Run() {
  proto::Packet packet;
  auto& header = (*packet.mutable_header());
  auto& data_pb = (*packet.mutable_payload()->mutable_analog_power_meter());
  header.set_experiment_id(experiment_id_);

  while (true) {
    // Wait for up to 1ms;
    auto sample = Dequeue(1);
    if (!sample) {
      if (run_stop_signal_.ShouldStop())
        break;
      else
        continue;
    }
    header.set_sequence_number(seq_num_++);
    header.set_timestamp(sample->timestamp * libpandobox::sample_format::kSampleTimePeriodNs);

    data_pb.set_device_id(sample->device_id);
    data_pb.set_sample(sample->data.analog.value);
    Publish("pandoboxd.power_meter", packet);
  }
}

void PowerMeter::Enable() {
  // 1kHz
  pando_box_->SetAin3SampRateDiv(100000);
  pando_box_->SetAin3Enabled(true);
}

void PowerMeter::Disable() {
  pando_box_->SetAin3Enabled(false);
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
