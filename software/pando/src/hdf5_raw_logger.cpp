#include "hdf5_raw_logger.h"

#include <utility>

namespace pnd {
namespace pando {

HDF5RawLogger::HDF5RawLogger(const char* raw_file_name) : file_(raw_file_name, H5F_ACC_EXCL) {}

void HDF5RawLogger::LogTimestamps(const KVContainer<int32_t, ChannelTimestamps>& timestamps) {
  timestamps_logger_.LogChannels(timestamps);
}
void HDF5RawLogger::LogMarkers(const KVContainer<int32_t, ChannelTimestamps>& markers) {
  markers_logger_.LogChannels(markers);
}
void HDF5RawLogger::LogHistograms(
    const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms) {
  histograms_logger_.LogChannels(histograms);
}

void RawHistogramLogger::LogChannels(
    const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms) {
  H5LockGuard l;
  if (!initialized_) {
    Init(histograms);
  }
  Write(histograms);
}

void RawHistogramLogger::Init(
    const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms) {
  auto& histogram = (*histograms.cbegin()).second;

  HDF5CompTypeAccessor::CompTypeSpecifier specifier;
  specifier.AddField(kBinSizeFieldName, kBinSizeCppType);
  specifier.AddField(kFirstBinIdxFieldName, kFirstBinIdxCppType);
  specifier.AddField(kLastBinIdxFieldName, kLastBinIdxCppType);
  specifier.AddField(
      kCountsFieldName, kCountsCppType, histogram.GetEndBinIdx() - histogram.GetBeginBinIdx());

  accessor_ = HDF5CompTypeAccessor{specifier};

  bin_size_fd_ = accessor_.GetFieldDescriptor(kBinSizeFieldName);
  first_bin_idx_fd_ = accessor_.GetFieldDescriptor(kFirstBinIdxFieldName);
  last_bin_idx_fd_ = accessor_.GetFieldDescriptor(kLastBinIdxFieldName);
  counts_fd_ = accessor_.GetFieldDescriptor(kCountsFieldName);

  std::vector<int32_t> channels;
  for (auto& histogram_kv : histograms) {
    channels.push_back(histogram_kv.first);
  }

  table_ =
      std::make_unique<HDF5Table>(file_, accessor_.GetH5Type(), "Histograms", channels, kChunkSize);
  initialized_ = true;
}

void RawHistogramLogger::Write(
    const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms) {
  wbuf_.clear();
  auto size = accessor_.GetH5TypeSize() *
      histograms.size(); // this should be number of channels in hist. Double check?
  wbuf_.resize(size);

  int idx = 0;
  for (auto& histogram_kv : histograms) {
    auto& histogram = histogram_kv.second;

    accessor_.SetField<kBinSizeCppType>(wbuf_, bin_size_fd_, idx, histogram.GetBinSize().count());

    accessor_.SetField<kFirstBinIdxCppType>(
        wbuf_, first_bin_idx_fd_, idx, histogram.GetBeginBinIdx());

    accessor_.SetField<kLastBinIdxCppType>(
        wbuf_, last_bin_idx_fd_, idx, histogram.GetEndBinIdx() - 1);

    auto wbuf_index = accessor_.GetFieldP<kCountsCppType>(wbuf_, counts_fd_, idx);
    std::copy(histogram.cbegin(), histogram.cend(), wbuf_index);

    idx++;
  }

  // Make sure the last job is done before we flip buffers
  if (append_worker_done_.valid()) {
    append_worker_done_.get();
  }

  std::swap(wbuf_, wbuf_worker_);
  append_worker_done_ = append_worker_.Async([this]() { table_->Append(wbuf_worker_); });
}

void RawTimestampLogger::LogChannels(const KVContainer<int32_t, ChannelTimestamps>& timestamps) {
  if (!initialized_) {
    Init();
  }
  Write(timestamps);
}

void RawTimestampLogger::Init() {
  HDF5CompTypeAccessor::CompTypeSpecifier specifier;
  specifier.AddField(kMacroTimesFieldName, kMacroTimesCppType);
  specifier.AddField(kMicroTimesFieldName, kMicroTimesCppType);
  accessor_ = HDF5CompTypeAccessor{specifier};

  macro_times_fd_ = accessor_.GetFieldDescriptor(kMacroTimesFieldName);
  micro_times_fd_ = accessor_.GetFieldDescriptor(kMicroTimesFieldName);

  initialized_ = true;
}

void RawTimestampLogger::Write(const KVContainer<int32_t, ChannelTimestamps>& timestamps) {
  for (auto& timestamps_kv : timestamps) {
    wbuf_.clear();
    auto timestamps_size = timestamps_kv.second.macro_times().size();
    wbuf_.resize(accessor_.GetH5TypeSize() * timestamps_size);

    int idx = 0;
    for (auto timestamp : timestamps_kv.second.macro_times()) {
      accessor_.SetField<kMacroTimesCppType>(wbuf_, macro_times_fd_, idx++, timestamp.count());
    }
    idx = 0;
    for (auto timestamp : timestamps_kv.second.micro_times()) {
      accessor_.SetField<kMicroTimesCppType>(wbuf_, micro_times_fd_, idx++, timestamp.count());
    }

    // Make sure the last job is done before we flip buffers
    if (append_worker_done_.valid()) {
      append_worker_done_.get();
    }

    std::swap(wbuf_, wbuf_worker_);
    append_worker_done_ = append_worker_.Async([this, ch_idx = timestamps_kv.first]() {
      auto tables_it = tables_.find(ch_idx);
      if (tables_it == tables_.end()) {
        auto name = fmt::format("TimestampsChannel{}", ch_idx);
        auto ep_res = tables_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(ch_idx),
            std::forward_as_tuple(
                file_, accessor_.GetH5Type(), name, std::vector<int>{(int)ch_idx}, kChunkSize));
        tables_it = ep_res.first;
        assert(ep_res.second);
        g_reporter->debug("RawTimestampLogger: Created dataset {}", name);
      }

      tables_it->second.Append(wbuf_worker_);
    });
  }
}

void RawMarkerLogger::LogChannels(const KVContainer<int32_t, ChannelTimestamps>& timestamps) {
  H5LockGuard l;
  if (!initialized_) {
    Init();
  }
  Write(timestamps);
}

void RawMarkerLogger::Init() {
  HDF5CompTypeAccessor::CompTypeSpecifier specifier;
  specifier.AddField(kMacroTimesFieldName, kMacroTimesCppType);
  accessor_ = HDF5CompTypeAccessor{specifier};

  macro_times_fd_ = accessor_.GetFieldDescriptor(kMacroTimesFieldName);

  initialized_ = true;
}

void RawMarkerLogger::Write(const KVContainer<int32_t, ChannelTimestamps>& timestamps) {
  for (auto& timestamps_kv : timestamps) {
    auto tables_it = tables_.find(timestamps_kv.first);
    if (tables_it == tables_.end()) {
      auto name = fmt::format("MarkersChannel{}", timestamps_kv.first);
      auto ep_res = tables_.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(timestamps_kv.first),
          std::forward_as_tuple(
              file_,
              accessor_.GetH5Type(),
              name,
              std::vector<int>{(int)timestamps_kv.first},
              kChunkSize));
      tables_it = ep_res.first;
      assert(ep_res.second);
      g_reporter->debug("RawTimestampLogger: Created dataset {}", name);
    }
    wbuf_.clear();
    auto timestamps_size = timestamps_kv.second.macro_times().size();
    wbuf_.resize(accessor_.GetH5TypeSize() * timestamps_size);

    int idx = 0;
    for (auto timestamp : timestamps_kv.second.macro_times()) {
      accessor_.SetField<kMacroTimesCppType>(wbuf_, macro_times_fd_, idx++, timestamp.count());
    }

    tables_it->second.Append(wbuf_);
  }
}

} // namespace pando
} // namespace pnd
