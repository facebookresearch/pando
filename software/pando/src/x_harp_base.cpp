#include "x_harp_base.h"

#include <array>
#include <thread>

namespace pnd {
namespace pando {

void XHarpBase::StartAcquisition() {
  Configure();
  LogStartStopData();
  ResetProc();
  ring_buffer_.Reset();

  acquisition_thread_ =
      common::ThreadContainer([&] { Acquire(); }, "XHarpBase::Acquire", &acquisition_stop_signal_);
}

void XHarpBase::StopAcquisition() {
  acquisition_thread_.Stop();
  acquisition_thread_.Join();
  LogStartStopData();
}

} // namespace pando
} // namespace pnd
