#pragma once

#include "camera_frame.h"
#include "hdf5_image_logger.h"
#include "publisher.h"
#include "thread_container.h"
#include "worker.h"

#include <chrono>
#include <memory>
#include <optional>

namespace pnd {
namespace pando {

/** A building block for camera device classes.
 *   - Publishes raw frames over ZMQ
 *   - Logs raw frames to HDF5
 *   - Sends ZMQ Start/Stop events
 */
class CameraFrameHandler {
 public:
  CameraFrameHandler(
      const char* raw_file_name,
      HDF5CompTypeFieldDescriptor::CppType pixel_type,
      std::size_t width,
      std::size_t height,
      int32_t experiment_id,
      bool log_raw_frames,
      bool publish_raw_frames);

  ~CameraFrameHandler();

  BlockingFuture<void> Handle(const CameraFrame& frame);

 private:
  void ProtobufAndPublishFrame(const CameraFrame& frame, int64_t sequence_number);

  Publisher publisher_;
  HDF5CompTypeFieldDescriptor::CppType pixel_type_;
  std::size_t width_;
  std::size_t height_;
  int32_t experiment_id_;
  bool publish_raw_frames_;

  int64_t image_seq_ = 0;
  int64_t event_seq_ = 0;
  std::chrono::nanoseconds event_timestamp{};

  proto::Packet image_packet_;

  std::optional<ImageLogger> logger_;

  Worker publish_worker_{"CameraFrameHandler::publish_worker_"};
  Worker log_worker_{"CameraFrameHandler::log_worker_"};
};

} // namespace pando
} // namespace pnd
