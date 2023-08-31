#include "hdf5_mock_timetagger.h"

#include "reporter.h"

#include <fmt/format.h>

#include <algorithm>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <utility>

namespace pnd {
namespace pando {

void HDF5MockTimetagger::StartAcquisition() {
  H5LockGuard l;

  timestamp_channels_.clear();
  bool learned_dtype = false;
  H5::DataType dtype;

  for (auto ch_idx : config_.enabled_channels) {
    auto name = fmt::format("{}Channel{}", "Timestamps", ch_idx);
    if (!file_.nameExists(name)) {
      throw std::runtime_error(
          "HDF5MockTimetagger: HDF5 dataset missing channel specified as enabled");
    }

    // Open the dataset for this channel
    auto ep_res = timestamp_channels_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(ch_idx),
        std::forward_as_tuple(file_, name));

    auto& channel_data = ep_res.first->second;

    if (channel_data.GetNCols() != 1) {
      throw std::runtime_error("HDF5MockTimetagger: HDF5 dataset is not 1D");
    }

    if (learned_dtype) {
      if (channel_data.GetDType() != dtype) {
        throw std::runtime_error("HDF5MockTimetagger: Properties differ between Datasets");
      }
    } else {
      dtype = channel_data.GetDType();
      timestamps_accessor_ = HDF5CompTypeAccessor{dtype};

      timestamps_micro_time_fd_ = timestamps_accessor_.GetFieldDescriptor("micro_times");
      if (!timestamps_micro_time_fd_) {
        g_reporter->warn("HDF5MockTimetagger: Dataset does not contain micro_times field");
      }

      timestamps_macro_time_fd_ = timestamps_accessor_.GetFieldDescriptor("macro_times");
      if (!timestamps_macro_time_fd_) {
        throw std::runtime_error(
            "HDF5MockTimetagger: HDF5 dataset does not contain macro_times field");
      }
      learned_dtype = true;
    }

    channel_data.buff.reserve(channel_data.GetDTypeSize() * kReadSize);
  }

  marker_channels_.clear();
  learned_dtype = false;

  std::list<int32_t> marker_channel_indices{0, 1, 2, 3}; // TODO maybe change this.
  for (auto& ch_idx : marker_channel_indices) {
    auto name = fmt::format("{}Channel{}", "Markers", ch_idx);
    if (file_.nameExists(name)) {
      // Open the dataset for this channel
      auto ep_res = marker_channels_.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(ch_idx),
          std::forward_as_tuple(file_, name));

      auto& channel_data = ep_res.first->second;

      if (channel_data.GetNCols() != 1) {
        throw std::runtime_error("HDF5MockTimetagger: HDF5 dataset is not 1D");
      }

      // Marker channels should have the same dtype as timestamps.
      if (learned_dtype) {
        if (channel_data.GetDType() != dtype) {
          throw std::runtime_error("HDF5MockTimetagger: Properties differ between Datasets");
        }
      } else {
        dtype = channel_data.GetDType();
        markers_accessor_ = HDF5CompTypeAccessor{dtype};

        markers_macro_time_fd_ = markers_accessor_.GetFieldDescriptor("macro_times");
        if (!markers_macro_time_fd_) {
          throw std::runtime_error(
              "HDF5MockTimetagger: HDF5 dataset does not contain macro_times field");
        }
        learned_dtype = true;
      }

      channel_data.buff.reserve(channel_data.GetDTypeSize() * kReadSize);
    }
  }
}

void HDF5MockTimetagger::StopAcquisition() {}

HDF5MockTimetagger::HDF5MockTimetagger(std::string file_path) {
  g_reporter->debug("HDF5MockTimetagger: opening {}", file_path);
  file_ = H5::H5File{file_path, H5F_ACC_RDONLY};
}

void HDF5MockTimetagger::UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) {
  std::chrono::nanoseconds bin_size{config_.bin_size_ns};
  auto from = bin_size * begin_bin_idx;
  auto until = bin_size * end_bin_idx;

  for (auto& time_tags : dest.timestamps) {
    // Clear the macro and micro time vectors for each channel without deallocating any memory.
    time_tags.second.macro_times().clear();
    time_tags.second.micro_times().clear();

    // Record the span of the timestamps we're about to store
    time_tags.second.SetSpan(from, until);
  }
  for (auto& time_tags : dest.marker_timestamps) {
    // clear marker timestamps
    time_tags.second.macro_times().clear();
    time_tags.second.micro_times().clear();

    // Record the span of the timestamps we're about to store
    time_tags.second.SetSpan(from, until);
  }

  // Used to detect whether all data has been read out of the HDF5 file being replayed
  // Set to false if any data read off of any channel, otherwise stays true and experiment can end.
  bool reached_eof = true;

  auto timestamps_it = dest.timestamps.begin();
  auto channel_data_it = timestamp_channels_.begin();

  while (timestamps_it != dest.timestamps.end()) {
    assert(channel_data_it != timestamp_channels_.end());
    assert(channel_data_it->first == timestamps_it->first);

    auto& ch_idx = channel_data_it->first;
    ChannelTimestamps& timestamps = timestamps_it->second;
    ChannelData& channel_data = channel_data_it->second;

    MacroTime macro_time{0};
    MicroTime micro_time{0};

    while (1) {
      // Keep channel_data.buff non-empty
      if (channel_data.buff_rd_idx * channel_data.GetDTypeSize() == channel_data.buff.size()) {
        H5LockGuard l;

        auto read_size = std::min(kReadSize, channel_data.GetNRows() - channel_data.table_rd_idx);

        if (read_size == 0) {
          break;
        }

        channel_data.Read(channel_data.buff, channel_data.table_rd_idx, read_size);
        channel_data.table_rd_idx += read_size;
        channel_data.buff_rd_idx = 0;
      }
      reached_eof = false;

      if (timestamps_micro_time_fd_) {
        micro_time =
            MicroTime{timestamps_accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT32>(
                channel_data.buff, timestamps_micro_time_fd_, channel_data.buff_rd_idx)};
      }
      macro_time =
          MacroTime{timestamps_accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64>(
              channel_data.buff, timestamps_macro_time_fd_, channel_data.buff_rd_idx)};

      if (macro_time < until) {
        timestamps.macro_times().push_back(macro_time);
        timestamps.micro_times().push_back(micro_time);

        channel_data.buff_rd_idx++;
      } else {
        break;
      }
    }

    ++timestamps_it;
    ++channel_data_it;
  }

  auto markers_it = dest.marker_timestamps.begin();
  channel_data_it = marker_channels_.begin();
  while (marker_channels_.size() != 0 && markers_it != dest.marker_timestamps.end()) {
    assert(channel_data_it != marker_channels_.end());
    assert(channel_data_it->first == markers_it->first);

    auto& ch_idx = channel_data_it->first;
    ChannelTimestamps& markers = markers_it->second;
    ChannelData& channel_data = channel_data_it->second;

    MacroTime macro_time{0};
    MicroTime micro_time{0};

    while (1) {
      // Keep channel_data.buff non-empty
      if (channel_data.buff_rd_idx * channel_data.GetDTypeSize() == channel_data.buff.size()) {
        H5LockGuard l;

        auto read_size = std::min(kReadSize, channel_data.GetNRows() - channel_data.table_rd_idx);

        if (read_size == 0) {
          break;
        }

        channel_data.Read(channel_data.buff, channel_data.table_rd_idx, read_size);
        channel_data.table_rd_idx += read_size;
        channel_data.buff_rd_idx = 0;
      }
      reached_eof = false;

      macro_time =
          MacroTime{markers_accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64>(
              channel_data.buff, markers_macro_time_fd_, channel_data.buff_rd_idx)};

      if (macro_time < until) {
        markers.macro_times().push_back(macro_time);

        channel_data.buff_rd_idx++;
      } else {
        break;
      }
    }

    ++markers_it;
    ++channel_data_it;
  }

  if (reached_eof) {
    throw std::runtime_error("HDF5MockTimetagger: Reached end of HDF5 file.");
  }

  std::this_thread::sleep_until(start_time_ + until);
}

} // namespace pando
} // namespace pnd
