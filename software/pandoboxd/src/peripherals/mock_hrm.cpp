#include "mock_hrm.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void MockHRM::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("MockHRM::Configure()");
};

void MockHRM::Run() {
  static constexpr char hrm_topic[] = "pandoboxd.hrm";
  int64_t ns_per_msec = 1000000;

  int seq_num = 0;
  int64_t timestamp_ns = 0;
  while (!run_stop_signal_.ShouldStop()) {
    proto::Packet packet;

    auto& header = (*packet.mutable_header());
    header.set_experiment_id(experiment_id_);
    header.set_sequence_number(seq_num);
    header.set_timestamp(timestamp_ns);

    auto& data_pb = (*packet.mutable_payload()->mutable_hrm());
    data_pb.set_device_id(0);
    data_pb.set_sample(seq_num);
    seq_num++;

    Publish(hrm_topic, packet);
    timestamp_ns += 10 * ns_per_msec; // HRM sample rate is 100hz
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
