#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace pnd {
namespace libpandobox {

namespace sample_format {

struct TrafficGen {
  uint16_t _reserved;
} __attribute__((__packed__));

struct AnalogIn {
  uint16_t value;
} __attribute__((__packed__));

struct Imu {
  uint16_t fsync;
  int16_t gyro_x;
  int16_t gyro_y;
  int16_t gyro_z;
  int16_t accel_x;
  int16_t accel_y;
  int16_t accel_z;
} __attribute__((__packed__));

struct DigitalIn {
  uint16_t direction;
} __attribute__((__packed__));

union PandoBoxData {
  DigitalIn digital;
  Imu imu;
  AnalogIn analog;
  TrafficGen traffic_gen;
} __attribute__((__packed__));

struct PandoBox {
  uint32_t type;
  uint32_t size;
  uint64_t timestamp;
  uint16_t device_id;
  PandoBoxData data;
} __attribute__((__packed__));

/** The number of nanoseconds per LSB in PandoBox::timestamp. */
constexpr uint32_t kSampleTimePeriodNs = 10;

enum class Magic {
  kDigitalInput = 0x53500000,
  kImu = 0x53500001,
  kAnalogInput = 0x53500002,
  kTrafficGen = 0x53500003,
};
} // namespace sample_format

struct PandoBoxInterface {
  virtual ~PandoBoxInterface() = default;

  virtual void SetRun(bool run) = 0;
  virtual uint32_t GetVersion() = 0;

  virtual void SetDin0Enabled(bool enabled) = 0;
  virtual void SetImu0Enabled(bool enabled) = 0;
  virtual void SetAin0Enabled(bool enabled) = 0;
  virtual void SetAin1Enabled(bool enabled) = 0;
  virtual void SetAin2Enabled(bool enabled) = 0;
  virtual void SetAin3Enabled(bool enabled) = 0;
  virtual void SetTrgen0Enabled(bool enabled) = 0;
  virtual void SetTrigGen0Enabled(bool enabled) = 0;
  virtual void SetTrigGen1Enabled(bool enabled) = 0;

  virtual void SetDin0Mocked(bool mock) = 0;
  virtual void SetImu0Mocked(bool mock) = 0;
  virtual void SetAin0Mocked(bool mock) = 0;
  virtual void SetAin1Mocked(bool mock) = 0;
  virtual void SetAin2Mocked(bool mock) = 0;
  virtual void SetAin3Mocked(bool mock) = 0;

  virtual void SetAin0SampRateDiv(uint32_t divisor) = 0;
  virtual void SetAin1SampRateDiv(uint32_t divisor) = 0;
  virtual void SetAin2SampRateDiv(uint32_t divisor) = 0;
  virtual void SetAin3SampRateDiv(uint32_t divisor) = 0;

  virtual void SetTrgen0SampRateDiv(uint32_t divisor) = 0;

  virtual void SetTrigGen0Period(uint32_t trigger_period) = 0;
  virtual void SetTrigGen1Period(uint32_t trigger_period) = 0;

  virtual void SetTrigGen0Width(uint32_t trigger_width) = 0;
  virtual void SetTrigGen1Width(uint32_t trigger_width) = 0;

  virtual void InitDma() = 0;
  virtual void StartDma() = 0;
  virtual void StopDma() = 0;
  virtual int Read(char* dest, unsigned int len, int timeout) = 0;

  /** Consume available samples in a zero-copy fashion.
   * @param consume Function that consumes a vector of pointers to samples
   * @param max_samples Maximum number of samples to consume per call
   * @param timeout Maximum amount of time (in milliseconds) to block while waiting for the first
   * sample
   * @return The number of samples that were consumed
   */
  virtual std::size_t ConsumeSamples(
      std::function<void(const std::vector<const sample_format::PandoBox*>& samples)> consume,
      unsigned int max_samples,
      int timeout) = 0;
};

} // namespace libpandobox
} // namespace pnd
