#include "digital_in.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void DigitalIn::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("DigitalIn::Configure()");
}

void DigitalIn::Run() {
  proto::Packet packet;
  auto& header = (*packet.mutable_header());
  auto& data_pb = (*packet.mutable_payload()->mutable_digital_in());

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
    const auto edge_type = sample->data.digital.direction ? proto::DigitalEdgeType::RISING_EDGE
                                                          : proto::DigitalEdgeType::FALLING_EDGE;
    data_pb.set_edge(edge_type);
    Publish("pandoboxd.digital_in", packet);
  }
}

void DigitalIn::Enable() {
  pando_box_->SetDin0Enabled(true);
}

void DigitalIn::Disable() {
  pando_box_->SetDin0Enabled(false);
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
