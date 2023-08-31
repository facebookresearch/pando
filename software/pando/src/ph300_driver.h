#pragma once

#include "pico_harp.h"

extern "C" {
#include <ph300-phlib/errorcodes.h>
#include <ph300-phlib/phdefin.h>
#include <ph300-phlib/phlib.h>
}

#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

namespace pnd {
namespace pando {

/** A class encapsulating our use of the PH300 programming library, PHLib. */
class RealPicoHarpBase::Driver {
 public:
  /** Exception raised when an PHLib call returns nonzero */
  class PHLibException : public std::exception {
   public:
    PHLibException(int err_code) : err_code_(err_code) {
      PH_GetErrorString(err_string_, err_code_);
    }
    const char* what() const {
      return err_string_;
    }

    bool operator==(const PHLibException& other) const {
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
    CallPHLib(PH_OpenDevice, dev_idx_, serial_number_);
  }

  ~Driver() {
    CallPHLib(PH_CloseDevice, dev_idx_);
  }

  /** Factory function. Tries to open devices with indices 0-7, in order, returning the first one
   * that succeeds. */
  static std::unique_ptr<Driver> OpenFirstDevice() {
    for (int dev_idx = 0; dev_idx < MAXDEVNUM; ++dev_idx) {
      try {
        return std::make_unique<Driver>(dev_idx);
      } catch (const PHLibException& e) {
        if (e == PHLibException(ERROR_DEVICE_OPEN_FAIL)) {
          continue;
        }

        // Error code wasn't ERROR_DEVICE_OPEN_FAIL, rethrow.
        throw;
      }
    }

    throw std::runtime_error("RealPicoHarpBase::Driver::OpenFirstDevice: No devices found.");
  }

  std::string GetLibraryVersion() {
    char version[8];
    CallPHLib(PH_GetLibraryVersion, version);
    return std::string(version);
  }

  void Initialize(int mode = MODE_T2) {
    CallPHLib(PH_Initialize, dev_idx_, mode);
  }

  HardwareInfo GetHardwareInfo() {
    // PH_GetHardwareInfo takes pointers to char arrays of these sizes
    char model[16];
    char partno[8];
    char version[8];

    CallPHLib(PH_GetHardwareInfo, dev_idx_, model, partno, version);

    return HardwareInfo{model, partno, version};
  }

  std::string GetSerialNumber() {
    char serial[8];
    CallPHLib(PH_GetSerialNumber, dev_idx_, serial);
    return std::string(serial);
  }

  BaseResolution GetBaseResolution() {
    BaseResolution b;
    // TODO ph300-phlib api manual doesn't mention bin_steps, but it's present in the  prototype in
    // phlib.h. What's up with that?
    CallPHLib(PH_GetBaseResolution, dev_idx_, &b.resolution, &b.bin_steps);
    return b;
  }

  int GetFeatures() {
    int features;
    CallPHLib(PH_GetFeatures, dev_idx_, &features);
    return features;
  }

  void Calibrate() {
    CallPHLib(PH_Calibrate, dev_idx_);
  }

  void SetInputCFD(int channel, int level, int zero_crossing) {
    CallPHLib(PH_SetInputCFD, dev_idx_, channel, level, zero_crossing);
  }

  void SetSyncDiv(int div) {
    CallPHLib(PH_SetSyncDiv, dev_idx_, div);
  }

  void SetSyncOffset(int offset) {
    CallPHLib(PH_SetSyncOffset, dev_idx_, offset);
  }

  void SetStopOverflow(bool stop, unsigned int count) {
    CallPHLib(PH_SetStopOverflow, dev_idx_, stop, count);
  }

  void SetBinning(int binning) {
    CallPHLib(PH_SetBinning, dev_idx_, binning);
  }

  void SetMultistopEnable(bool enable) {
    CallPHLib(PH_SetMultistopEnable, dev_idx_, enable);
  }

  void SetOffset(int offset) {
    CallPHLib(PH_SetOffset, dev_idx_, offset);
  }

  void ClearHistMem(int block) {
    CallPHLib(PH_ClearHistMem, dev_idx_, block);
  }

  void StartMeas(int tacq) {
    CallPHLib(PH_StartMeas, dev_idx_, tacq);
  }

  void StopMeas() {
    CallPHLib(PH_StopMeas, dev_idx_);
  }

  bool CTCStatus() {
    int status;
    CallPHLib(PH_CTCStatus, dev_idx_, &status);
    return status;
  }

  void GetHistogram(unsigned int* counts, int block) {
    CallPHLib(PH_GetHistogram, dev_idx_, counts, block);
  }

  double GetResolution() {
    double resolution;
    CallPHLib(PH_GetResolution, dev_idx_, &resolution);
    return resolution;
  }

  int GetCountRate(int channel) {
    int count_rate;
    CallPHLib(PH_GetCountRate, dev_idx_, channel, &count_rate);
    return count_rate;
  }

  int GetFlags() {
    int flags;
    CallPHLib(PH_GetFlags, dev_idx_, &flags);
    return flags;
  }

  double GetElapsedMeasTime() {
    double elapsed;
    CallPHLib(PH_GetElapsedMeasTime, dev_idx_, &elapsed);
    return elapsed;
  }

  int GetWarnings() {
    int warnings;
    CallPHLib(PH_GetWarnings, dev_idx_, &warnings);
    return warnings;
  }

  std::string GetWarningsText(int warnings) {
    // PH_GetWarningsText takes "pointer to a buffer for at least 16384 characters"
    auto text = std::make_unique<char[]>(16384);
    CallPHLib(PH_GetWarningsText, dev_idx_, text.get(), warnings);

    return std::string(text.get());
  }

  std::string GetHardwareDebugInfo() {
    // PH_GetHardwareDebugInfo takes a "pointer to a buffer for at least 16384 characters"
    auto debug_info = std::make_unique<char[]>(16384);
    CallPHLib(PH_GetHardwareDebugInfo, dev_idx_, debug_info.get());

    return std::string(debug_info.get());
  }

  int ReadFifo(uint32_t* buffer, int count) {
    int n_actual;
    CallPHLib(PH_ReadFiFo, dev_idx_, buffer, count, &n_actual);
    return n_actual;
  }

  void SetMarkerEdges(bool rising_0, bool rising_1, bool rising_2, bool rising_3) {
    CallPHLib(PH_SetMarkerEdges, dev_idx_, rising_0, rising_1, rising_2, rising_3);
  }

  void SetMarkerEnable(bool en_0, bool en_1, bool en_2, bool en_3) {
    CallPHLib(PH_SetMarkerEnable, dev_idx_, en_0, en_1, en_2, en_3);
  }

  void SetMarkerHoldoffTime(int holdoff_time) {
    CallPHLib(PH_SetMarkerHoldoffTime, dev_idx_, holdoff_time);
  }

  /** The serial number returned by PH_OpenDevice during construction. */
  char serial_number_[8];

  const int dev_idx_;

 private:
  /** Wrapper for calling PHLib functions. */
  template <class... Params, class... Args>
  inline void CallPHLib(int (*f)(Params...), Args&&... args) {
    int ret = f(std::forward<Args>(args)...);
    if (ret < 0) {
      throw PHLibException(ret);
    }
  }
};

} // namespace pando
} // namespace pnd
