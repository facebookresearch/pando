#include "device_base.h"

#include <array>

namespace pnd {
namespace pando {

DeviceBase::RawData DeviceBase::CreateDest() {
  RawData dest;
  dest.timestamps = {config_.enabled_channels.cbegin(), config_.enabled_channels.cend()};
  dest.histograms = {config_.enabled_channels.cbegin(),
                     config_.enabled_channels.cend(),
                     std::chrono::nanoseconds(config_.bin_size_ns)};

  // TODO support variable number of marker channels
  static constexpr std::array<int32_t, 4> marker_channel_indices{0, 1, 2, 3};
  dest.marker_timestamps = {marker_channel_indices.cbegin(), marker_channel_indices.cend()};

  return dest;
}

} // namespace pando
} // namespace pnd
