#pragma once

#include "camera_frame.h"
#include "hdf5_comptype_accessor.h"
#include "hdf5_comptype_field_descriptor.h"
#include "hdf5_table.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace pnd {
namespace pando {

class ImageLogger {
 public:
  static constexpr char kSequenceNumberFieldName[] = "sequence_number";
  static constexpr char kTimestampFieldName[] = "timestamp";
  static constexpr char kExposureTimeFieldName[] = "exposure_time_us";

  static constexpr auto kSequenceNumberCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_INT32;
  static constexpr auto kTimestampCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_INT64;
  static constexpr auto kExposureTimeCppType = HDF5CompTypeFieldDescriptor::CPPTYPE_DOUBLE;

  static constexpr hsize_t kChunkSize = 1;

  ImageLogger(
      const char* file_name,
      HDF5CompTypeFieldDescriptor::CppType pixel_type,
      std::size_t width,
      std::size_t height);

  void Log(const CameraFrame& frame, int32_t sequence_number);

 private:
  H5LockAdapter<H5::H5File> file_;

  std::optional<HDF5Table> image_table_;

  std::optional<HDF5Table> metadata_table_;

  // We use a compound type with 2 fields (timestamp & sequence_number) for the metadata table.
  // image data is stored in a 2D array type.
  HDF5CompTypeAccessor metadata_accessor_;

  const HDF5CompTypeFieldDescriptor* sequence_number_fd_;

  const HDF5CompTypeFieldDescriptor* timestamp_fd_;

  const HDF5CompTypeFieldDescriptor* exposure_time_fd_;
};

} // namespace pando
} // namespace pnd
