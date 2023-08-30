#pragma once

#include "hydra_harp.h"

extern "C" {
#include <hydraharp-hhlib/errorcodes.h>
#include <hydraharp-hhlib/hhdefin.h>
#include <hydraharp-hhlib/hhlib.h>
}

#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

/** A class encapsulating our use of the HydraHarp programming library, HHLib. */
class RealHydraHarpBase::Driver {
 public:
  /** Exception raised when an HHLib call returns nonzero */
  class HHLibException : public std::exception {
   public:
    HHLibException(int err_code) : err_code_(err_code) {
      HH_GetErrorString(err_string_, err_code_);
    }
    const char* what() const {
      return err_string_;
    }

    bool operator==(const HHLibException& other) const {
      return err_code_ == other.err_code_;
    }

   private:
    const int err_code_;
    char err_string_[40];
  };

  /** Return type of Driver::GetHardwareInfo. */
  struct HardwareInfo {
    std::string model;
    std::string partno;
    std::string version;
  };

  /** Return type of Driver::GetBaseResolution. */
  struct BaseResolution {
    double resolution;
    int bin_steps;
  };

  /** Return type of Driver::GetModuleInfo. */
  struct ModuleInfo {
    int model_code;
    int version_code;
  };

  Driver(int dev_idx = 0) : dev_idx_(dev_idx) {
    CallHHLib(HH_OpenDevice, dev_idx_, serial_number_);
  }

  ~Driver() {
    CallHHLib(HH_CloseDevice, dev_idx_);
  }

  /** Factory function. Tries to open devices with indices 0-7, in order, returning the first one
   * that succeeds. */
  static std::unique_ptr<Driver> OpenFirstDevice() {
    for (int dev_idx = 0; dev_idx < MAXDEVNUM; ++dev_idx) {
      try {
        return std::make_unique<Driver>(dev_idx);
      } catch (const HHLibException& e) {
        if (e == HHLibException(HH_ERROR_DEVICE_OPEN_FAIL)) {
          continue;
        }

        // Error code wasn't HH_ERROR_DEVICE_OPEN_FAIL, rethrow.
        throw;
      }
    }

    throw std::runtime_error("RealHydraHarpBase::Driver::OpenFirstDevice: No devices found.");
  }

  std::string GetLibraryVersion() {
    char version[8];
    CallHHLib(HH_GetLibraryVersion, version);
    return std::string(version);
  }

  void Initialize(int mode = MODE_T2, bool external_ref = false) {
    CallHHLib(HH_Initialize, dev_idx_, mode, external_ref);
  }

  HardwareInfo GetHardwareInfo() {
    // HH_GetHardwareInfo takes pointers to char arrays of these sizes
    char model[16];
    char partno[8];
    char version[8];

    CallHHLib(HH_GetHardwareInfo, dev_idx_, model, partno, version);

    return HardwareInfo{model, partno, version};
  }

  int GetFeatures() {
    int features;
    CallHHLib(HH_GetFeatures, dev_idx_, &features);
    return features;
  }

  std::string GetSerialNumber() {
    char serial[8];
    CallHHLib(HH_GetSerialNumber, dev_idx_, serial);
    return std::string(serial);
  }

  BaseResolution GetBaseResolution() {
    BaseResolution b;
    CallHHLib(HH_GetBaseResolution, dev_idx_, &b.resolution, &b.bin_steps);
    return b;
  }

  int GetNumOfInputChannels() {
    int n_channels;
    CallHHLib(HH_GetNumOfInputChannels, dev_idx_, &n_channels);
    return n_channels;
  }

  int GetNumOfModules() {
    int n_modules;
    CallHHLib(HH_GetNumOfModules, dev_idx_, &n_modules);
    return n_modules;
  }

  ModuleInfo GetModuleInfo(int mod_idx) {
    ModuleInfo m;
    CallHHLib(HH_GetModuleInfo, dev_idx_, mod_idx, &m.model_code, &m.version_code);
    return m;
  }

  int GetModuleIndex(int channel) {
    int mod_idx;
    CallHHLib(HH_GetModuleIndex, dev_idx_, channel, &mod_idx);
    return mod_idx;
  }

  std::string GetHardwareDebugInfo() {
    // HH_GetHardwareDebugInfo takes a "pointer to a buffer for at least 65536 characters"
    auto debug_info = std::make_unique<char[]>(65536);
    CallHHLib(HH_GetHardwareDebugInfo, dev_idx_, debug_info.get());

    return std::string(debug_info.get());
  }

  void Calibrate() {
    CallHHLib(HH_Calibrate, dev_idx_);
  }

  void SetSyncDiv(int div) {
    CallHHLib(HH_SetSyncDiv, dev_idx_, div);
  }

  void SetSyncCFD(int level, int zero_crossing) {
    CallHHLib(HH_SetSyncCFD, dev_idx_, level, zero_crossing);
  }

  void SetSyncChannelOffset(int value) {
    CallHHLib(HH_SetSyncChannelOffset, dev_idx_, value);
  }

  void SetInputCFD(int channel, int level, int zero_crossing) {
    CallHHLib(HH_SetInputCFD, dev_idx_, channel, level, zero_crossing);
  }

  void SetInputChannelOffset(int channel, int value) {
    CallHHLib(HH_SetInputChannelOffset, dev_idx_, channel, value);
  }

  void SetInputChannelEnable(int channel, bool enable) {
    CallHHLib(HH_SetInputChannelEnable, dev_idx_, channel, enable);
  }

  void SetStopOverflow(bool stop, unsigned int count) {
    CallHHLib(HH_SetStopOverflow, dev_idx_, stop, count);
  }

  void SetBinning(int binning) {
    CallHHLib(HH_SetBinning, dev_idx_, binning);
  }

  void SetOffset(int offset) {
    CallHHLib(HH_SetOffset, dev_idx_, offset);
  }

  int SetHistoLen(int lencode) {
    int actual_len;
    CallHHLib(HH_SetHistoLen, dev_idx_, lencode, &actual_len);
    return actual_len;
  }

  void ClearHistMem() {
    CallHHLib(HH_ClearHistMem, dev_idx_);
  }

  void SetMeasControl(int meas_control, bool rising_start, int rising_stop) {
    CallHHLib(HH_SetMeasControl, dev_idx_, meas_control, rising_start, rising_stop);
  }

  void StartMeas(int tacq) {
    CallHHLib(HH_StartMeas, dev_idx_, tacq);
  }

  void StopMeas() {
    CallHHLib(HH_StopMeas, dev_idx_);
  }

  bool CTCStatus() {
    int status;
    CallHHLib(HH_CTCStatus, dev_idx_, &status);
    return status;
  }

  void GetHistogram(unsigned int* counts, int channel, bool clear) {
    CallHHLib(HH_GetHistogram, dev_idx_, counts, channel, clear);
  }

  double GetResolution() {
    double resolution;
    CallHHLib(HH_GetResolution, dev_idx_, &resolution);
    return resolution;
  }

  int GetSyncRate() {
    int sync_rate;
    CallHHLib(HH_GetSyncRate, dev_idx_, &sync_rate);
    return sync_rate;
  }

  int GetCountRate(int channel) {
    int count_rate;
    CallHHLib(HH_GetCountRate, dev_idx_, channel, &count_rate);
    return count_rate;
  }

  int GetFlags() {
    int flags;
    CallHHLib(HH_GetFlags, dev_idx_, &flags);
    return flags;
  }

  double GetElapsedMeasTime() {
    double elapsed;
    CallHHLib(HH_GetElapsedMeasTime, dev_idx_, &elapsed);
    return elapsed;
  }

  int GetWarnings() {
    int warnings;
    CallHHLib(HH_GetWarnings, dev_idx_, &warnings);
    return warnings;
  }

  std::string GetWarningsText(int warnings) {
    // HH_GetWarningsText takes "pointer to a buffer for at least 16384 characters"
    auto text = std::make_unique<char[]>(16384);
    CallHHLib(HH_GetWarningsText, dev_idx_, text.get(), warnings);

    return std::string(text.get());
  }

  double GetSyncPeriod() {
    double period;
    CallHHLib(HH_GetSyncPeriod, dev_idx_, &period);
    return period;
  }

  int ReadFifo(uint32_t* buffer, int count) {
    int n_actual;
    CallHHLib(HH_ReadFiFo, dev_idx_, buffer, count, &n_actual);
    return n_actual;
  }

  void SetMarkerEdges(bool rising_0, bool rising_1, bool rising_2, bool rising_3) {
    CallHHLib(HH_SetMarkerEdges, dev_idx_, rising_0, rising_1, rising_2, rising_3);
  }

  void SetMarkerEnable(bool en_0, bool en_1, bool en_2, bool en_3) {
    CallHHLib(HH_SetMarkerEnable, dev_idx_, en_0, en_1, en_2, en_3);
  }

  void SetMarkerHoldoffTime(int holdoff_time) {
    CallHHLib(HH_SetMarkerHoldoffTime, dev_idx_, holdoff_time);
  }

  int GetContModeBlock(void* buffer) {
    int n_bytes_received;
    CallHHLib(HH_GetContModeBlock, dev_idx_, buffer, &n_bytes_received);
    return n_bytes_received;
  }

  /** The serial number returned by HH_OpenDevice during construction. */
  char serial_number_[8];

  const int dev_idx_;

 private:
  /** Wrapper for calling HHLib functions. */
  template <class... Params, class... Args>
  inline void CallHHLib(int (*f)(Params...), Args&&... args) {
    int ret = f(std::forward<Args>(args)...);
    if (ret < 0) {
      throw HHLibException(ret);
    }
  }
};

} // namespace pando
} // namespace pnd
