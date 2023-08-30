#pragma once

#include "ptu_reader.h"
#include "x_harp_base.h"

#include <string>
#include <thread>

namespace pnd {
namespace pando {

/** Base class for "Mocked" PicoQuant devices.
 * Implements Consume and Acquire, leaving UpdateRawData unimplemented.
 */
class MockXHarpBase : virtual public XHarpBase {
 protected:
  /** Open a PTU file.
   * @param file_path Path to the PTU file
   * @param record_type The record type the PTU file is expected to contain
   */
  MockXHarpBase(const std::string& file_path, uint32_t record_type);

 private:
  /** The number of records to read from the PTU file at at time. */
  static constexpr size_t kReadBlockSize = 131072;

  void Configure() final;
  void Acquire() final;

  PTUReader ptu_reader_;
};

/** Template for creating fully implemented device class for "Mocked" PicoQuant devices.
 * @tparam ProcBase Class with XHarpBase as a virtual base, implementing UpdateRawData
 * @tparam record_type The record type PTU files are expected to contain
 */
template <class ProcBase, uint32_t record_type>
class MockXHarp : public ProcBase, MockXHarpBase {
 public:
  /** Open a PTU file.
   * @param file_path Path to the PTU file
   */
  MockXHarp(const std::string& file_path) : MockXHarpBase{file_path, record_type} {};

 private:
  /** Wrap the implementation of UpdateRawData to limit processing speed to real-time */
  void UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) final {
    ProcBase::UpdateRawData(begin_bin_idx, end_bin_idx, dest);
    auto until = std::chrono::nanoseconds(config_.bin_size_ns) * end_bin_idx;
    std::this_thread::sleep_until(start_time_ + until);
  }
};

} // namespace pando
} // namespace pnd
