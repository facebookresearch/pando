#include "camera_frame_handler.h"
#include "protobuf_util.h"
#include "reporter.h"

namespace pnd {
namespace pando {

CameraFrameHandler::CameraFrameHandler(
    const char* raw_file_name,
    HDF5CompTypeFieldDescriptor::CppType pixel_type,
    std::size_t width,
    std::size_t height,
    int32_t experiment_id,
    bool log_raw_frames,
    bool publish_raw_frames)
    : pixel_type_{pixel_type},
      width_{width},
      height_{height},
      experiment_id_{experiment_id},
      publish_raw_frames_{publish_raw_frames} {
  if (log_raw_frames && raw_file_name)
    logger_.emplace(raw_file_name, pixel_type_, width_, height_);

  // Send ZMQ Start event
  publish_worker_.Async([=]() {
    publisher_.SendZmqEvent(proto::EVENT_START, experiment_id_, event_seq_++, event_timestamp);
  });
}

CameraFrameHandler::~CameraFrameHandler() {
  // Send ZMQ Stop event
  publish_worker_.Async([=]() {
    publisher_.SendZmqEvent(proto::EVENT_STOP, experiment_id_, event_seq_++, event_timestamp);
  });
}

BlockingFuture<void> CameraFrameHandler::Handle(const CameraFrame& frame) {
  // Stop event will have timestamp of the last frame published
  event_timestamp = frame.timestamp;

  std::future<void> publish_done;
  std::future<void> log_done;

  auto sequence_number = image_seq_++;

  if (publish_raw_frames_)
    publish_done =
        publish_worker_.Async([=]() { ProtobufAndPublishFrame(frame, sequence_number); });

  if (logger_)
    log_done = log_worker_.Async([=]() { logger_->Log(frame, sequence_number); });

  // Return a new future that is a monad over publish_done & log_done
  return std::async(

      std::launch::deferred,
      [this, publish_done = std::move(publish_done), log_done = std::move(log_done)]() mutable {
        bool blocked_on_publish{}, blocked_on_log{};

        if (publish_done.valid()) {
          blocked_on_publish =
              publish_done.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
          publish_done.get();
        }

        if (log_done.valid()) {
          blocked_on_log =
              log_done.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
          log_done.get();
        }

        if (blocked_on_publish || blocked_on_log)
          g_reporter->warn(
              "CameraFrameHandler: blocked_on_publish_: {}, blocked_on_log_: {} ",
              blocked_on_publish,
              blocked_on_log);
      }

  );
}

void CameraFrameHandler::ProtobufAndPublishFrame(
    const CameraFrame& frame,
    int64_t sequence_number) {
  // Populate packet header fields
  auto& header = *image_packet_.mutable_header();
  header.set_experiment_id(experiment_id_);
  header.set_sequence_number(sequence_number);
  header.set_timestamp(frame.timestamp.count());

  std::size_t pixel_size{};
  switch (pixel_type_) {
    case HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8:
      pixel_size = 1;
      break;
    case HDF5CompTypeFieldDescriptor::CPPTYPE_UINT16:
      pixel_size = 2;
      break;
    default:
      assert(false);
  }

  // Populate payload field
  auto* payload = image_packet_.mutable_payload()->mutable_camera_image();
  payload->set_device_id(0); // TODO support multiple cameras, if requested
  payload->set_width(width_);
  payload->set_height(height_);
  payload->set_bytes_per_pixel(pixel_size);
  payload->set_image_data(frame.data.get(), frame.size);
  payload->set_exposure_time_us(frame.exposure_time.count());

  zmq::message_t msg;
  common::ProtobufSerialize(image_packet_, msg);

  static constexpr char image_topic[] = "pando.camera_image";
  publisher_.PublishZmqMessage(image_topic, sizeof(image_topic), msg);
}

} // namespace pando
} // namespace pnd
