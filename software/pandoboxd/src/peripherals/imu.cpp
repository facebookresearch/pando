#include "imu.h"

#include <chrono>
#include "reporter.h"

namespace pnd {
namespace pandoboxd {
namespace peripherals {

void Imu::Configure(const PandoboxdInterface::Config& config) {
  g_reporter->info("Imu::Configure()");
}

void Imu::Run() {
  proto::Packet packet;
  auto& header = (*packet.mutable_header());
  auto& data_pb = (*packet.mutable_payload()->mutable_imu());
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
    data_pb.set_gyro_x(sample->data.imu.gyro_x);
    data_pb.set_gyro_y(sample->data.imu.gyro_y);
    data_pb.set_gyro_z(sample->data.imu.gyro_z);
    data_pb.set_accel_x(sample->data.imu.accel_x);
    data_pb.set_accel_y(sample->data.imu.accel_y);
    data_pb.set_accel_z(sample->data.imu.accel_z);
    data_pb.set_fsync(sample->data.imu.fsync);

    Publish("pandoboxd.imu", packet);
  }
}

void Imu::Enable() {
  pando_box_->SetImu0Enabled(true);
}

void Imu::Disable() {
  pando_box_->SetImu0Enabled(false);
}

} // namespace peripherals
} // namespace pandoboxd
} // namespace pnd
