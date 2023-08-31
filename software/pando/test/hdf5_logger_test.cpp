#include "hdf5_logger.h"
#include "hdf5_raw_logger.h"

#include "pando.pb.h"
#include "reporter.h"

#include "gtest/gtest.h"

#include <cstdio>

namespace pnd {
namespace pando {

class HDF5LoggerTest : public ::testing::Test {
 protected:
  HDF5LoggerTest() {
    g_reporter = common::Reporter::Create("hdf5_logger_test");
  }
};

/**
 * Construct an HDF5Logger, but don't log any data.
 *
 * hdf_logger_test_Initialization_raw.h5 & hdf_logger_test_Initialization_analyzed.h5 are created.
 */
TEST_F(HDF5LoggerTest, Initialization) {
  static constexpr char raw_fname[] = "hdf_logger_test_Initialization_raw.h5";
  static constexpr char analyzed_fname[] = "hdf_logger_test_Initialization_analyzed.h5";
  static constexpr char peripheral_fname[] = "hdf_logger_test_Initialization_peripheral.h5";
  std::remove(raw_fname);
  std::remove(analyzed_fname);
  std::remove(peripheral_fname);
  HDF5Logger{analyzed_fname, peripheral_fname};
  HDF5RawLogger{raw_fname};
}

/**
 * Log some timestamps on multiple channels.
 *
 * hdf_logger_test_LogPacketTimestamps_raw.h5 should end up with datasets TimestampsChannel[0-2],
 * and each one of those datasets should have 50 rows containing sequential timestamps.
 *
 * hdf_logger_test_LogPacketTimestamps_analyzed.h5 should end up empty.
 */
TEST_F(HDF5LoggerTest, LogPacketTimestamps) {
  static constexpr char raw_fname[] = "hdf_logger_test_LogPacketTimestamps_raw.h5";
  static constexpr char analyzed_fname[] = "hdf_logger_test_LogPacketTimestamps_analyzed.h5";
  static constexpr char peripheral_fname[] = "hdf_logger_test_LogPacketTimestamps_peripheral.h5";
  std::remove(raw_fname);
  std::remove(analyzed_fname);
  std::remove(peripheral_fname);
  HDF5Logger l(analyzed_fname, peripheral_fname);
  HDF5RawLogger l_raw(raw_fname);

  for (int tint = 0; tint < 10; ++tint) {
    auto channels = std::vector<int32_t>{0, 1, 2};
    auto timestamps_container =
        KVContainer<int32_t, ChannelTimestamps>{channels.cbegin(), channels.cend()};

    for (int i = 0; i < 5; ++i) {
      for (auto& timestamps_kv : timestamps_container) {
        auto& timestamps = timestamps_kv.second;
        timestamps.macro_times().push_back(MacroTime{tint * 5 + i});
        timestamps.macro_times().push_back(MicroTime{tint * 5 + i});
      }
    }

    l_raw.LogTimestamps(timestamps_container);
  }
}

/**
 * Log some g2 curves on multiple channels.
 *
 * hdf_logger_test_LogPacketG2_raw.h5 should end up empty.
 *
 * hdf_logger_test_LogPacketG2_analyzed.h5 should end up with datasets G2 and G2_headers, and each
 * one of those datasets should have 10 rows. Dataset G2 should have 3 columns, and dataset
 * G2_headers should have 1 column.
 */
TEST_F(HDF5LoggerTest, LogPacketG2) {
  static constexpr char raw_fname[] = "hdf_logger_test_LogPacketG2_raw.h5";
  static constexpr char analyzed_fname[] = "hdf_logger_test_LogPacketG2_analyzed.h5";
  static constexpr char peripheral_fname[] = "hdf_logger_test_LogPacketG2_peripheral.h5";
  std::remove(raw_fname);
  std::remove(analyzed_fname);
  std::remove(peripheral_fname);
  HDF5Logger l(analyzed_fname, peripheral_fname);
  HDF5RawLogger l_raw(raw_fname);

  proto::Packet packet_pb;
  proto::Packet::Header* header_pb = packet_pb.mutable_header();
  proto::G2* g2_pb = packet_pb.mutable_payload()->mutable_g2();

  // Use same payload in each channel of each packet
  std::vector<int32_t> dummy_k = {0, 1, 2, 3, 4};
  std::vector<double> dummy_g2 = {2.0, 1.9, 1.1, 1.0};
  *(g2_pb->mutable_meta()->mutable_k()) = {dummy_k.cbegin(), dummy_k.cend()};
  for (int chan_idx = 0; chan_idx < 3; ++chan_idx) {
    *(*g2_pb->mutable_channels())[chan_idx].mutable_g2() = {dummy_g2.cbegin(), dummy_g2.cend()};
  }

  for (int packet_idx = 0; packet_idx < 10; ++packet_idx) {
    // Update header every packet
    header_pb->set_experiment_id(0);
    header_pb->set_sequence_number(packet_idx);
    header_pb->set_timestamp(1000 * packet_idx);

    l.LogPacket(packet_pb);
  }
}

} // namespace pando
} // namespace pnd
