#include "hdf5_mock_histogrammer.h"

#include "reporter.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iterator>
#include <stdexcept>
#include <thread>

namespace pnd {
namespace pando {

void HDF5MockHistogrammer::StartAcquisition() {
  // Reset state
  buff_.clear();
  buff_begin_bin_idx_ = 0;
  buff_end_bin_idx_ = 0;
  table_rd_idx_ = 0;
  next_bin_idx_ = 0;

  // Determine which columns in the table correspond to the currently enabled channels
  enabled_cols_.clear();
  auto file_channels = table_->GetChannels();
  for (auto channel : config_.enabled_channels) {
    auto found = std::find(file_channels.cbegin(), file_channels.cend(), channel);
    if (found == file_channels.cend()) {
      throw std::runtime_error(
          "HDF5MockHistogrammer: HDF5 dataset missing channel specified as enabled");
    } else {
      enabled_cols_.push_back(std::distance(file_channels.cbegin(), found));
    }
  }

  // Read the first row
  ReadNextRow();
  if (buff_begin_bin_idx_ != 0) {
    g_reporter->warn("HDF5MockHistogrammer: H5 file begins at bin index {}", buff_begin_bin_idx_);
    next_bin_idx_ = buff_begin_bin_idx_;
  }

  // Check that the bin size recorded in the first column of the first row matches the current
  // config (We assume if one cell matches they all do.)
  auto bin_size =
      accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_INT64>(buff_, bin_size_fd_, 0);
  if (config_.bin_size_ns != bin_size) {
    throw std::runtime_error(
        "HDF5MockHistogrammer: bin size of histogram data in the HDF5 file does not match current configuration");
  }
}

void HDF5MockHistogrammer::StopAcquisition() {}

HDF5MockHistogrammer::HDF5MockHistogrammer(std::string file_path) {
  H5LockGuard l;

  g_reporter->debug("HDF5MockHistogrammer: opening {}", file_path);
  file_ = H5::H5File{file_path, H5F_ACC_RDONLY};

  if (!file_.nameExists(kPayloadDsetName)) {
    throw std::runtime_error("HDF5MockHistogrammer: HDF5 file missing Histograms dataset");
  }

  table_ = std::make_unique<HDF5Table>(file_, kPayloadDsetName);
  accessor_ = HDF5CompTypeAccessor{table_->GetDType()};
  bin_size_fd_ = accessor_.GetFieldDescriptor("bin_size");
  first_bin_idx_fd_ = accessor_.GetFieldDescriptor("first_bin_idx");
  last_bin_idx_fd_ = accessor_.GetFieldDescriptor("last_bin_idx");
  counts_fd_ = accessor_.GetFieldDescriptor("counts");
}

void HDF5MockHistogrammer::ReadNextRow() {
  if (table_rd_idx_ >= table_->GetNRows()) {
    throw std::runtime_error("HDF5MockHistogrammer: reached end of HDF5 dataset");
  }

  // Read a new row from the table
  table_->Read(buff_, table_rd_idx_++, 1);

  // Retrieve span from the first column of the newly read row (assume other columns match)
  buff_begin_bin_idx_ =
      accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64>(buff_, first_bin_idx_fd_, 0);
  buff_end_bin_idx_ =
      accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64>(buff_, last_bin_idx_fd_, 0) +
      1;

  // We'll assume this is true when reading the counts field
  assert(buff_end_bin_idx_ - buff_begin_bin_idx_ == counts_fd_->GetArrayLen());
}

void HDF5MockHistogrammer::UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) {
  // Set span  for every channel
  for (auto& histogram_kv : dest.histograms) {
    auto& histogram = histogram_kv.second;
    histogram.SetSpan(begin_bin_idx, end_bin_idx);
  }

  // Copy appropriate bins from the histograms recorded in the HDF5 table into dest.histograms
  // This may require reading 0, 1, or more rows from the table.
  while (next_bin_idx_ < end_bin_idx) {
    // Read a new row if we've copied all the bins from the current one
    if (next_bin_idx_ >= buff_end_bin_idx_) {
      assert(next_bin_idx_ == buff_end_bin_idx_);
      ReadNextRow();
    }

    // The (last + 1) bin index to copy out of buff_ in this loop iteration.
    // Either buff_ contains all the bins we need and this will be the last loop iteration, or we
    // need to copy all remaining bins out of buff_ and then read a new row from the table in the
    // next loop iteration.
    auto end_copy_idx = std::min(end_bin_idx, buff_end_bin_idx_);

    // Sanity checks
    assert(next_bin_idx_ >= buff_begin_bin_idx_);
    assert(next_bin_idx_ >= begin_bin_idx);
    assert(end_copy_idx > buff_begin_bin_idx_);

    // Calculate the source and destination bin ranges for the copy
    auto src_begin_offset = next_bin_idx_ - buff_begin_bin_idx_;
    auto src_end_offset = end_copy_idx - buff_begin_bin_idx_;
    auto dst_begin_offset = next_bin_idx_ - begin_bin_idx;

    // Iterate over enabled channels
    auto col_idx_it = enabled_cols_.cbegin();
    for (auto& histogram_kv : dest.histograms) {
      // Prove that we're copying from the correct column for this channel
      assert(histogram_kv.first == table_->GetChannels()[*col_idx_it]);

      auto& histogram = histogram_kv.second;
      const auto* counts = accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(
          buff_, counts_fd_, *col_idx_it++);

      auto src_begin = counts + src_begin_offset;
      auto src_end = counts + src_end_offset;
      auto dst_begin = histogram.data() + dst_begin_offset;

      // Check that the copy won't overflow the source or destination
      assert(src_end <= counts + counts_fd_->GetArrayLen());
      assert(dst_begin + std::distance(src_begin, src_end) <= histogram.data() + histogram.size());

      std::copy(src_begin, src_end, dst_begin);
    }
    next_bin_idx_ = end_copy_idx;
  }

  std::chrono::nanoseconds until{config_.bin_size_ns * end_bin_idx};
  std::this_thread::sleep_until(start_time_ + until);
}

} // namespace pando
} // namespace pnd
