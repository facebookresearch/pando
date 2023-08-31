#include "mock_imu.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void MockImu::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("MockImu::Configure()");
};

void MockImu::Run() {
  static constexpr char imu_topic[] = "pandoboxd.imu";
  int64_t ns_per_msec = 1000000;

  int seq_num = 0;
  int64_t timestamp_ns = 0;
  while (!run_stop_signal_.ShouldStop()) {
    proto::Packet packet;

    auto& header = (*packet.mutable_header());
    header.set_experiment_id(experiment_id_);
    header.set_sequence_number(seq_num);
    header.set_timestamp(timestamp_ns);

    auto& data_pb = (*packet.mutable_payload()->mutable_imu());
    data_pb.set_device_id(0);
    data_pb.set_gyro_x(seq_num);
    data_pb.set_gyro_y(seq_num + 1);
    data_pb.set_gyro_z(seq_num + 2);
    data_pb.set_accel_x(seq_num + 3);
    data_pb.set_accel_y(seq_num + 4);
    data_pb.set_accel_z(seq_num + 5);
    seq_num++;

    Publish(imu_topic, packet);

    timestamp_ns += 1 * ns_per_msec; // IMU sample rate is 1khz
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
