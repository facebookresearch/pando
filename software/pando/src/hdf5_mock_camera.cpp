#include "hdf5_mock_camera.h"
#include "hdf5_comptype_accessor.h"
#include "hdf5_image_logger.h"
#include "reporter.h"
#include "saturating_copy.h"

#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

HDF5MockCamera::HDF5MockCamera(std::string file_path)
    : file_(file_path, H5F_ACC_RDONLY),
      image_table_(file_, "Images"),
      metadata_table_(file_, "Metadata") {
  H5LockGuard l;

  g_reporter->debug("HDF5MockCamera: opened {}", file_path);

  // Safely downcast H5::DataType to H5::ArrayType
  if (image_table_.GetDType().getClass() != H5T_ARRAY)
    throw std::runtime_error("HDF5MockCamera: image dataset contains non-array dtype");
  auto image_dtype = static_cast<const H5::ArrayType&>(image_table_.GetDType());

  if (image_dtype.getArrayNDims() != 2)
    throw std::runtime_error("HDF5MockCamera: image_dtype is not 2D");
  hsize_t image_dims[2];
  image_dtype.getArrayDims(image_dims);
  width_ = image_dims[0];
  height_ = image_dims[1];
  g_reporter->debug("HDF5MockCamera: resolution is {}x{}", width_, height_);

  pixel_type_ = HDF5CompTypeFieldDescriptor::GetCppType(image_dtype.getSuper());

  g_reporter->debug("HDF5MockCamera: Pixel size is {}", image_dtype.getSuper().getSize());
}

void HDF5MockCamera::StartImpl(
    int32_t experiment_id,
    PandoInterface::Config config,
    const char* raw_file_name) {
  config_ = std::move(config);

  // TODO check exposure matches

  // Constructing frame_handler opens the raw H5 file (if raw logging is enabled). We do this here,
  // not in the grab thread, so that any exception thrown is immediately handled by the caller.
  auto frame_handler = std::make_shared<CameraFrameHandler>(
      raw_file_name,
      // NOTE: we convert all images to 8 bit before passing them to frame_handler
      // pixel_type_,
      HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8,
      width_,
      height_,
      experiment_id,
      config.log_raw_data,
      config_.publish_raw_data);

  run_thread_ = common::ThreadContainer(
      [this, frame_handler] { Grab(std::move(frame_handler)); },
      "HDF5MockCamera::Grab",
      &stop_signal_);
}

void HDF5MockCamera::StopImpl() {
  run_thread_.Stop();
  run_thread_.Join();
}

void HDF5MockCamera::Grab(std::shared_ptr<CameraFrameHandler> frame_handler) {
  HDF5CompTypeAccessor metadata_accessor(metadata_table_.GetDType());
  auto seq_num_fd = metadata_accessor.GetFieldDescriptor(ImageLogger::kSequenceNumberFieldName);
  auto timestamp_fd = metadata_accessor.GetFieldDescriptor(ImageLogger::kTimestampFieldName);
  auto exposure_fd = metadata_accessor.GetFieldDescriptor(ImageLogger::kExposureTimeFieldName);

  BlockingFuture<void> handle_done;

  HDF5Table::Buffer metadata_buff;
  auto buff_a = std::make_shared<HDF5Table::Buffer>();
  auto buff_b = std::make_shared<HDF5Table::Buffer>();

  for (hsize_t table_rd_idx = 0; !stop_signal_.ShouldStop(); ++table_rd_idx) {
    if (table_rd_idx >= metadata_table_.GetNRows())
      throw std::runtime_error("HDF5MockCamera: reached end of HDF5 dataset");

    // Read from metadata table
    metadata_table_.Read(metadata_buff, table_rd_idx, 1);
    auto sequence_number = metadata_accessor.GetField<ImageLogger::kSequenceNumberCppType>(
        metadata_buff, seq_num_fd, 0);
    std::chrono::nanoseconds timestamp{
        metadata_accessor.GetField<ImageLogger::kTimestampCppType>(metadata_buff, timestamp_fd, 0)};
    // Handle H5 files with no exposure_time_us field.
    std::chrono::duration<double, std::micro> exposure_time{
        exposure_fd ? metadata_accessor.GetField<ImageLogger::kExposureTimeCppType>(
                          metadata_buff, exposure_fd, 0)
                    : 0};

    // Read from image data table
    image_table_.Read(*buff_a, table_rd_idx, 1);

    // Convert 16 bit images to 8 bit in place
    if (pixel_type_ == HDF5CompTypeFieldDescriptor::CPPTYPE_UINT16) {
      const auto width = width_;
      const auto height = height_;
      for (int i = 0; i < height; ++i) {
        auto src_row_start = reinterpret_cast<uint16_t*>(buff_a->data()) + i * width;
        auto dst_row_start = reinterpret_cast<uint8_t*>(buff_a->data()) + i * width;
        SaturatingCopy(src_row_start, src_row_start + width, dst_row_start);
      }
      buff_a->resize(sizeof(uint8_t) * (width * height));
    }

    // Limit playback to real time
    std::this_thread::sleep_until(start_time_ + timestamp);

    // Flip image buffer
    if (handle_done.valid())
      handle_done.get();
    std::swap(*buff_a, *buff_b);

    handle_done = frame_handler->Handle({
        // Use the aliasing constructor of shared_ptr to share
        // ownership of buff_b while pointing to
        // the image buffer it owns.
        {buff_b, buff_b->data()},
        buff_b->size(),
        timestamp,
        exposure_time,
    });
  }
}

} // namespace pando
} // namespace pnd
