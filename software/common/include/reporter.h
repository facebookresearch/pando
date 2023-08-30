#pragma once

#include <spdlog/spdlog.h>

#include <spdlog/sinks/dist_sink.h>

namespace pnd {
namespace common {

class Reporter {
 public:
  ~Reporter();

  /** Create a new spdlog::logger backed by dist_sink_.
   * @param thread_name Name for the logger
   */
  static std::shared_ptr<spdlog::logger> Create(const std::string& thread_name);

  /** Open a new log file, closing any existing one.
   * @param filename The name of the new log file
   */
  static void OpenLogfile(const spdlog::filename_t& filename);

  /** Set the log level for the file sink */
  static void SetLogfileLevel(spdlog::level::level_enum level);

  /** Set the log level for the stderr sink. */
  static void SetStderrLevel(spdlog::level::level_enum level);

  /** Emit a log message containing the information in version.h */
  static void LogVersion();

  /** Create a new ZMQ sink, closing any existing one. */
  static void CreateZmqSink(const std::string& pub_ep, const std::string& topic_format_string);

  /** Destroy any existing ZMQ sink */
  static void DestroyZmqSink();

 private:
  Reporter();

  /** Update the distribution sink */
  void SetSinks();
  std::shared_ptr<spdlog::sinks::dist_sink_mt> dist_sink_;
  spdlog::sink_ptr stderr_sink_;
  spdlog::sink_ptr file_sink_;
  spdlog::sink_ptr zmq_sink_;

  static Reporter instance;
  std::shared_ptr<spdlog::logger> canary_;
};

/** thread_local spdlog::logger */
extern thread_local std::shared_ptr<spdlog::logger> g_reporter;

} // namespace common

// Exported into namespace pnd to save typing `common::`
using common::g_reporter;

} // namespace pnd
