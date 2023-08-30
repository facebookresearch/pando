#include "pando.pb.h"
#include "pando_impl.h"

#include "basler_a2.h"
#include "basler_aca2000_cl.h"
#include "basler_boost.h"
#include "correlator.h"
#include "device_base.h"
#include "device_processor.h"
#include "hdf5_mock_camera.h"
#include "hdf5_mock_histogrammer.h"
#include "hdf5_mock_timetagger.h"
#include "hydra_harp.h"
#include "log_exception.h"
#include "multi_harp.h"
#include "orca_fusion.h"
#include "pf32_dcs.h"
#include "pf32_g2.h"
#include "pf32_tt.h"
#include "pico_harp.h"
#include "protobuf_util.h"
#include "reporter.h"
#include "singleton_context.h"
#include "version.h"
#include "worker.h"

#include <cmath>

#include <fmt/format.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <utility>

namespace pnd {
namespace pando {

Pando::Pando() try : experiment_monitor_(common::ThreadMonitor::Get()) {
  // Bind the Proxy to its static endpoints, then start it.
  proxy_.BindIngress(kProxyIngressInprocEp);
  proxy_.BindEgress(kProxyEgressInprocEp);
  proxy_tcp_egress_port_ = proxy_.BindEgress(kProxyEgressTcpStartPort, kProxyEgressTcpEndPort);
  proxy_.Start();

  // Add the ZMQ sink to g_reporter
  common::Reporter::CreateZmqSink(kProxyIngressInprocEp, kReportTopicFormatString);
  // common::Reporter::SetStderrLevel(spdlog::level::critical);
} catch (...) {
  common::LogException();
}

void Pando::Configure(const Config& config) try {
  std::lock_guard<std::mutex> l(config_lock_);

  // TODO come up with a better way to choose device
  if ((config.device != config_.device) ||
      (config.pf32_laser_master != config_.pf32_laser_master) ||
      ((config.device == DeviceType::HYDRA_HARP_T2_MOCK_PTU ||
        config.device == DeviceType::HYDRA_HARP_T3_MOCK_PTU ||
        config.device == DeviceType::PICO_HARP_T2_MOCK_PTU ||
        config.device == DeviceType::PICO_HARP_T3_MOCK_PTU ||
        config.device == DeviceType::TIMETAGGER_MOCK_HDF5 ||
        config.device == DeviceType::MULTI_HARP_T2_MOCK_PTU ||
        config.device == DeviceType::MULTI_HARP_T3_MOCK_PTU ||
        config.device == DeviceType::HISTOGRAMMER_MOCK_HDF5) &&
       config.mock_file != config_.mock_file) ||
      ((config.device == DeviceType::PF32_DCS || config.device == DeviceType::PF32_TCSPC) &&
       config.pf32_firmware_path != config_.pf32_firmware_path) ||
      ((config.device == DeviceType::PF32_DCS || config.device == DeviceType::PF32_TCSPC) &&
       config.pf32_bit_mode != config_.pf32_bit_mode)) {
    g_reporter->debug("Pando::Configure: Constructing new device ({})", (int)config.device);

    // End any existing session
    if (device_session_handle_) {
      g_reporter->warn(
          "Pando::Configure: Device changed while session in progress, stopping device session early.");
      device_session_handle_.reset();
    }

    // Destruct any existing device
    device_.reset();

    // Construct a new device
    switch (config.device) {
      case DeviceType::NONE:
        break;
      case DeviceType::HYDRA_HARP_T2:
        device_ = std::make_unique<DeviceProcessor>(std::make_unique<HydraHarpT2>());
        break;
      case DeviceType::HYDRA_HARP_T3:
        device_ = std::make_unique<DeviceProcessor>(std::make_unique<HydraHarpT3>());
        break;
      case DeviceType::PICO_HARP_T2:
        device_ = std::make_unique<DeviceProcessor>(std::make_unique<PicoHarpT2>());
        break;
      case DeviceType::PICO_HARP_T3:
        device_ = std::make_unique<DeviceProcessor>(std::make_unique<PicoHarpT3>());
        break;
      case DeviceType::MULTI_HARP_T2:
        device_ = std::make_unique<DeviceProcessor>(std::make_unique<MultiHarpT2>());
        break;
      case DeviceType::MULTI_HARP_T3:
        device_ = std::make_unique<DeviceProcessor>(std::make_unique<MultiHarpT3>());
        break;
      case DeviceType::PF32_DCS:
        switch (config.pf32_bit_mode) {
          case PF32BitMode::SIXTEEN:
            // TODO find a way to inform hdf5_logger_ to use a 16 bit type for the Histograms
            // dataset, then allow this case.
            throw std::runtime_error(
                "Pando::Configure: Using the PF32_DCS device in 16 bit mode is not currently supported due to a limitation of the HDF5 logger.");
            device_ = std::make_unique<DeviceProcessor>(
                std::make_unique<PF32Dcs16Bit>(config.pf32_firmware_path.c_str()));
            break;
          case PF32BitMode::EIGHT:
            device_ = std::make_unique<DeviceProcessor>(
                std::make_unique<PF32Dcs8Bit>(config.pf32_firmware_path.c_str()));
            break;
          case PF32BitMode::FOUR:
            device_ = std::make_unique<DeviceProcessor>(
                std::make_unique<PF32Dcs4Bit>(config.pf32_firmware_path.c_str()));
            break;
        }
        break;
      case DeviceType::PF32_TCSPC:
        switch (config.pf32_bit_mode) {
          case PF32BitMode::SIXTEEN:
            device_ = std::make_unique<DeviceProcessor>(
                std::make_unique<PF32Tt16Bit>(config.pf32_firmware_path.c_str()));
            break;
          case PF32BitMode::EIGHT:
            device_ = std::make_unique<DeviceProcessor>(
                std::make_unique<PF32Tt8Bit>(config.pf32_firmware_path.c_str()));
            break;
          case PF32BitMode::FOUR:
            device_ = std::make_unique<DeviceProcessor>(
                std::make_unique<PF32Tt4Bit>(config.pf32_firmware_path.c_str()));
            break;
        }
        break;
      case DeviceType::PF32_G2:
        device_ = std::make_unique<PF32G2>(config.pf32_firmware_path.c_str());
        break;
      case DeviceType::HYDRA_HARP_T2_MOCK_PTU:
        device_ =
            std::make_unique<DeviceProcessor>(std::make_unique<MockHydraHarpT2>(config.mock_file));
        break;
      case DeviceType::HYDRA_HARP_T3_MOCK_PTU:
        device_ =
            std::make_unique<DeviceProcessor>(std::make_unique<MockHydraHarpT3>(config.mock_file));
        break;
      case DeviceType::PICO_HARP_T2_MOCK_PTU:
        device_ =
            std::make_unique<DeviceProcessor>(std::make_unique<MockPicoHarpT2>(config.mock_file));
        break;
      case DeviceType::PICO_HARP_T3_MOCK_PTU:
        device_ =
            std::make_unique<DeviceProcessor>(std::make_unique<MockPicoHarpT3>(config.mock_file));
        break;
      case DeviceType::MULTI_HARP_T2_MOCK_PTU:
        device_ =
            std::make_unique<DeviceProcessor>(std::make_unique<MockMultiHarpT2>(config.mock_file));
        break;
      case DeviceType::MULTI_HARP_T3_MOCK_PTU:
        device_ =
            std::make_unique<DeviceProcessor>(std::make_unique<MockMultiHarpT3>(config.mock_file));
        break;
      case DeviceType::TIMETAGGER_MOCK_HDF5:
        device_ = std::make_unique<DeviceProcessor>(
            std::make_unique<HDF5MockTimetagger>(config.mock_file));
        break;
      case DeviceType::HISTOGRAMMER_MOCK_HDF5:
        device_ = std::make_unique<DeviceProcessor>(
            std::make_unique<HDF5MockHistogrammer>(config.mock_file));
        break;
      case DeviceType::BASLER_A2:
        device_ = std::make_unique<BaslerA2>();
        break;
      case DeviceType::CAMERA_MOCK_HDF5:
        device_ = std::make_unique<HDF5MockCamera>(config.mock_file);
        break;
      case DeviceType::ORCA_FUSION:
        device_ = std::make_unique<OrcaFusion>();
        break;
      case DeviceType::BASLER_ACA2000_CL:
        device_ = BaslerAca2000CL::Create();
        break;
      case DeviceType::BASLER_BOOST:
        device_ = BaslerBoost::Create();
        break;
      default:
        throw std::runtime_error("Pando::Configure: Unknown device type specified!");
        break;
    }
  }

  // Connect to Pandoboxd
  if (config.use_pandoboxd) {
    if (!pandoboxd_client_) {
      // Connect to Pandoboxd RPC server
      pandoboxd_client_ = std::make_unique<PandoboxdClient>(config.pandoboxd_ip_addr);

      // Connect to Pandoboxd ZMQ PUB endpont
      proxy_.Stop();
      // Remove any previously made connection
      if (pandoboxd_pub_ep_.size()) {
        proxy_.DisconnectIngress(pandoboxd_pub_ep_.c_str());
      }
      // Make new connection
      pandoboxd_pub_ep_ =
          fmt::format("tcp://{}:{}", config.pandoboxd_ip_addr, PandoboxdClient::kPandoboxdPubPort);
      proxy_.ConnectIngress(pandoboxd_pub_ep_.c_str());
      proxy_.Start();
    } else {
    }
  } else {
    pandoboxd_session_handle_.reset();
    pandoboxd_client_.reset();
  }

  config_ = config;
  g_reporter->trace("Config updated");
} catch (...) {
  common::LogException();
}

Pando::Config Pando::GetConfig() try {
  std::lock_guard<std::mutex> l(config_lock_);
  return config_;
} catch (...) {
  common::LogException();
}

bool Pando::IsHealthy() {
  return experiment_monitor_->GetCrashedThreadCount() == 0;
}

void Pando::StartImpl() try {
  auto config = GetConfig();

  experiment_monitor_->Reset();

  std::string base_name = "sawyer_log";

  // Generate log file names
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss_datetime;
  oss_datetime << std::put_time(&tm, "%Y-%m-%d_%H%M%S");
  auto str_datetime = oss_datetime.str();
  auto raw_file_name =
      fmt::format("{}/sawyer_raw_{}_{}.h5", config.output_dir, str_datetime, GetProxyEgressPort());
  auto analyzed_file_name = fmt::format(
      "{}/sawyer_analyzed_{}_{}.h5", config.output_dir, str_datetime, GetProxyEgressPort());
  auto peripheral_file_name = fmt::format(
      "{}/sawyer_peripheral_{}_{}.h5", config.output_dir, str_datetime, GetProxyEgressPort());

  auto raw_fname_cstr = raw_file_name.c_str();
  auto analyzed_fname_cstr = analyzed_file_name.c_str();
  auto peripheral_fname_cstr = peripheral_file_name.c_str();
  if (!config.log_analyzed_data) {
    analyzed_fname_cstr = nullptr;
  }
  if (!config.log_peripheral_data) {
    peripheral_fname_cstr = nullptr;
  }

  hdf5_logger_ = std::make_unique<HDF5Logger>(analyzed_fname_cstr, peripheral_fname_cstr);
  hdf5_logger_->LogConfig();

  // start up the logging thread and make sure it has initialized before
  // continuing
  std::unique_lock<std::mutex> sl(start_lock_);
  logger_running_ = false;
  logger_thread_ = common::ThreadContainer(
      [&] { Log(); },
      "Pando::Log",
      &logger_stop_signal_,
      common::thread_prio::ThreadPriority::kAboveNormal);
  start_cv_.wait(sl, [&] { return logger_running_; });
  sl.unlock();

  auto this_experiment = next_experiment_++;

  if (device_) {
    device_session_handle_ = device_->Start(this_experiment, config, raw_fname_cstr);
  } else {
    g_reporter->warn("Pando started, but no device has been constructed yet");
  }

  if (pandoboxd_client_) {
    pandoboxd_session_handle_ = pandoboxd_client_->Start(this_experiment, config);
  }
} catch (...) {
  common::LogException();
}

void Pando::StopImpl() try {
  device_session_handle_.reset();
  pandoboxd_session_handle_.reset();
  logger_thread_.Stop();
  logger_thread_.Join();
  hdf5_logger_.reset();
} catch (...) {
  common::LogException();
}

void Pando::Log() {
  auto context = common::SingletonContext::Get();
  zmq::socket_t sub_socket{*context, zmq::socket_type::sub};
  sub_socket.connect(kProxyEgressInprocEp);

  auto config = GetConfig();
  // Subscribe topics
  if (config.log_peripheral_data) {
    for (const char* topic : kPandoboxdTopicsToLog) {
      // Topic string includes terminating null character to avoid subscribing to prefixes
      size_t topic_size = std::strlen(topic) + 1;
      sub_socket.setsockopt(ZMQ_SUBSCRIBE, topic, topic_size);
    }
  }
  if (config.log_analyzed_data) {
    for (const char* topic : kAnalyzedTopicsToLog) {
      // Topic string includes terminating null character to avoid subscribing to prefixes
      size_t topic_size = std::strlen(topic) + 1;
      sub_socket.setsockopt(ZMQ_SUBSCRIBE, topic, topic_size);
    }
  }
  zmq::pollitem_t pi = {sub_socket, 0, ZMQ_POLLIN, 0};

  Worker log_packet_worker{"HDF5Logger::LogPacket"};
  BlockingFuture<void> log_packet_worker_done;
  zmq::message_t topic_msg, payload_msg;
  proto::Packet packet_pb_deserialize, packet_pb_log; // ping pong

  // now that we've subscribed to the topics
  // signal that it is safe for the other threads
  // to start logging
  std::unique_lock<std::mutex> sl(start_lock_);
  logger_running_ = true;
  sl.unlock();
  start_cv_.notify_all();

  while (zmq::poll(&pi, 1, 100) || !logger_stop_signal_.ShouldStop()) {
    if (pi.revents & ZMQ_POLLIN) {
      sub_socket.recv(&topic_msg);
      if (topic_msg.more()) {
        sub_socket.recv(&payload_msg);
        if (payload_msg.more()) {
          throw std::runtime_error("ZMQ message had too many segments");
        }
      } else {
        throw std::runtime_error("ZMQ message had too few segments");
      }

      std::string topic(topic_msg.data<const char>(), topic_msg.size());

      common::ProtobufDeserialize(payload_msg, packet_pb_deserialize);
      assert(packet_pb_deserialize.has_header());
      assert(packet_pb_deserialize.has_payload());

      // Wait for previous iteration to complete
      if (log_packet_worker_done.valid()) {
        log_packet_worker_done.get();
      }

      std::swap(packet_pb_deserialize, packet_pb_log);
      log_packet_worker_done = log_packet_worker.Async(
          &HDF5Logger::LogPacket, hdf5_logger_.get(), std::cref(packet_pb_log));
    }
  }
}

Pando::~Pando() try {
  common::Reporter::DestroyZmqSink();
  g_reporter->trace("Pando destructed");
} catch (...) {
  common::LogException();
}

std::shared_ptr<PandoInterface> SingletonPando::Get() {
  while (true) {
    std::shared_ptr<PandoInterface> pando_sp = pando_wp_.lock();

    if (pando_sp) {
      // An instance already exists, return it
      return pando_sp;
    } else if (auto l = std::unique_lock<std::mutex>(creation_mutex_, std::try_to_lock)) {
      // No instance exists, make a new one and return it
      struct MakeSharedEnabler : Pando {}; // Expose Pando's private constructor
      pando_sp = std::make_shared<MakeSharedEnabler>();
      pando_wp_ = pando_sp;
      return pando_sp;
    } else {
      // No instance exists but another thread is already making one. Wait.
      std::this_thread::yield();
    }
  }
}

std::mutex SingletonPando::creation_mutex_;
std::weak_ptr<PandoInterface> SingletonPando::pando_wp_;

void LogVersion() {
  g_reporter->info("Pando Version: {}", JULIET_VERSION_STRING);
}

} // namespace pando
} // namespace pnd
