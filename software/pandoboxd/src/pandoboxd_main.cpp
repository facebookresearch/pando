#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

#include <fmt/chrono.h>

#include "reporter.h"
#include "rpc_server.h"

static std::atomic_int g_signal = 0;
static void SignalHandler(int which) {
  g_signal = which;
};

int main() {
  using namespace pnd;

  // Catch CTRL-C and other fun signals
  std::signal(SIGINT, SignalHandler);
  std::signal(SIGTERM, SignalHandler);

  // Name the reporter for this thread
  g_reporter = common::Reporter::Create("main");

  common::Reporter::SetStderrLevel(spdlog::level::trace);

  // Open a log file
  common::Reporter::OpenLogfile(fmt::format(
      "pandoboxd_{0:%y}_{0:%m}_{0:%d}_{0:%H}{0:%M}{0:%S}.log", fmt::localtime(std::time(nullptr))));

  // Start a server
  pandoboxd::RpcServer server;

  // Wait for SIGINT or SIGTERM
  while (g_signal == 0) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  g_reporter->info("Signal received: {}", g_signal);

  return 0;
}
