#pragma once

#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <stdexcept>
#include <thread>

#include <fmt/format.h>

#include "pf32_acquisition.h"
#include "pf32_acquisition_g2.h"
#include "pf32_ll_types.h"
#include "reporter.h"

namespace pnd {
namespace pando {
namespace pf32_ll {

/** Low level management class for the PF32.
 * Encapsulates our usage of the vendor library.
 */
class PF32LL {
 public:
  class PF32LLException : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  /** Open the device. */
  PF32LL(const char* firmware_path) {
    // Route PF32 API log messages through our logging infrastructure
    api::setLogStreamLevel(api::LOGLEVEL_OFF);
    api::setLogCallback(&LogCallback);

    handle_ = Handle(api::PF32_constructWithCustomFirmware(firmware_path), api::PF32_destruct);

    // Generate log messages when status changes
    api::setStatusCallback(handle_.get(), &StatusCallback);

    // Wait for the device to become ready
    for (int sleep_cnt = 0;; sleep_cnt++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      auto status = api::getLinkStatus(handle_.get());

      if (status == api::ready) {
        g_reporter->debug("PF32LL: PF32 became ready after ~{}ms.", sleep_cnt * 100);
        break;
      } else if (status == api::error) {
        throw PF32LLException("PF32LL: Link status is \"error\"");
      } else if (sleep_cnt >= 20) {
        throw PF32LLException("PF32LL: waited >2sec for PF32 to become ready");
      }
    }

    // set non-default bias
    int VEB_setpoint = 1900;
    int VBD_setpoint = 3700 - (VEB_setpoint - 1200) / 4;
    int total_bias_setpoint = 4 * VBD_setpoint + VEB_setpoint;
    api::setDAC(handle_.get(), api::VBD_DAC, 0);
    api::setDAC(handle_.get(), api::VEB_DAC, VEB_setpoint);
    api::setDAC(handle_.get(), api::VBD_DAC, VBD_setpoint);

    // workaround for all-zero frames (bad defaults in fw)
    api::SetWireInValue(handle_.get(), kWindowHighThresholdKey, 0xffff, 0xffff);
    api::SetWireInValue(handle_.get(), kWindowLowThresholdKey, 0, 0xffff);
    api::UpdateWireIns(handle_.get());
  }

  template <typename BitMode>
  std::unique_ptr<Acquisition<BitMode>> CreateAcquisition(
      std::chrono::nanoseconds frame_period,
      api::PF32_mode mode,
      const std::list<int32_t>& enabled_channels) {
    return std::make_unique<Acquisition<BitMode>>(handle_, frame_period, mode, enabled_channels);
  }

  std::unique_ptr<AcquisitionG2> CreateAcquisitionG2(
      std::chrono::nanoseconds frame_period,
      unsigned long frame_count,
      bool burst_mode,
      std::array<int, 4> rebin_factors,
      const std::list<int32_t>& enabled_channels) {
    return std::make_unique<AcquisitionG2>(
        handle_, frame_period, frame_count, burst_mode, rebin_factors, enabled_channels);
  }

 private:
  static constexpr int kWindowHighThresholdKey = 0x16;
  static constexpr int kWindowLowThresholdKey = 0x17;

  /** Logging callback provided to vendor lib */
  static void LogCallback(
      int level,
      const unsigned short* component,
      int component_len,
      const unsigned short* msg,
      int msg_len) {
    // Translate from pf32 api level to spdlog level
    enum spdlog::level::level_enum spdlog_level;
    switch (level) {
      case api::LOGLEVEL_TRACE:
        spdlog_level = spdlog::level::trace;
        break;
      case api::LOGLEVEL_DEBUG:
        spdlog_level = spdlog::level::debug;
        break;
      case api::LOGLEVEL_INFO:
        // Demote info to debug
        spdlog_level = spdlog::level::debug;
        break;
      case api::LOGLEVEL_WARNING:
        spdlog_level = spdlog::level::warn;
        break;
      case api::LOGLEVEL_ERROR:
        spdlog_level = spdlog::level::err;
        break;
      case api::LOGLEVEL_OFF:
      default:
        spdlog_level = spdlog::level::off;
        break;
    }

    // Convert from wchar_t string to a char_t string usable by spdlog
    std::wstring_view w_component_sv(reinterpret_cast<const wchar_t*>(component));
    std::wstring_view w_msg_sv(reinterpret_cast<const wchar_t*>(msg));
    std::string component_str(w_component_sv.begin(), w_component_sv.end());
    std::string msg_str(w_msg_sv.begin(), w_msg_sv.end());
    g_reporter->log(spdlog_level, "PF32 API: [{}] {}", component_str, msg_str);
  }

  /** Logging callback provided to vendor lib */
  static void StatusCallback(int conn_status) {
    g_reporter->debug("PF32LL: Status changed to {}", conn_status);
  }

  Handle handle_;
};

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
