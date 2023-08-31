#include "report_accumulator.h"

#include <fmt/format.h>
#include <list>
#include <map>

namespace pnd {
namespace pandoboxd {

using Reports = ReportAccumulator::Reports;
using Report = ReportAccumulator::Report;

ReportAccumulator::ReportAccumulator(std::string sub_sock_ep, std::string report_topic_base)
    : sub_sock_ep_(sub_sock_ep), report_topic_base_(report_topic_base) {
  accumulator_thread_ = common::ThreadContainer(
      [&] { Accumulate(); }, "ReportAccumulator::Accumulate", &accumulator_stop_signal_);
}

ReportAccumulator::~ReportAccumulator() {
  accumulator_thread_.Stop();
  accumulator_thread_.Join();
}

Reports& ReportAccumulator::GetReports() {
  std::lock_guard<std::mutex> lck(reports_internal_mutex_);

  reports_ = std::move(reports_internal_);
  reports_internal_.clear();

  return reports_;
}

void ReportAccumulator::Accumulate() {
  zmq::socket_t socket{*context_, zmq::socket_type::sub};
  socket.bind(sub_sock_ep_);

  // Subscribe to topics.
  std::map<std::string, std::string> topics_to_levels = {};
  for (auto& level :
       std::list<std::string>{"trace", "debug", "info", "warn", "error", "critical"}) {
    std::string topic = fmt::format("{}.{}", report_topic_base_, level);
    topics_to_levels.emplace(topic, level);
    socket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.length());
  }

  zmq::pollitem_t pi = {socket, 0, ZMQ_POLLIN, 0};
  zmq::message_t topic_msg, report_msg;

  while (zmq::poll(&pi, 1, 100) || !accumulator_stop_signal_.ShouldStop()) {
    if (pi.revents & ZMQ_POLLIN) {
      socket.recv(&topic_msg);
      if (topic_msg.more()) {
        socket.recv(&report_msg);
        if (report_msg.more()) {
          throw std::runtime_error(
              "ReportAccumulator: Received ZMQ message with too many segments.");
        }
      } else {
        throw std::runtime_error("ReportAccumulator: Received ZMQ message with too few segments.");
      }

      std::string topic(topic_msg.data<const char>(), topic_msg.size());
      std::string report(report_msg.data<const char>(), report_msg.size());

      // Get level string.
      std::string level;
      if (topics_to_levels.find(topic) != topics_to_levels.end()) {
        level = topics_to_levels.at(topic);
      } else {
        throw std::runtime_error("ReportAccumulator: Received ZMQ message with unknown topic.");
      }

      // Guard and push new report to reports_internal_
      std::lock_guard<std::mutex> lck(
          reports_internal_mutex_); // TODO is there overhead from constructing this everytime?
      reports_internal_.push_back(Report{level, report});
    }
  }
}

} // namespace pandoboxd
} // namespace pnd
