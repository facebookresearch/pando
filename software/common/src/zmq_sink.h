#pragma once

#include "singleton_context.h"

#include "spdlog/common.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/sink.h"

#include <fmt/format.h>
#include <zmq.hpp>

#include <mutex>

namespace spdlog {
namespace sinks {

/** A custom spdlog sink that publishes messages on a ZMQ PUB socket. */
class zmq_sink : public base_sink<std::mutex> {
 public:
  // Last token of topic_format_string should be %l - level ("trace", "debug", "info", "warn",
  // "error", or "critical")
  zmq_sink(const std::string& pub_ep, const std::string& topic_format_string)
      : topic_formatter_{topic_format_string.c_str(), pattern_time_type::local, ""} {
    pub_socket_.connect(pub_ep);

    // Make sure overridden set_pattern_ runs once to set eol=""
    set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
  }

 protected:
  void sink_it_(const details::log_msg& msg) final {
    // Create the ZMQ topic string
    spdlog::memory_buf_t topic;
    topic_formatter_.format(msg, topic);

    // Create the formatted message
    spdlog::memory_buf_t payload;
    formatter_->format(msg, payload);

    pub_socket_.send(topic.data(), topic.size(), ZMQ_SNDMORE);
    pub_socket_.send(payload.data(), payload.size());
  }

  void flush_() final{};

  void set_pattern_(const std::string& pattern) final {
    // Explicitly set eol="" so that there's no newline at the end of each message
    set_formatter_(
        details::make_unique<spdlog::pattern_formatter>(pattern, pattern_time_type::local, ""));
  }

 private:
  pattern_formatter topic_formatter_;
  std::shared_ptr<zmq::context_t> context_{pnd::common::SingletonContext::Get()};
  zmq::socket_t pub_socket_{*context_, zmq::socket_type::pub};
};

} // namespace sinks
} // namespace spdlog
