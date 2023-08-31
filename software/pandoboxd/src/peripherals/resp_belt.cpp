#include "resp_belt.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void RespBelt::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("RespBelt::Configure()");
}

void RespBelt::Run() {
  proto::Packet packet;
  auto& header = (*packet.mutable_header());
  auto& data_pb = (*packet.mutable_payload()->mutable_resp_belt());
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
    Publish("pandoboxd.resp_belt", packet);
  }
}

void RespBelt::Enable() {
  // 10hz
  pando_box_->SetAin2SampRateDiv(10000000);
  pando_box_->SetAin2Enabled(true);
}

void RespBelt::Disable() {
  pando_box_->SetAin2Enabled(false);
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
