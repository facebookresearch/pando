#include "hdf5_table.h"

#include <H5Cpp.h>

#include <chrono>
#include <cstdint>
#include <thread>

#include <iostream>

#include "gtest/gtest.h"

namespace pnd {
namespace pando {

TEST(HDF5TableTest, WriteReadTest) {
  static constexpr char dset_name[] = "uint64_dset";
  static constexpr hsize_t n_writes = 30;
  static constexpr hsize_t n_cols = 32;
  static constexpr hsize_t n_rows_per_write = 1001;
  static constexpr hsize_t chunk_size = 4;

  H5::H5File file_{"hdf5_table_test.h5", H5F_ACC_TRUNC};
  H5::DataType dtype = H5::PredType::NATIVE_INT64;
  HDF5Table::Buffer wbuf_, rbuf_;

  std::vector<int> write_channels;
  for (int i = 0; i < n_cols; i++) {
    write_channels.push_back(i * 2);
  }

  HDF5Table table_w{file_, dtype, dset_name, write_channels, chunk_size};
  EXPECT_EQ(table_w.GetDType().getId(), dtype.getId());
  EXPECT_EQ(table_w.GetDTypeSize(), sizeof(uint64_t));

  hsize_t expected_n_rows = 0;

  for (uint64_t write_idx = 0; write_idx < n_writes; ++write_idx) {
    EXPECT_EQ(table_w.GetNRows(), (expected_n_rows / chunk_size) * chunk_size);
    EXPECT_EQ(table_w.GetNCols(), n_cols);

    wbuf_.assign(sizeof(uint64_t) * n_rows_per_write * n_cols, 0);
    auto dst = reinterpret_cast<uint64_t*>(wbuf_.data());
    for (uint64_t row_idx = 0; row_idx < n_rows_per_write; ++row_idx) {
      for (uint64_t col_idx = 0; col_idx < n_cols; ++col_idx) {
        auto index = (write_idx * n_rows_per_write + row_idx) * n_cols + col_idx;
        *dst++ = index;
      }
    }
    table_w.Append(wbuf_);

    expected_n_rows += n_rows_per_write;
    EXPECT_EQ(table_w.GetNRows(), (expected_n_rows / chunk_size) * chunk_size);
    EXPECT_EQ(table_w.GetNCols(), n_cols);
  }

  // There is data left in the chunk buffer. Flush it out.
  ASSERT_TRUE(expected_n_rows % chunk_size);
  table_w.WriteChunkBuff();
  EXPECT_EQ(table_w.GetNRows(), expected_n_rows);

  HDF5Table table_r{file_, dset_name};

  EXPECT_EQ(table_r.GetDTypeSize(), sizeof(uint64_t));
  auto& read_channels = table_r.GetChannels();
  EXPECT_EQ(read_channels.size(), write_channels.size());
  for (int i = 0; i < read_channels.size(); i++) {
    EXPECT_EQ(read_channels[i], write_channels[i]);
  }

  table_r.Read(rbuf_, 0, n_writes * n_rows_per_write);
  EXPECT_EQ(table_r.GetNRows(), expected_n_rows);
  EXPECT_EQ(table_r.GetNCols(), n_cols);

  for (size_t i = 0; i < n_writes * n_rows_per_write * n_cols; ++i) {
    auto src = reinterpret_cast<uint64_t*>(rbuf_.data()) + i;
    EXPECT_EQ(*src, i);
  }
}

} // namespace pando
} // namespace pnd
