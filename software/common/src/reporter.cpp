#include "reporter.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "zmq_sink.h"

namespace pnd {
namespace common {

Reporter Reporter::instance;
std::shared_ptr<spdlog::logger> default_reporter = Reporter::Create("unnamed_thread");
thread_local std::shared_ptr<spdlog::logger> g_reporter = default_reporter;

Reporter::Reporter() {
  dist_sink_ = std::make_shared<spdlog::sinks::dist_sink_mt>();

  // Create dummy file sink (defer opening logfile until filename is provided)
  file_sink_ = std::make_shared<spdlog::sinks::null_sink_mt>();

  // Create stderr sink
  stderr_sink_ = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
  stderr_sink_->set_pattern("[%n] [%^%l%$] %v");
  stderr_sink_->set_level(spdlog::level::trace);

  // Create dummy zmq sink (defer opening logfile until pub endpoint is provided)
  zmq_sink_ = std::make_shared<spdlog::sinks::null_sink_mt>();

  // Update dist_sink_ to use file_sink_ & stderr_sink_
  SetSinks();

  canary_ = Create("reporter_canary");
}

Reporter::~Reporter() {
  canary_->info("Reporter: Shutting down cleanly.");
}

std::shared_ptr<spdlog::logger> Reporter::Create(const std::string& thread_name) {
  auto r = std::make_shared<spdlog::logger>(thread_name, instance.dist_sink_);
  r->set_level(spdlog::level::trace);
  r->flush_on(spdlog::level::trace);
  return r;
}

void Reporter::OpenLogfile(const spdlog::filename_t& filename) {
  instance.file_sink_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, false);
  instance.file_sink_->set_level(spdlog::level::trace);

  instance.SetSinks();

  g_reporter->info("Reporter: Opened {}", filename);
}

void Reporter::SetLogfileLevel(spdlog::level::level_enum level) {
  instance.file_sink_->set_level(level);
}

void Reporter::SetStderrLevel(spdlog::level::level_enum level) {
  instance.stderr_sink_->set_level(level);
}

void Reporter::CreateZmqSink(const std::string& pub_ep, const std::string& topic_format_string) {
  instance.zmq_sink_ = std::make_shared<spdlog::sinks::zmq_sink>(pub_ep, topic_format_string);
  instance.zmq_sink_->set_level(spdlog::level::trace);

  instance.SetSinks();
}

void Reporter::DestroyZmqSink() {
  instance.zmq_sink_ = std::make_shared<spdlog::sinks::null_sink_mt>();

  instance.SetSinks();
}

void Reporter::SetSinks() {
  dist_sink_->set_sinks(std::vector<spdlog::sink_ptr>{stderr_sink_, file_sink_, zmq_sink_});
}

} // namespace common
} // namespace pnd
