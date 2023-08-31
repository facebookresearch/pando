#include "mock_x_harp_base.h"

namespace pnd {
namespace pando {

MockXHarpBase::MockXHarpBase(const std::string& file_path, uint32_t record_type)
    : ptu_reader_{file_path, record_type} {
  picosecondsPerTick_ = static_cast<uint64_t>(ptu_reader_.getResolution() * 1000000000000.0);
  g_reporter->debug("MockXHarpBase: picosecondsPerTick_ {}", picosecondsPerTick_);
}

void MockXHarpBase::Configure() {
  ptu_reader_.Rewind();
}

void MockXHarpBase::Acquire() {
  RecordBuffer* read_buff = ring_buffer_.AdvanceWrite();
  read_buff->occupancy = 0;
  while (!acquisition_stop_signal_.ShouldStop()) {
    // Flip the buffer if possible
    if (auto new_buffer = ring_buffer_.MaybeAdvanceWrite()) {
      new_buffer->occupancy = 0;
      read_buff = new_buffer;
    }

    // Sleep if the buffer is full, otherwise read kReadBlockSize records from the PTU file
    if (read_buff->occupancy + kReadBlockSize > kReadBufferSize) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else {
      auto n_records =
          ptu_reader_.GetRecords(read_buff->data + read_buff->occupancy, kReadBlockSize);
      read_buff->occupancy += n_records;

      // Check if we've reached the end of the file
      if (n_records == 0) {
        g_reporter->info("Reached end of PTU file");
        // Finalize read_buff before returning only if the end of the PTU file was reached
        while (!acquisition_stop_signal_.ShouldStop() && !ring_buffer_.MaybeAdvanceWrite()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return;
      }
    }
  }
}

} // namespace pando
} // namespace pnd
