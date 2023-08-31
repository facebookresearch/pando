#pragma once

#include <string>

#include <H5Cpp.h>

#include "device_base.h"
#include "hdf5_comptype_accessor.h"
#include "hdf5_table.h"
#include "mutex.h"
#include "types.h"

#include <cstdint>
#include <memory>

namespace pnd {
namespace pando {

class HDF5MockTimetagger final : public DeviceBase {
 public:
  HDF5MockTimetagger(std::string file_path);

 private:
  // Staple some metadata onto an HDF5Table
  struct ChannelData : HDF5Table {
    using HDF5Table::HDF5Table;
    hsize_t table_rd_idx = 0;
    Buffer buff;
    size_t buff_rd_idx = 0;
  };

  static constexpr hsize_t kReadSize = 50000;

  void StartAcquisition() final;
  void StopAcquisition() final;
  void UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) final;

  DeviceType GetDeviceType() final {
    return DeviceType::TIMETAGGER;
  }

  bool DeviceGeneratesMicrotimes() final {
    return true;
  }

  H5LockAdapter<H5::H5File> file_;
  std::map<int32_t, ChannelData> timestamp_channels_;
  std::map<int32_t, ChannelData> marker_channels_;
  HDF5CompTypeAccessor timestamps_accessor_;
  HDF5CompTypeAccessor markers_accessor_;
  const HDF5CompTypeFieldDescriptor* timestamps_micro_time_fd_;
  const HDF5CompTypeFieldDescriptor* timestamps_macro_time_fd_;
  const HDF5CompTypeFieldDescriptor* markers_macro_time_fd_;
};

} // namespace pando
} // namespace pnd
