#pragma once

#include "spin_lock.h"
#include "thread_prio.h"

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace pnd {
namespace common {

class StopSignal {
 public:
  /** Check for a request to stop.
   * @return true if thread should stop
   */
  bool ShouldStop();

  /** Wait for a request to stop for @p timeout.
   * @param[in] timeout Duration to wait before returning false
   * @return true if thread should stop
   */
  bool ShouldStop(const std::chrono::steady_clock::duration& timeout);

  /**
   * Wait for a request to stop until @p deadline.
   * @param[in] deadline Timepoint to wait until before returning false
   * @return true if thread should stop
   */
  bool ShouldStop(const std::chrono::steady_clock::time_point& deadline);

 private:
  friend class ThreadContainer;

  /** @brief Signal the waiting thread to stop. */
  void NotifyStop();

  /**
   * @brief Reset stop_ to false
   */
  void Reset();

  bool stop_ = false;
  SpinLock sm_;
  std::mutex m_;
  std::condition_variable cv_;
};

/** Launch a thread.
 * @param f Function to execute in thread
 * @param thread_name Name for the thread_local Reporter instance
 * @param stop_signal Raw pointer to the StopSignal that @p f monitors
 */
class ThreadContainer {
 public:
  ThreadContainer() {}
  ThreadContainer(
      const std::function<void()>& f,
      std::string thread_name,
      StopSignal* stop_signal = nullptr,
      thread_prio::ThreadPriority priority = thread_prio::ThreadPriority::kNormal);

  /** Ask the thread to stop. */
  void Stop();

  /** Join the thread. */
  void Join();

  ~ThreadContainer();

  /** Move assignment operator. */
  ThreadContainer& operator=(ThreadContainer&& other);

 private:
  StopSignal* stop_signal_ = nullptr;
  std::string thread_name_;
  std::thread thread_;
};

} // namespace common
} // namespace pnd
