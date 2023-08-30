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

/** Partial implementation of XHarpBase for real or mocked Hydra Harp in T2 mode */
class HydraHarpT2Base : public XHarpProcBase<HydraHarpT2Base> {
  friend class XHarpProcBase<HydraHarpT2Base>;

  struct T2TimeTag {
    Record timetag : 25;
    Record channel : 6;
    Record special : 1;
  };

  static constexpr uint64_t kT2Wraparound = 33554432;

  void ConsumeRecord(Record tag);
};

/** Partial implementation of XHarpBase for real or mocked Hydra Harp in T3 mode */
class HydraHarpT3Base : public XHarpProcBase<HydraHarpT3Base> {
  friend class XHarpProcBase<HydraHarpT3Base>;

  struct T3TimeTag {
    Record nsync : 10;
    Record dtime : 15;
    Record channel : 6;
    Record special : 1;
  };

  static constexpr uint64_t kT3Wraparound = 1024;

  void ConsumeRecord(Record tag);

  bool DeviceGeneratesMicrotimes() override {
    return true;
  }
};

/** Partial implementation of XHarpBase for real Hydra Harp in T2 or T3 mode */
class RealHydraHarpBase : virtual public XHarpBase {
  void Configure() final;
  void Acquire() final;
  void LogStartStopData() final;

 protected:
  // Forward declaration
  class Driver;

  std::unique_ptr<Driver> driver_;
};

/** Complete implementation of XHarpBase for real Hydra Harp in T2 mode */
class HydraHarpT2 : public HydraHarpT2Base, public RealHydraHarpBase {
 public:
  HydraHarpT2();
};

/** Complete implementation of XHarpBase for real Hydra Harp in T3 mode */
class HydraHarpT3 : public HydraHarpT3Base, public RealHydraHarpBase {
 public:
  HydraHarpT3();
};

/** Complete implementation of XHarpBase for mocked Hydra Harp in T2 mode */
using MockHydraHarpT2 = MockXHarp<HydraHarpT2Base, PTUReader::kRtHydraHarp2T2>;

/** Complete implementation of XHarpBase for mocked Hydra Harp in T3 mode */
using MockHydraHarpT3 = MockXHarp<HydraHarpT3Base, PTUReader::kRtHydraHarp2T3>;

} // namespace pando
} // namespace pnd
