#pragma once

#include "histogram.h"
#include "kv_container.h"
#include "pando.h"
#include "session_manager.h"
#include "types.h"

#include <chrono>
#include <utility>

namespace pnd {
namespace pando {

/** Base class for a device that produces "raw data".
 * The raw data can be either single photon timestamps or binned photon counts ("Histograms").
 * The interface defined here is used by DeviceProcessor to manage any of our myriad of device
 * classes that produce this type of data.
 */
class DeviceBase : public common::SessionManager<PandoInterface::Config> {
 public:
  /** Return type for GetDeviceType. */
  enum class DeviceType {
    TIMETAGGER, /**< UpdateRawData implementation populates RawData::timestamps. */
    HISTOGRAMMER, /**< UpdateRawData implementation populates RawData::histograms. */
  };

  /** Type of the destination for UpdateRawData. */
  struct RawData {
    KVContainer<int32_t, ChannelTimestamps> timestamps;
    KVContainer<int32_t, Histogram<std::chrono::nanoseconds>> histograms;
    KVContainer<int32_t, ChannelTimestamps> marker_timestamps;
  };

  /** Create an instance of RawData suitable for immediate use with UpdateRawData. */
  RawData CreateDest();

  /** Shall populate dest with raw data cooresponding to the specified time interval.
   *
   * DeviceProcessor calls this method periodically and asynchronously to consume raw data produced
   * by the device.
   *
   * If GetDeviceType returns TIMETAGGER, the implementation shall populate the timestamps field of
   * dest. The macrotimes stored into dest shall be sorted in increasing order on a per-channel
   * basis, as that is a requirement of DeviceProcessor. If GetDeviceType returns HISTOGRAMMER, the
   * implementation shall populate the histograms field of dest. In either case, the implementation
   * may populate the marker_timestamps field of dest.
   *
   * The time interval is specified as a range of "bin indices", which are in terms of the current
   * value of config_.bin_size_ns. The range includes begin_bin_idx but excludes end_bin_idx. The
   * bin indices may be converted to a range of macrotimes (which is likewise inclusive of its
   * beginning and exclusive of its end) like so:
   * std::chrono::nanoseconds from{config_.bin_size_ns * begin_bin_idx};
   * std::chrono::nanoseconds until{config_.bin_size_ns * end_bin_idx};
   *
   * It's expected that the implementation will need to block until the
   * specified time interval has actually elapsed, which is the only intentional mechanism by which
   * processing speed is limited.
   *
   * It's acceptable for the implementation to do some processing synchronously, so long as that
   * processing takes (on average) less time than the specified time interval. DeviceProcessor calls
   * this method in a dedicated thread for this reason.
   *
   * If consumption of raw data falls significantly behind real-time (by several seconds, suppose),
   * the implementation should eventually throw an exception indicating that "processing is too
   * slow", or similar. Alternatively, the exception may be thrown in a worker thread inside of an
   * instance of common::ThreadContainer.
   *
   * The implementation may expect the caller to make the following guarantees:
   *   1. An acquisition is currently in progress (StartAcquisition ran, StopAcquisition hasn't
   *      yet).
   *   1. The very first begin_bin_idx argument of a given acquisition is always 0
   *   2. Each subsequent begin_bin_idx argument is equal to the end_bin_idx argument of the
   *      previous call (That is, the specified time intervals are contiguous)
   *   3. The channel indices present in the supplied dest argument are in agreement with the
   *      contents of config_.enabled_channels (though KVContainer guarantees that they have been
   *      sorted and deduplicated.)
   */
  virtual void UpdateRawData(size_t begin_bin_idx, size_t end_bin_idx, RawData& dest) = 0;

  /** Shall indicate which kind of raw data the device produces. */
  virtual DeviceType GetDeviceType() = 0;

  /** Shall return the native resolution (in picoseconds) of microtimes on this device.
   * Only applicable if DeviceGeneratesMicrotimes returns true.
   * Used by DeviceProcessor to determine DTOF bin width.
   */
  virtual uint64_t GetMicrotimeResolutionPs() {
    return 1;
  }

  /** Shall return true if the device generates microtimes, or false if it does not.
   * Only applicable if GetDeviceType returns TIMETAGGER.
   * For example, a PicoQuant device in T2 mode produces macrotimes but not microtimes, so that
   * implementation should return false.
   */
  virtual bool DeviceGeneratesMicrotimes() {
    return false;
  }

 protected:
  /** Shall make preparations for the first call to UpdateRawData.
   * Reset internal state, initialize hardware based on config_, launch acquisition thread, etc.
   */
  virtual void StartAcquisition() = 0;

  /** Shall perform any work necessary to return to an "Idle" state.
   * Deinitialize hardware, stop & join acquisition thread, etc.
   */
  virtual void StopAcquisition() = 0;

  PandoInterface::Config config_;

 private:
  void StartImpl(PandoInterface::Config config) final {
    config_ = std::move(config);
    StartAcquisition();
  }

  void StopImpl() final {
    StopAcquisition();
  }
};
} // namespace pando
} // namespace pnd
