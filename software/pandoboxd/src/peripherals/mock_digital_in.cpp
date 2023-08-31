#include "mock_digital_in.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void MockDigitalIn::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("MockDigitalIn::Configure()");
};

void MockDigitalIn::Run() {
  static constexpr char digital_in_topic[] = "pandoboxd.digital_in";
  // helpful conversion factors
  int64_t ns_per_sec = 1000000000;
  int64_t ns_per_msec = 1000000;
  // state for fake proto fields
  int seq_num = 0;
  int64_t timestamp_ns = 0;
  while (!run_stop_signal_.ShouldStop()) {
    proto::Packet packet;

    auto& header = (*packet.mutable_header());
    header.set_experiment_id(experiment_id_);
    header.set_sequence_number(seq_num++);
    header.set_timestamp(timestamp_ns);

    auto& data_pb = (*packet.mutable_payload()->mutable_digital_in());

    std::chrono::milliseconds sleep_time;
    if (timestamp_ns % ns_per_sec == 0) {
      data_pb.set_device_id(0);
      data_pb.set_edge(proto::DigitalEdgeType::RISING_EDGE);
      timestamp_ns += 10 * ns_per_msec;
      sleep_time = std::chrono::milliseconds(10);
    } else {
      data_pb.set_device_id(0);
      data_pb.set_edge(proto::DigitalEdgeType::FALLING_EDGE);
      timestamp_ns += 990 * ns_per_msec;
      sleep_time = std::chrono::milliseconds(990);
    }

    Publish(digital_in_topic, packet);
    std::this_thread::sleep_for(sleep_time);
  }
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
