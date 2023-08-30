#pragma once

#include <string>

#include <H5Cpp.h>

#include "device_base.h"
#include "hdf5_comptype_accessor.h"
#include "hdf5_table.h"
#include "mutex.h"

#include <cstdint>
#include <memory>

namespace pnd {
namespace pando {

class HDF5MockHistogrammer final : public DeviceBase {
 public:
  HDF5MockHistogrammer(std::string file_path);

 private:
  static constexpr char kPayloadDsetName[] = "Histograms";

  void StartAcquisition() final;
  void StopAcquisition() final;
  void UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) final;

  DeviceType GetDeviceType() final {
    return DeviceType::HISTOGRAMMER;
  }

  void ReadNextRow();

  H5LockAdapter<H5::H5File> file_;
  std::unique_ptr<HDF5Table> table_;
  HDF5CompTypeAccessor accessor_;
  const HDF5CompTypeFieldDescriptor* bin_size_fd_;
  const HDF5CompTypeFieldDescriptor* first_bin_idx_fd_;
  const HDF5CompTypeFieldDescriptor* last_bin_idx_fd_;
  const HDF5CompTypeFieldDescriptor* counts_fd_;

  std::vector<size_t> enabled_cols_;

  HDF5Table::Buffer buff_;
  uint64_t buff_begin_bin_idx_ = 0;
  uint64_t buff_end_bin_idx_ = 0;
  hsize_t table_rd_idx_ = 0;
  uint64_t next_bin_idx_ = 0;
};

} // namespace pando
} // namespace pnd
