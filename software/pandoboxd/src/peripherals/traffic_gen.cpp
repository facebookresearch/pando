#include "traffic_gen.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void TrafficGen::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("TrafficGen::Configure()");
}

void TrafficGen::Run() {
  proto::Packet packet;
  auto& header = (*packet.mutable_header());
  auto& data_pb = (*packet.mutable_payload()->mutable_traffic_gen());
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

    Publish("pandoboxd.traffic_gen", packet);
  }
}

void TrafficGen::Enable() {
  // Set sample rate to 10 sample/second
  pando_box_->SetTrgen0SampRateDiv(10000000);
  pando_box_->SetTrgen0Enabled(true);
}

void TrafficGen::Disable() {
  pando_box_->SetTrgen0Enabled(false);
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
