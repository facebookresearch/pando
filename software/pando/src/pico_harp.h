#pragma once

#include <chrono>
#include <fstream>
#include <memory>
#include <stdexcept>

#include "mock_x_harp_base.h"
#include "ptu_reader.h"
#include "types.h"
#include "x_harp_base.h"
#include "x_harp_proc_base.h"

namespace pnd {
namespace pando {

/** Partial implementation of XHarpBase for real or mocked Pico Harp in T2 mode */
class PicoHarpT2Base : public XHarpProcBase<PicoHarpT2Base> {
  friend class XHarpProcBase<PicoHarpT2Base>;

  struct T2TimeTag {
    Record time : 28;
    Record channel : 4;
  };

  static constexpr uint64_t kPicosecondsPerTick = 4;
  static constexpr uint64_t kT2Wraparound = 210698240;

  void ConsumeRecord(const Record& tag);
};

/** Partial implementation of XHarpBase for real or mocked Pico Harp in T3 mode */
class PicoHarpT3Base : public XHarpProcBase<PicoHarpT3Base> {
  friend class XHarpProcBase<PicoHarpT3Base>;

  struct T3TimeTag {
    Record nsync : 16;
    Record dtime : 12;
    Record channel : 4;
  };

  static constexpr uint64_t kPicosecondsPerTick = 4;
  static constexpr uint64_t kT3Wraparound = 65536;

  void ConsumeRecord(const Record& tag);
  bool DeviceGeneratesMicrotimes() override {
    return true;
  }
};

/** Partial implementation of XHarpBase for real Pico Harp in T2 or T3 mode */
class RealPicoHarpBase : virtual public XHarpBase {
  void Configure() final;
  void Acquire() final;

 protected:
  // Forward declaration
  class Driver;

  std::unique_ptr<Driver> driver_;
};

/** Complete implementation of XHarpBase for real Pico Harp in T2 mode */
class PicoHarpT2 : public PicoHarpT2Base, public RealPicoHarpBase {
 public:
  PicoHarpT2();
};

/** Complete implementation of XHarpBase for real Pico Harp in T3 mode */
class PicoHarpT3 : public PicoHarpT3Base, public RealPicoHarpBase {
 public:
  PicoHarpT3();
};

/** Complete implementation of XHarpBase for mocked Pico Harp in T2 mode */
using MockPicoHarpT2 = MockXHarp<PicoHarpT2Base, PTUReader::kRtPicoHarp2T2>;

/** Complete implementation of XHarpBase for mocked Pico Harp in T3 mode */
using MockPicoHarpT3 = MockXHarp<PicoHarpT3Base, PTUReader::kRtPicoHarp2T3>;

} // namespace pando
} // namespace pnd
