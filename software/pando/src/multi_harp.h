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

/** Partial implementation of XHarpBase for real or mocked Multi Harp in T2 mode */
class MultiHarpT2Base : public XHarpProcBase<MultiHarpT2Base> {
  friend class XHarpProcBase<MultiHarpT2Base>;

  struct T2TimeTag {
    Record timetag : 25;
    Record channel : 6;
    Record special : 1;
  };

  static constexpr uint64_t kT2Wraparound = 33554432;

  void ConsumeRecord(Record tag);

  /** TTREADMAX for MultiHarp is 8 times what it is for pico, hydra. Need to adjust
      how long we'll wait to get all the tags */
  int GetExcessDelayFactor() final {
    return 2;
  }
};

/** Partial implementation of XHarpBase for real or mocked Multi Harp in T3 mode */
class MultiHarpT3Base : public XHarpProcBase<MultiHarpT3Base> {
  friend class XHarpProcBase<MultiHarpT3Base>;

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

  /** TTREADMAX for MultiHarp is 8 times what it is for pico, hydra. Need to adjust
      how long we'll wait to get all the tags */
  int GetExcessDelayFactor() final {
    return 2;
  }
};

/** Partial implementation of XHarpBase for real Multi Harp in T2 or T3 mode */
class RealMultiHarpBase : virtual public XHarpBase {
  void Configure() final;
  void Acquire() final;
  void LogStartStopData() final;

 protected:
  // Forward declaration
  class Driver;

  std::unique_ptr<Driver> driver_;
};

/** Complete implementation of XHarpBase for real Multi Harp in T2 mode */
class MultiHarpT2 : public MultiHarpT2Base, public RealMultiHarpBase {
 public:
  MultiHarpT2();
};

/** Complete implementation of XHarpBase for real Multi Harp in T3 mode */
class MultiHarpT3 : public MultiHarpT3Base, public RealMultiHarpBase {
 public:
  MultiHarpT3();
};

/** Complete implementation of XHarpBase for mocked Multi Harp in T2 mode */
using MockMultiHarpT2 = MockXHarp<MultiHarpT2Base, PTUReader::kRtMultiHarp2T2>;

/** Complete implementation of XHarpBase for mocked Multi Harp in T3 mode */
using MockMultiHarpT3 = MockXHarp<MultiHarpT3Base, PTUReader::kRtMultiHarp2T3>;

} // namespace pando
} // namespace pnd
