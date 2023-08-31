
#include "hdf5_image_logger.h"

#include <H5ArrayType.h>

namespace pnd {
namespace pando {

ImageLogger::ImageLogger(
    const char* file_name,
    HDF5CompTypeFieldDescriptor::CppType pixel_type,
    std::size_t width,
    std::size_t height)
    : file_(file_name, H5F_ACC_EXCL) {
  // Build compound type & accessor for metatadata_table_
  HDF5CompTypeAccessor::CompTypeSpecifier metadata_specifier;
  metadata_specifier.AddField(kSequenceNumberFieldName, kSequenceNumberCppType);
  metadata_specifier.AddField(kTimestampFieldName, kTimestampCppType);
  metadata_specifier.AddField(kExposureTimeFieldName, kExposureTimeCppType);
  metadata_accessor_ = HDF5CompTypeAccessor{metadata_specifier};

  sequence_number_fd_ = metadata_accessor_.GetFieldDescriptor(kSequenceNumberFieldName);
  timestamp_fd_ = metadata_accessor_.GetFieldDescriptor(kTimestampFieldName);
  exposure_time_fd_ = metadata_accessor_.GetFieldDescriptor(kExposureTimeFieldName);

  // dtype of Images dset is a 2D array (not a compound type)
  const hsize_t img_dims[2]{width, height};
  H5::ArrayType images_h5_type(HDF5CompTypeFieldDescriptor::GetPredType(pixel_type), 2, img_dims);

  metadata_table_.emplace(file_, metadata_accessor_.GetH5Type(), "Metadata", kChunkSize);
  image_table_.emplace(file_, images_h5_type, "Images", kChunkSize);
};

void ImageLogger::Log(const CameraFrame& frame, int32_t sequence_number) {
  // Serialize metadata
  HDF5Table::Buffer wbuf(metadata_accessor_.GetH5TypeSize());
  metadata_accessor_.SetField<kSequenceNumberCppType>(
      wbuf, sequence_number_fd_, 0, sequence_number);
  metadata_accessor_.SetField<kTimestampCppType>(wbuf, timestamp_fd_, 0, frame.timestamp.count());
  metadata_accessor_.SetField<kExposureTimeCppType>(
      wbuf, exposure_time_fd_, 0, frame.exposure_time.count());

  // Append to tables
  metadata_table_->Append(wbuf);
  image_table_->Append(frame.data.get(), frame.size);
}

} // namespace pando
} // namespace pnd
