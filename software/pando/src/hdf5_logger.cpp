#include "hdf5_logger.h"
#include "config_h5_transcoder.h"

#include "reporter.h"

#include <google/protobuf/reflection.h>

#include <vector>

namespace pnd {
namespace pando {

HDF5Logger::HDF5Logger(const char* analyzed_file_name, const char* peripheral_file_name) {
  if (analyzed_file_name) {
    log_analyzed_ = true;
    g_reporter->info(
        "HDF5Logger::HDF5Logger: Opened {} as analyzed data file.", analyzed_file_name);
    analyzed_file_ = H5::H5File(analyzed_file_name, H5F_ACC_EXCL);
    assert(analyzed_file_.getId());
  }
  if (peripheral_file_name) {
    log_peripheral_ = true;
    g_reporter->info(
        "HDF5Logger::HDF5Logger: Opened {} as peripheral data file.", peripheral_file_name);
    peripheral_file_ = H5::H5File(peripheral_file_name, H5F_ACC_EXCL);
    assert(peripheral_file_.getId());
  }
}

void HDF5Logger::LogPacket(const proto::Packet& packet) {
  // TODO inspect header

  // Hand the packet payload to the appropriate dataset
  auto& header = packet.header();
  auto& payload = packet.payload();

  if (log_analyzed_) {
    switch (payload.data_case()) {
      case proto::Packet::Payload::kCounts:
        count_rate_logger_.LogChannels(header, payload.counts());
        return;
      case proto::Packet::Payload::kG2:
        g2_logger_.LogChannels(header, payload.g2());
        return;
      case proto::Packet::Payload::kG2I:
        g2i_logger_.LogChannels(header, payload.g2i());
        return;
      case proto::Packet::Payload::kEvent:
        event_logger_.LogChannelLess(header, payload.event());
        return;
      case proto::Packet::Payload::kDtof:
        dtof_logger_.LogChannels(header, payload.dtof());
        return;
      case proto::Packet::Payload::kCri:
        cri_logger_.LogChannels(header, payload.cri());
        return;
      case proto::Packet::Payload::kPpsStats:
        pps_stats_logger_.LogChannelLess(header, payload.pps_stats());
        return;
      case proto::Packet::Payload::kPower:
        power_logger_analyzed_.LogChannels(header, payload.power());
        return;
    }
  }

  if (log_peripheral_) {
    switch (payload.data_case()) {
      case proto::Packet::Payload::kHrm:
        hrm_logger_.LogChannelLess(header, payload.hrm());
        return;
      case proto::Packet::Payload::kPulseOx:
        pulse_ox_logger_.LogChannelLess(header, payload.pulse_ox());
        return;
      case proto::Packet::Payload::kRespBelt:
        resp_belt_logger_.LogChannelLess(header, payload.resp_belt());
        return;
      case proto::Packet::Payload::kImu:
        imu_logger_.LogChannelLess(header, payload.imu());
        return;
      case proto::Packet::Payload::kDigitalIn:
        digital_in_logger_.LogChannelLess(header, payload.digital_in());
        return;
      case proto::Packet::Payload::kAnalogPowerMeter:
        analog_power_meter_logger_.LogChannelLess(header, payload.analog_power_meter());
        return;
    }
  }

  g_reporter->warn("HDF5Logger::LogPacket: Unknown payload {}", payload.data_case());
}

void HDF5Logger::LogConfig() {
  if (log_analyzed_) {
    ConfigH5Transcoder::Write(analyzed_file_, SingletonPando::Get()->GetConfig());
  }
}

} // namespace pando
} // namespace pnd
