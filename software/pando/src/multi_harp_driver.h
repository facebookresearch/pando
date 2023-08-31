#pragma once

#include "multi_harp.h"

extern "C" {
#include <multiharp-mhlib/errorcodes.h>
#include <multiharp-mhlib/mhdefin.h>
#include <multiharp-mhlib/mhlib.h>
}

#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

/** A class encapsulating our use of the MultiHarp programming library, MHLib. */
class RealMultiHarpBase::Driver {
 public:
  /** Exception raised when an MHLib call returns nonzero */
  class MHLibException : public std::exception {
   public:
    MHLibException(int err_code) : err_code_(err_code) {
      MH_GetErrorString(err_string_, err_code_);
    }
    const char* what() const {
      return err_string_;
    }

    bool operator==(const MHLibException& other) const {
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
    CallMHLib(MH_OpenDevice, dev_idx_, serial_number_);
  }

  ~Driver() {
    CallMHLib(MH_CloseDevice, dev_idx_);
  }

  /** Factory function. Tries to open devices with indices 0-7, in order, returning the first one
   * that succeeds. */
  static std::unique_ptr<Driver> OpenFirstDevice() {
    for (int dev_idx = 0; dev_idx < MAXDEVNUM; ++dev_idx) {
      try {
        return std::make_unique<Driver>(dev_idx);
      } catch (const MHLibException& e) {
        if (e == MHLibException(MH_ERROR_DEVICE_OPEN_FAIL)) {
          continue;
        }
        // Error code wasn't MH_ERROR_DEVICE_OPEN_FAIL, rethrow.
        throw;
      }
    }

    throw std::runtime_error("RealMultiHarpBase::Driver::OpenFirstDevice: No devices found.");
  }

  std::string GetLibraryVersion() {
    char version[8];
    CallMHLib(MH_GetLibraryVersion, version);
    return std::string(version);
  }

  void Initialize(int mode = MODE_T2, bool external_ref = false) {
    CallMHLib(MH_Initialize, dev_idx_, mode, external_ref);
  }

  HardwareInfo GetHardwareInfo() {
    // MH_GetHardwareInfo takes pointers to char arrays of these sizes
    // following sample code as source of truth (it varies from manual)
    char model[32];
    char partno[8];
    char version[16];

    CallMHLib(MH_GetHardwareInfo, dev_idx_, model, partno, version);

    return HardwareInfo{model, partno, version};
  }

  int GetFeatures() {
    int features;
    CallMHLib(MH_GetFeatures, dev_idx_, &features);
    return features;
  }

  std::string GetSerialNumber() {
    char serial[8];
    CallMHLib(MH_GetSerialNumber, dev_idx_, serial);
    return std::string(serial);
  }

  BaseResolution GetBaseResolution() {
    BaseResolution b;
    CallMHLib(MH_GetBaseResolution, dev_idx_, &b.resolution, &b.bin_steps);
    return b;
  }

  int GetNumOfInputChannels() {
    int n_channels;
    CallMHLib(MH_GetNumOfInputChannels, dev_idx_, &n_channels);
    return n_channels;
  }

  int GetNumOfModules() {
    int n_modules;
    CallMHLib(MH_GetNumOfModules, dev_idx_, &n_modules);
    return n_modules;
  }

  ModuleInfo GetModuleInfo(int mod_idx) {
    ModuleInfo m;
    CallMHLib(MH_GetModuleInfo, dev_idx_, mod_idx, &m.model_code, &m.version_code);
    return m;
  }

  void SetSyncDiv(int div) {
    CallMHLib(MH_SetSyncDiv, dev_idx_, div);
  }

  void SetSyncEdgeTrg(int level, int edge) {
    CallMHLib(MH_SetSyncEdgeTrg, dev_idx_, level, edge);
  }

  void SetSyncDeadTime(int on, int deadtime) {
    CallMHLib(MH_SetSyncDeadTime, dev_idx_, on, deadtime);
  }

  void SetSyncChannelOffset(int value) {
    CallMHLib(MH_SetSyncChannelOffset, dev_idx_, value);
  }

  void SetInputEdgeTrg(int channel, int level, int edge) {
    CallMHLib(MH_SetInputEdgeTrg, dev_idx_, channel, level, edge);
  }

  void SetInputDeadTime(int channel, int on, int deadtime) {
    CallMHLib(MH_SetInputDeadTime, dev_idx_, channel, on, deadtime);
  }

  void SetInputChannelOffset(int channel, int value) {
    CallMHLib(MH_SetInputChannelOffset, dev_idx_, channel, value);
  }

  void SetInputChannelEnable(int channel, bool enable) {
    CallMHLib(MH_SetInputChannelEnable, dev_idx_, channel, enable);
  }

  void SetStopOverflow(bool stop, unsigned int count) {
    CallMHLib(MH_SetStopOverflow, dev_idx_, stop, count);
  }

  void SetBinning(int binning) {
    CallMHLib(MH_SetBinning, dev_idx_, binning);
  }

  void SetOffset(int offset) {
    CallMHLib(MH_SetOffset, dev_idx_, offset);
  }

  int SetHistoLen(int lencode) {
    int actual_len;
    CallMHLib(MH_SetHistoLen, dev_idx_, lencode, &actual_len);
    return actual_len;
  }

  void ClearHistMem() {
    CallMHLib(MH_ClearHistMem, dev_idx_);
  }

  void SetMeasControl(int meas_control, bool rising_start, int rising_stop) {
    CallMHLib(MH_SetMeasControl, dev_idx_, meas_control, rising_start, rising_stop);
  }

  void StartMeas(int tacq) {
    CallMHLib(MH_StartMeas, dev_idx_, tacq);
  }

  void StopMeas() {
    CallMHLib(MH_StopMeas, dev_idx_);
  }

  bool CTCStatus() {
    int status;
    CallMHLib(MH_CTCStatus, dev_idx_, &status);
    return status;
  }

  void GetHistogram(unsigned int* counts, int channel) {
    CallMHLib(MH_GetHistogram, dev_idx_, counts, channel);
  }

  double GetResolution() {
    double resolution;
    CallMHLib(MH_GetResolution, dev_idx_, &resolution);
    return resolution;
  }

  int GetSyncRate() {
    int sync_rate;
    CallMHLib(MH_GetSyncRate, dev_idx_, &sync_rate);
    return sync_rate;
  }

  int GetCountRate(int channel) {
    int count_rate;
    CallMHLib(MH_GetCountRate, dev_idx_, channel, &count_rate);
    return count_rate;
  }

  int GetFlags() {
    int flags;
    CallMHLib(MH_GetFlags, dev_idx_, &flags);
    return flags;
  }

  double GetElapsedMeasTime() {
    double elapsed;
    CallMHLib(MH_GetElapsedMeasTime, dev_idx_, &elapsed);
    return elapsed;
  }

  int GetWarnings() {
    int warnings;
    CallMHLib(MH_GetWarnings, dev_idx_, &warnings);
    return warnings;
  }

  std::string GetWarningsText(int warnings) {
    // MH_GetWarningsText takes "pointer to a buffer for at least 16384 characters"
    auto text = std::make_unique<char[]>(16384);
    CallMHLib(MH_GetWarningsText, dev_idx_, text.get(), warnings);

    return std::string(text.get());
  }

  double GetSyncPeriod() {
    double period;
    CallMHLib(MH_GetSyncPeriod, dev_idx_, &period);
    return period;
  }

  int ReadFifo(uint32_t* buffer) {
    int n_actual;
    CallMHLib(MH_ReadFiFo, dev_idx_, buffer, &n_actual);
    return n_actual;
  }

  void SetMarkerEdges(bool rising_0, bool rising_1, bool rising_2, bool rising_3) {
    CallMHLib(MH_SetMarkerEdges, dev_idx_, rising_0, rising_1, rising_2, rising_3);
  }

  void SetMarkerEnable(bool en_0, bool en_1, bool en_2, bool en_3) {
    CallMHLib(MH_SetMarkerEnable, dev_idx_, en_0, en_1, en_2, en_3);
  }

  void SetMarkerHoldoffTime(int holdoff_time) {
    CallMHLib(MH_SetMarkerHoldoffTime, dev_idx_, holdoff_time);
  }

  /** The serial number returned by MH_OpenDevice during construction. */
  char serial_number_[8];

  const int dev_idx_;

 private:
  /** Wrapper for calling MHLib functions. */
  template <class... Params, class... Args>
  inline void CallMHLib(int (*f)(Params...), Args&&... args) {
    int ret = f(std::forward<Args>(args)...);
    if (ret < 0) {
      throw MHLibException(ret);
    }
  }
};

} // namespace pando
} // namespace pnd
