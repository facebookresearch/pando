#pragma once

#include "pandoboxd_interface.h"

#include "singleton_context.h"
#include "thread_container.h"

#include <zmq.hpp>
#include <mutex>

namespace pnd {
namespace pandoboxd {

class ReportAccumulator {
 public:
  ReportAccumulator(std::string sub_sock_ep, std::string report_topic_base);
  ~ReportAccumulator();

  using Reports = PandoboxdInterface::Reports;
  using Report = PandoboxdInterface::Report;

  Reports& GetReports();

 private:
  Reports reports_, reports_internal_;
  std::mutex reports_internal_mutex_; // guards reports_internal_

  std::string sub_sock_ep_, report_topic_base_;

  common::ThreadContainer accumulator_thread_;
  common::StopSignal accumulator_stop_signal_;

  std::shared_ptr<zmq::context_t> context_{common::SingletonContext::Get()};

  void Accumulate();
};

} // namespace pandoboxd
} // namespace pnd
