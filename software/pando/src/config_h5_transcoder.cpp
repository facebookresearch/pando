#include "config_h5_transcoder.h"

#include "hdf5_logger.h"
#include "mutex.h"
#include "reporter.h"

#include <fmt/format.h>

namespace pnd {
namespace pando {

template <class T>
std::string FormatVector(const std::vector<T>& vector) {
  std::string result("[");
  bool first = true;
  for (auto& item : vector) {
    if (!first) {
      result += ", ";
    } else {
      first = false;
    }
    result += fmt::format("{}", item);
  }
  result += "]";
  return result;
}

template <class T1, class T2>
std::string FormatMap(const std::map<T1, T2>& map) {
  std::string result = "{";
  bool first_element = true;
  for (auto& entry : map) {
    if (!first_element) {
      result += ", ";
    } else {
      first_element = false;
    }
    result += fmt::format("{{{}: {}}}", entry.first, entry.second);
  }
  result += "}";
  return result;
}

template <class T>
std::vector<int32_t> ConvertToIntVector(std::vector<T> value) {
  std::vector<int32_t> int_value;
  for (const auto& device : value) {
    int_value.push_back(static_cast<int32_t>(device));
  }
  return int_value;
}

void ConfigH5Transcoder::Write(H5::H5File& file, PandoInterface::Config config) {
  H5LockGuard l;
  // Add metadata group
  H5::DataSpace dataspace(H5S_SCALAR);
  auto metadata = file.createGroup("/metadata");
  // write pando config to HDF5 file
  WriteAttribute("device", static_cast<int>(config.device), metadata);
  WriteAttribute("exp_type", static_cast<int>(config.exp_type), metadata);
  WriteAttribute("mock_file", config.mock_file, metadata);
  WriteAttribute("output_dir", config.output_dir, metadata);
  WriteAttribute("bin_size_ns", config.bin_size_ns, metadata);
  WriteAttribute("points_per_level", config.points_per_level, metadata);
  WriteAttribute("n_levels", config.n_levels, metadata);
  WriteAttribute("rebin_factor", config.rebin_factor, metadata);
  WriteAttribute("final_bin_count", config.final_bin_count, metadata);
  WriteAttribute("calc_g2i", config.calc_g2i, metadata);
  WriteAttribute("enabled_channels", config.enabled_channels, metadata);
  WriteAttribute("pf32_laser_master", config.pf32_laser_master, metadata);
  WriteAttribute("pf32_bit_mode", static_cast<int>(config.pf32_bit_mode), metadata);
  WriteAttribute("xharp_level", config.xharp_level, metadata);
  WriteAttribute("xharp_zerox", config.xharp_zerox, metadata);
  WriteAttribute("xharp_sync_div", config.xharp_sync_div, metadata);
  WriteAttribute("xharp_sync_level", config.xharp_sync_level, metadata);
  WriteAttribute("xharp_sync_zerox", config.xharp_sync_zerox, metadata);
  WriteAttribute("xharp_sync_offset", config.xharp_sync_offset, metadata);
  WriteAttribute("multi_harp_sync_trigger_edge", config.multi_harp_sync_trigger_edge, metadata);
  WriteAttribute(
      "multi_harp_sync_trigger_level_mv", config.multi_harp_sync_trigger_level_mv, metadata);
  WriteAttribute("multi_harp_sync_tdead_ps", config.multi_harp_sync_tdead_ps, metadata);
  WriteAttribute("laser_sync_period_ps", config.laser_sync_period_ps, metadata);
  WriteAttribute("hharp_input_offsets", config.hharp_input_offsets, metadata);
  WriteAttribute("multi_harp_trigger_edge", config.multi_harp_trigger_edge, metadata);
  WriteAttribute("multi_harp_trigger_level_mv", config.multi_harp_trigger_level_mv, metadata);
  WriteAttribute("multi_harp_tdead_ps", config.multi_harp_tdead_ps, metadata);
  WriteAttribute("pf32_firmware_path", config.pf32_firmware_path, metadata);
  WriteAttribute("dtof_range_min", config.dtof_range_min, metadata);
  WriteAttribute("dtof_range_max", config.dtof_range_max, metadata);
  WriteAttribute("cri_offset", config.cri_offset, metadata);
  WriteAttribute("cri_width", config.cri_width, metadata);
  WriteAttribute("publish_raw_data", config.publish_raw_data, metadata);
  WriteAttribute("log_raw_data", config.log_raw_data, metadata);
  WriteAttribute("log_analyzed_data", config.log_analyzed_data, metadata);
  WriteAttribute("log_peripheral_data", config.log_peripheral_data, metadata);
  WriteAttribute("count_integ_period_ns", config.count_integ_period_ns, metadata);
  WriteAttribute("dtof_integ_period_ns", config.dtof_integ_period_ns, metadata);
  WriteAttribute("cri_integ_period_ns", config.cri_integ_period_ns, metadata);
  WriteAttribute("use_pandoboxd", config.use_pandoboxd, metadata);
  WriteAttribute("pandoboxd_ip_addr", config.pandoboxd_ip_addr, metadata);
  WriteAttribute("use_pps", config.use_pps, metadata);
  WriteAttribute(
      "camera_frame_trigger_period_10ns", config.camera_frame_trigger_period_10ns, metadata);
  WriteAttribute("camera_exposure_time_us", config.camera_exposure_time_us, metadata);
  WriteAttribute("camera_enable_test_pattern", config.camera_enable_test_pattern, metadata);
  WriteAttribute("aux_frame_trigger_period_10ns", config.aux_frame_trigger_period_10ns, metadata);
  WriteAttribute("camera_enable_roi", config.camera_enable_roi, metadata);
  WriteAttribute("camera_roi_x_offset", config.camera_roi_x_offset, metadata);
  WriteAttribute("camera_roi_y_offset", config.camera_roi_y_offset, metadata);
  WriteAttribute("camera_roi_width", config.camera_roi_width, metadata);
  WriteAttribute("camera_roi_height", config.camera_roi_height, metadata);
  WriteAttribute("pf32_g2_frame_count", config.pf32_g2_frame_count, metadata);
  WriteAttribute("pf32_g2_rebin_factor_0", config.pf32_g2_rebin_factor_0, metadata);
  WriteAttribute("pf32_g2_rebin_factor_1", config.pf32_g2_rebin_factor_1, metadata);
  WriteAttribute("pf32_g2_rebin_factor_2", config.pf32_g2_rebin_factor_2, metadata);
  WriteAttribute("pf32_g2_rebin_factor_3", config.pf32_g2_rebin_factor_3, metadata);
}

void ConfigH5Transcoder::WriteAttribute(const std::string& name, int value, H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, value);
  auto dataspace = H5::DataSpace(H5S_SCALAR);
  auto attr = metadata.createAttribute(name, H5::PredType::NATIVE_INT, dataspace);
  attr.write(H5::PredType::NATIVE_INT, &value);
}

void ConfigH5Transcoder::WriteAttribute(
    const std::string& name,
    const std::string& value,
    H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, value);
  if (value.length() > 0) {
    auto dataspace = H5::DataSpace(H5S_SCALAR);
    H5::StrType str_type(H5::PredType::C_S1, value.length());
    auto attr = metadata.createAttribute(name, str_type, dataspace);
    attr.write(str_type, value.c_str());
  }
}

void ConfigH5Transcoder::WriteAttribute(
    const std::string& name,
    uint64_t value,
    H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, value);
  auto dataspace = H5::DataSpace(H5S_SCALAR);
  auto attr = metadata.createAttribute(name, H5::PredType::NATIVE_UINT64, dataspace);
  attr.write(H5::PredType::NATIVE_UINT64, &value);
}

void ConfigH5Transcoder::WriteAttribute(
    const std::string& name,
    uint32_t value,
    H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, value);
  auto dataspace = H5::DataSpace(H5S_SCALAR);
  auto attr = metadata.createAttribute(name, H5::PredType::NATIVE_UINT32, dataspace);
  attr.write(H5::PredType::NATIVE_UINT32, &value);
}

void ConfigH5Transcoder::WriteAttribute(
    const std::string& name,
    const std::map<int, int>& map,
    H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, FormatMap(map));
  using MapEntry = std::map<int, int>::value_type;
  H5::CompType entryType(sizeof(MapEntry));
  entryType.insertMember("key", HOFFSET(MapEntry, first), H5::PredType::NATIVE_INT);
  entryType.insertMember("value", HOFFSET(MapEntry, second), H5::PredType::NATIVE_INT);
  std::vector<MapEntry> entries(map.cbegin(), map.cend());
  int ndims = 1;
  const hsize_t dims = map.size();
  if (dims > 0) {
    auto dataspace = H5::DataSpace(ndims, &dims);
    auto attr = metadata.createAttribute(name, entryType, dataspace);
    attr.write(entryType, entries.data());
  }
}

void ConfigH5Transcoder::WriteAttribute(
    const std::string& name,
    const std::vector<int32_t>& value,
    H5::Group metadata) {
  // copy into a variable of known type
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, FormatVector(value));
  const hsize_t dims = value.size();
  int ndims = 1;
  if (value.size() > 0) {
    auto dataspace = H5::DataSpace(ndims, &dims);
    auto attr = metadata.createAttribute(name, H5::PredType::NATIVE_INT, dataspace);
    attr.write(H5::PredType::NATIVE_INT, value.data());
  }
}

void ConfigH5Transcoder::WriteAttribute(
    const std::string& name,
    const std::list<int32_t>& value,
    H5::Group metadata) {
  std::vector<int32_t> vector(value.cbegin(), value.cend());
  WriteAttribute(name, vector, metadata);
}

void ConfigH5Transcoder::WriteAttribute(const std::string& name, double value, H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, value);
  auto dataspace = H5::DataSpace(H5S_SCALAR);
  auto attr = metadata.createAttribute(name, H5::PredType::NATIVE_DOUBLE, dataspace);
  attr.write(H5::PredType::NATIVE_DOUBLE, &value);
}

void ConfigH5Transcoder::WriteAttribute(const std::string& name, bool value, H5::Group metadata) {
  g_reporter->debug("ConfigH5Transcoder: {} = {}", name, value);
  auto dataspace = H5::DataSpace(H5S_SCALAR);
  auto attr = metadata.createAttribute(name, H5::PredType::NATIVE_CHAR, dataspace);
  attr.write(H5::PredType::NATIVE_CHAR, &value);
}

} // namespace pando
} // namespace pnd
