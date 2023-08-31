#pragma once

#include "hdf5_comptype_accessor.h"
#include "hdf5_table.h"
#include "histogram.h"
#include "kv_container.h"
#include "mutex.h"
#include "reporter.h"
#include "types.h"
#include "worker.h"

#include <H5Cpp.H>
#include <fmt/format.h>
#include <map>
#include <memory>

namespace pnd {
namespace pando {

class RawTimestampLogger {
 public:
  RawTimestampLogger(H5::H5File& raw_file) : file_(raw_file){};
  void LogChannels(const KVContainer<int32_t, ChannelTimestamps>& timestamps);

 private:
  H5::H5File& file_;
  bool initialized_ = false;

  std::map<int32_t, HDF5Table> tables_;

  hsize_t kChunkSize = 10000;

  HDF5CompTypeAccessor accessor_;
  HDF5Table::Buffer wbuf_, wbuf_worker_;
  Worker append_worker_{"RawTimestampLogger::append_worker_"};
  BlockingFuture<void> append_worker_done_;

  void Init();
  void Write(const KVContainer<int32_t, ChannelTimestamps>& timestamps);

  const HDF5CompTypeFieldDescriptor* macro_times_fd_;
  static constexpr char kMacroTimesFieldName[] = "macro_times";
  static constexpr auto kMacroTimesCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64;

  const HDF5CompTypeFieldDescriptor* micro_times_fd_;
  static constexpr char kMicroTimesFieldName[] = "micro_times";
  static constexpr auto kMicroTimesCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT32;
};

class RawMarkerLogger {
 public:
  RawMarkerLogger(H5::H5File& raw_file) : file_(raw_file){};
  void LogChannels(const KVContainer<int32_t, ChannelTimestamps>& timestamps);

 private:
  H5::H5File& file_;
  bool initialized_ = false;

  std::map<int32_t, HDF5Table> tables_;

  hsize_t kChunkSize = 1000;

  HDF5CompTypeAccessor accessor_;
  HDF5Table::Buffer wbuf_;

  void Init();
  void Write(const KVContainer<int32_t, ChannelTimestamps>& timestamps);

  const HDF5CompTypeFieldDescriptor* macro_times_fd_;
  static constexpr char kMacroTimesFieldName[] = "macro_times";
  static constexpr auto kMacroTimesCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64;
};

class RawHistogramLogger {
 public:
  RawHistogramLogger(H5::H5File& raw_file) : file_(raw_file){};
  void LogChannels(const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms);

 private:
  H5::H5File& file_;
  bool initialized_ = false;

  std::unique_ptr<HDF5Table> table_;

  hsize_t kChunkSize = 1;

  HDF5CompTypeAccessor accessor_;
  HDF5Table::Buffer wbuf_, wbuf_worker_;
  Worker append_worker_{"RawHistogramLogger::append_worker_"};
  BlockingFuture<void> append_worker_done_;

  void Init(const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms);
  void Write(const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms);

  const HDF5CompTypeFieldDescriptor* bin_size_fd_;
  static constexpr char kBinSizeFieldName[] = "bin_size";
  static constexpr auto kBinSizeCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_INT64;

  const HDF5CompTypeFieldDescriptor* first_bin_idx_fd_;
  static constexpr char kFirstBinIdxFieldName[] = "first_bin_idx";
  static constexpr auto kFirstBinIdxCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64;

  const HDF5CompTypeFieldDescriptor* last_bin_idx_fd_;
  static constexpr char kLastBinIdxFieldName[] = "last_bin_idx";
  static constexpr auto kLastBinIdxCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT64;

  const HDF5CompTypeFieldDescriptor* counts_fd_;
  static constexpr char kCountsFieldName[] = "counts";
  static constexpr auto kCountsCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8;
};

class HDF5RawLogger {
 public:
  explicit HDF5RawLogger();
  explicit HDF5RawLogger(const char* raw_file_name);
  void LogTimestamps(const KVContainer<int32_t, ChannelTimestamps>& timestamps);
  void LogMarkers(const KVContainer<int32_t, ChannelTimestamps>& markers);
  void LogHistograms(const KVContainer<int32_t, Histogram<std::chrono::nanoseconds>>& histograms);

 private:
  H5LockAdapter<H5::H5File> file_;

  RawTimestampLogger timestamps_logger_{file_};
  RawMarkerLogger markers_logger_{file_};
  RawHistogramLogger histograms_logger_{file_};
};

} // namespace pando
} // namespace pnd
