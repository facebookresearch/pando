#pragma once

#include <cstddef>
#include <cstdint>

#include "bitfield.h"

namespace pnd {
namespace libpandobox {

/** Classes capturing register layouts as specified in the PandoBox Internal Component Interfaces
 * document. */
namespace reg {

using Register = volatile uint32_t;

/** Common register layout that exists in multiple register banks. */
class CommonPeripheralFlags : BitField<Register> {
  static constexpr std::size_t kEnableOffset = 0;
  static constexpr std::size_t kEnableWidth = 1;
  static constexpr std::size_t kMockOffset = 1;
  static constexpr std::size_t kMockWidth = 1;

 public:
  BitField<Register>::NvStorage ReadEnable() {
    return ReadField<kEnableWidth, kEnableOffset>();
  }
  void WriteEnable(BitField<Register>::NvStorage value) {
    return WriteField<kEnableWidth, kEnableOffset>(value);
  }

  BitField<Register>::NvStorage ReadMock() {
    return ReadField<kMockWidth, kMockOffset>();
  }
  void WriteMock(BitField<Register>::NvStorage value) {
    return WriteField<kMockWidth, kMockOffset>(value);
  }
} __attribute__((__packed__));

/** Layout of Global Configuration register bank. */
struct Global {
  static constexpr char kUioDevName[] = "global_config_regs_0";

  class Flags0 : BitField<Register> {
    static constexpr std::size_t kRunOffset = 0;
    static constexpr std::size_t kRunWidth = 1;

   public:
    BitField<Register>::NvStorage ReadRun() {
      return ReadField<kRunWidth, kRunOffset>();
    }
    void WriteRun(BitField<Register>::NvStorage value) {
      return WriteField<kRunWidth, kRunOffset>(value);
    }

  } __attribute__((__packed__)) flags_0;

  uint32_t flags_1;
  uint32_t version;
} __attribute__((__packed__));

/** Layout of Digital Input Configuration register bank. */
struct DigitalInput {
  static constexpr char kUioDevNameInst0[] = "digital_inputs_0";

  CommonPeripheralFlags flags_0;
} __attribute__((__packed__));

/** Layout of IMU Configuration register bank. */
struct Imu {
  static constexpr char kUioDevNameInst0[] = "imu_0";

  CommonPeripheralFlags flags_0;
} __attribute__((__packed__));

/** Layout of Analog Input Configuration register bank. */
struct AnalogInput {
  static constexpr char kUioDevNameInst0[] = "adc_0";
  static constexpr char kUioDevNameInst1[] = "adc_1";
  static constexpr char kUioDevNameInst2[] = "adc_2";
  static constexpr char kUioDevNameInst3[] = "adc_3";

  CommonPeripheralFlags flags_0;
  uint32_t sample_rate_div;
} __attribute__((__packed__));

/** Layout of Traffic Generator Configuration register bank. */
struct TrafficGenerator {
  static constexpr char kUioDevNameInst0[] = "traffic_generator_0";

  class CommonPeripheralFlags : BitField<Register> {
    static constexpr std::size_t kEnableOffset = 0;
    static constexpr std::size_t kEnableWidth = 1;

   public:
    BitField<Register>::NvStorage ReadEnable() {
      return ReadField<kEnableWidth, kEnableOffset>();
    }
    void WriteEnable(BitField<Register>::NvStorage value) {
      return WriteField<kEnableWidth, kEnableOffset>(value);
    }

  } __attribute__((__packed__)) flags_0;
  uint32_t sample_rate_div;
} __attribute__((__packed__));

/** Layout of Trigger Generator Configuration register bank. */
struct TriggerGen {
  static constexpr char kUioDevNameInst0[] = "frame_trig_gen_0";
  static constexpr char kUioDevNameInst1[] = "frame_trig_gen_1";

  class Flags : BitField<Register> {
    static constexpr std::size_t kEnableOffset = 0;
    static constexpr std::size_t kEnableWidth = 1;

   public:
    BitField<Register>::NvStorage ReadEnable() {
      return ReadField<kEnableWidth, kEnableOffset>();
    }
    void WriteEnable(BitField<Register>::NvStorage value) {
      return WriteField<kEnableWidth, kEnableOffset>(value);
    }

  } __attribute__((__packed__)) flags_0;
  uint32_t trigger_period;
  uint32_t trigger_width;
} __attribute__((__packed__));

} // namespace reg
} // namespace libpandobox
} // namespace pnd
