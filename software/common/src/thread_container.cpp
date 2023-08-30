#include "thread_container.h"

#include "log_exception.h"
#include "reporter.h"
#include "thread_monitor.h"
#include "thread_name.h"

namespace pnd {
namespace common {

std::atomic_bool g_thread_crashed = false;

void StopSignal::NotifyStop() {
  std::lock_guard<SpinLock> sl{sm_};
  std::unique_lock<std::mutex> l(m_);
  stop_ = true;
  l.unlock();

  cv_.notify_one();
}

void StopSignal::Reset() {
  std::lock_guard<SpinLock> sl{sm_};
  std::unique_lock<std::mutex> l(m_);
  stop_ = false;
}

bool StopSignal::ShouldStop() {
  std::lock_guard<SpinLock> sl{sm_};
  return stop_;
}

bool StopSignal::ShouldStop(const std::chrono::steady_clock::duration& timeout) {
  std::unique_lock<std::mutex> l(m_);
  return cv_.wait_for(l, timeout, [&] { return stop_; });
}

bool StopSignal::ShouldStop(const std::chrono::steady_clock::time_point& deadline) {
  std::unique_lock<std::mutex> l(m_);
  return cv_.wait_until(l, deadline, [&] { return stop_; });
}

ThreadContainer::ThreadContainer(
    const std::function<void()>& f,
    std::string thread_name,
    StopSignal* stop_signal,
    thread_prio::ThreadPriority priority)
    : stop_signal_(stop_signal), thread_name_(std::move(thread_name)) {
  auto wrapper = [](const std::function<void()>& f,
                    std::string thread_name,
                    thread_prio::ThreadPriority priority) {
    SetThreadName(thread_name);

    // Replace the default thread_local Reporter with a named one
    g_reporter = Reporter::Create(thread_name);

    g_reporter->trace("Thread started.");

    auto monitor = ThreadMonitor::Get();

    thread_prio::SetThreadPrio(priority);

    try {
      f();
    } catch (...) {
      LogException(false);
      monitor->HandleThreadCrashed(thread_name);
    }

    g_reporter->trace("Thread stopped.");
  };

  // Clear the stop signal
  if (stop_signal_) {
    stop_signal_->Reset();
  }

  g_reporter->trace("Starting thread {}.", thread_name_);
  thread_ = std::thread(wrapper, f, thread_name_, priority);
}

void ThreadContainer::Stop() {
  if (stop_signal_ && !stop_signal_->ShouldStop()) {
    g_reporter->trace("Stopping thread {}.", thread_name_);
    stop_signal_->NotifyStop();
  }
}

void ThreadContainer::Join() {
  if (thread_.joinable()) {
    thread_.join();
    g_reporter->trace("Joined thread {}.", thread_name_);
  }
}

ThreadContainer::~ThreadContainer() {
  Stop();
  Join();
}

ThreadContainer& ThreadContainer::operator=(ThreadContainer&& other) {
  if (thread_.joinable()) {
    throw std::runtime_error("ThreadContainer move assign called, but lvalue is joinable");
  }
  thread_ = std::move(other.thread_);
  thread_name_ = std::move(other.thread_name_);
  stop_signal_ = other.stop_signal_;
  other.stop_signal_ = nullptr;

  return *this;
}

} // namespace common
} // namespace pnd
