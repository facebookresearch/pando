#include "pando.h"
#include "reporter.h"
#include "singleton_context.h"
#include "version.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace pnd::common;
using namespace pnd::pando;
namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(SessionHandle);

PYBIND11_MODULE(pypando, pj) {
  pj.doc() = "pybind11 generated bindings for the C++ class Pando";

  auto pando =
      py::class_<PandoInterface, std::shared_ptr<PandoInterface>>(pj, "Pando")
          .def_readonly_static("kProxyEgressInprocEp", &PandoInterface::kProxyEgressInprocEp)
          .def_readonly_static(
              "kProxyEgressTcpStartPort", &PandoInterface::kProxyEgressTcpStartPort)
          .def_readonly_static("kProxyEgressTcpEndPort", &PandoInterface::kProxyEgressTcpEndPort)
          .def_readonly_static("kProxyIngressInprocEp", &PandoInterface::kProxyIngressInprocEp)
          .def("Configure", &PandoInterface::Configure)
          .def("Start", &PandoInterface::Start<>)
          .def("IsHealthy", &PandoInterface::IsHealthy)
          .def("GetProxyEgressPort", &PandoInterface::GetProxyEgressPort);

  py::enum_<PandoInterface::DeviceType>(pando, "DeviceType")
      .value("NONE", PandoInterface::DeviceType::NONE)
      .value("HYDRA_HARP_T2", PandoInterface::DeviceType::HYDRA_HARP_T2)
      .value("HYDRA_HARP_T3", PandoInterface::DeviceType::HYDRA_HARP_T3)
      .value("PICO_HARP_T2", PandoInterface::DeviceType::PICO_HARP_T2)
      .value("PICO_HARP_T3", PandoInterface::DeviceType::PICO_HARP_T3)
      .value("MULTI_HARP_T2", PandoInterface::DeviceType::MULTI_HARP_T2)
      .value("MULTI_HARP_T3", PandoInterface::DeviceType::MULTI_HARP_T3)
      .value("PF32_DCS", PandoInterface::DeviceType::PF32_DCS)
      .value("PF32_TCSPC", PandoInterface::DeviceType::PF32_TCSPC)
      .value("PF32_G2", PandoInterface::DeviceType::PF32_G2)
      .value("HYDRA_HARP_T2_MOCK_PTU", PandoInterface::DeviceType::HYDRA_HARP_T2_MOCK_PTU)
      .value("HYDRA_HARP_T3_MOCK_PTU", PandoInterface::DeviceType::HYDRA_HARP_T3_MOCK_PTU)
      .value("PICO_HARP_T2_MOCK_PTU", PandoInterface::DeviceType::PICO_HARP_T2_MOCK_PTU)
      .value("PICO_HARP_T3_MOCK_PTU", PandoInterface::DeviceType::PICO_HARP_T3_MOCK_PTU)
      .value("MULTI_HARP_T2_MOCK_PTU", PandoInterface::DeviceType::MULTI_HARP_T2_MOCK_PTU)
      .value("MULTI_HARP_T3_MOCK_PTU", PandoInterface::DeviceType::MULTI_HARP_T3_MOCK_PTU)
      .value("TIMETAGGER_MOCK_HDF5", PandoInterface::DeviceType::TIMETAGGER_MOCK_HDF5)
      .value("HISTOGRAMMER_MOCK_HDF5", PandoInterface::DeviceType::HISTOGRAMMER_MOCK_HDF5)
      .value("BASLER_A2", PandoInterface::DeviceType::BASLER_A2)
      .value("CAMERA_MOCK_HDF5", PandoInterface::DeviceType::CAMERA_MOCK_HDF5)
      .value("ORCA_FUSION", PandoInterface::DeviceType::ORCA_FUSION)
      .value("BASLER_ACA2000_CL", PandoInterface::DeviceType::BASLER_ACA2000_CL)
      .value("BASLER_BOOST", PandoInterface::DeviceType::BASLER_BOOST);

  py::enum_<PandoInterface::ExperimentType>(pando, "ExperimentType")
      .value("NONE", PandoInterface::ExperimentType::NONE)
      .value("TD", PandoInterface::ExperimentType::TD)
      .value("DCS", PandoInterface::ExperimentType::DCS)
      .value("TDDCS", PandoInterface::ExperimentType::TDDCS);

  py::enum_<PandoInterface::PF32BitMode>(pando, "PF32BitMode")
      .value("SIXTEEN", PandoInterface::PF32BitMode::SIXTEEN)
      .value("EIGHT", PandoInterface::PF32BitMode::EIGHT)
      .value("FOUR", PandoInterface::PF32BitMode::FOUR);

  py::class_<PandoInterface::Config>(pando, "Config")
      .def(py::init<>())
      .def_readwrite("device", &PandoInterface::Config::device)
      .def_readwrite("exp_type", &PandoInterface::Config::exp_type)
      .def_readwrite("mock_file", &PandoInterface::Config::mock_file)
      .def_readwrite("output_dir", &PandoInterface::Config::output_dir)
      .def_readwrite("bin_size_ns", &PandoInterface::Config::bin_size_ns)
      .def_readwrite("points_per_level", &PandoInterface::Config::points_per_level)
      .def_readwrite("n_levels", &PandoInterface::Config::n_levels)
      .def_readwrite("rebin_factor", &PandoInterface::Config::rebin_factor)
      .def_readwrite("final_bin_count", &PandoInterface::Config::final_bin_count)
      .def_readwrite("calc_g2i", &PandoInterface::Config::calc_g2i)
      .def_readwrite("enabled_channels", &PandoInterface::Config::enabled_channels)
      .def_readwrite("pf32_laser_master", &PandoInterface::Config::pf32_laser_master)
      .def_readwrite("xharp_level", &PandoInterface::Config::xharp_level)
      .def_readwrite("pf32_bit_mode", &PandoInterface::Config::pf32_bit_mode)
      .def_readwrite("xharp_zerox", &PandoInterface::Config::xharp_zerox)
      .def_readwrite("xharp_sync_offset", &PandoInterface::Config::xharp_sync_offset)
      .def_readwrite("xharp_sync_div", &PandoInterface::Config::xharp_sync_div)
      .def_readwrite("xharp_sync_level", &PandoInterface::Config::xharp_sync_level)
      .def_readwrite("xharp_sync_zerox", &PandoInterface::Config::xharp_sync_zerox)
      .def_readwrite(
          "multi_harp_sync_trigger_edge", &PandoInterface::Config::multi_harp_sync_trigger_edge)
      .def_readwrite(
          "multi_harp_sync_trigger_level_mv",
          &PandoInterface::Config::multi_harp_sync_trigger_level_mv)
      .def_readwrite("multi_harp_sync_tdead_ps", &PandoInterface::Config::multi_harp_sync_tdead_ps)
      .def_readwrite("multi_harp_trigger_edge", &PandoInterface::Config::multi_harp_trigger_edge)
      .def_readwrite(
          "multi_harp_trigger_level_mv", &PandoInterface::Config::multi_harp_trigger_level_mv)
      .def_readwrite("multi_harp_tdead_ps", &PandoInterface::Config::multi_harp_tdead_ps)
      .def_readwrite("laser_sync_period_ps", &PandoInterface::Config::laser_sync_period_ps)
      .def_readwrite("hharp_input_offsets", &PandoInterface::Config::hharp_input_offsets)
      .def_readwrite("pf32_firmware_path", &PandoInterface::Config::pf32_firmware_path)
      .def_readwrite("dtof_range_min", &PandoInterface::Config::dtof_range_min)
      .def_readwrite("dtof_range_max", &PandoInterface::Config::dtof_range_max)
      .def_readwrite("cri_offset", &PandoInterface::Config::cri_offset)
      .def_readwrite("cri_width", &PandoInterface::Config::cri_width)
      .def_readwrite("publish_raw_data", &PandoInterface::Config::publish_raw_data)
      .def_readwrite("log_raw_data", &PandoInterface::Config::log_raw_data)
      .def_readwrite("log_analyzed_data", &PandoInterface::Config::log_analyzed_data)
      .def_readwrite("log_peripheral_data", &PandoInterface::Config::log_peripheral_data)
      .def_readwrite("count_integ_period_ns", &PandoInterface::Config::count_integ_period_ns)
      .def_readwrite("dtof_integ_period_ns", &PandoInterface::Config::dtof_integ_period_ns)
      .def_readwrite("cri_integ_period_ns", &PandoInterface::Config::cri_integ_period_ns)
      .def_readwrite("use_pandoboxd", &PandoInterface::Config::use_pandoboxd)
      .def_readwrite("pandoboxd_ip_addr", &PandoInterface::Config::pandoboxd_ip_addr)
      .def_readwrite("use_pps", &PandoInterface::Config::use_pps)
      .def_readwrite(
          "camera_frame_trigger_period_10ns",
          &PandoInterface::Config::camera_frame_trigger_period_10ns)
      .def_readwrite("camera_exposure_time_us", &PandoInterface::Config::camera_exposure_time_us)
      .def_readwrite(
          "camera_enable_test_pattern", &PandoInterface::Config::camera_enable_test_pattern)
      .def_readwrite("camera_enable_roi", &PandoInterface::Config::camera_enable_roi)
      .def_readwrite("camera_roi_x_offset", &PandoInterface::Config::camera_roi_x_offset)
      .def_readwrite("camera_roi_y_offset", &PandoInterface::Config::camera_roi_y_offset)
      .def_readwrite("camera_roi_width", &PandoInterface::Config::camera_roi_width)
      .def_readwrite("camera_roi_height", &PandoInterface::Config::camera_roi_height)
      .def_readwrite(
          "aux_frame_trigger_period_10ns", &PandoInterface::Config::aux_frame_trigger_period_10ns)
      .def_readwrite("pf32_g2_frame_count", &PandoInterface::Config::pf32_g2_frame_count)
      .def_readwrite("pf32_g2_rebin_factor_0", &PandoInterface::Config::pf32_g2_rebin_factor_0)
      .def_readwrite("pf32_g2_rebin_factor_1", &PandoInterface::Config::pf32_g2_rebin_factor_1)
      .def_readwrite("pf32_g2_rebin_factor_2", &PandoInterface::Config::pf32_g2_rebin_factor_2)
      .def_readwrite("pf32_g2_rebin_factor_3", &PandoInterface::Config::pf32_g2_rebin_factor_3)
      .def_readwrite("pf32_g2_burst_mode", &PandoInterface::Config::pf32_g2_burst_mode);

  py::class_<SessionHandle>(pando, "SessionHandle");

  py::class_<SingletonPando>(pj, "SingletonPando").def_static("Get", &SingletonPando::Get);

  auto reporter = py::class_<Reporter>(pj, "Reporter")
                      .def_static("OpenLogfile", &Reporter::OpenLogfile)
                      .def_static("SetLogfileLevel", &Reporter::SetLogfileLevel)
                      .def_static("SetStderrLevel", &Reporter::SetStderrLevel);

  pj.def("LogVersion", &LogVersion);

  py::enum_<spdlog::level::level_enum>(reporter, "Level")
      .value("TRACE", spdlog::level::trace)
      .value("DEBUG", spdlog::level::debug)
      .value("INFO", spdlog::level::info)
      .value("WARN", spdlog::level::warn)
      .value("ERR", spdlog::level::err)
      .value("CRITICAL", spdlog::level::critical)
      .value("OFF", spdlog::level::off);

  py::class_<zmq::context_t, std::shared_ptr<zmq::context_t>>(pj, "Context")
      .def("__int__", [](zmq::context_t& c) {
        return reinterpret_cast<size_t>(static_cast<void*>(c));
      });

  py::class_<SingletonContext>(pj, "SingletonContext").def_static("Get", &SingletonContext::Get);

  pj.attr("PandoVersionString") = py::str(JULIET_VERSION_STRING);
}
