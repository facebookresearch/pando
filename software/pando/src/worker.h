#pragma once

#include "task.h"
#include "thread_container.h"
#include "thread_prio.h"

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>

namespace pnd {
namespace pando {

/** A wrapper for std::future whose destructor waits for any shared state to become ready.
 * This is useful for pausing stack unwinding until an asynchronous task that is using references to
 * local variables completes.
 */
template <class R>
struct BlockingFuture : std::future<R> {
  BlockingFuture() = default;
  BlockingFuture(BlockingFuture&&) = default;
  BlockingFuture(std::future<R>&& f) : std::future<R>::future(std::move(f)) {}

  BlockingFuture& operator=(BlockingFuture&& other) = default;

  ~BlockingFuture() {
    if (this->valid()) {
      this->wait();
    }
  }
};

/** Run functions asynchronously in a worker thread.
 * Similar to std::async with policy = async, but executes tasks inside a particular
 * ThreadContainer instead of a in a new thread or a thread pool.
 */
class Worker {
 public:
  Worker(
      const std::string& thread_name,
      common::thread_prio::ThreadPriority priority = common::thread_prio::ThreadPriority::kNormal);
  ~Worker();

  /** Invoke a function or function object asynchronously in the worker thread.
   * @param f Function, function pointer, or function object instance to call
   * @param args The arguments to invoke the function object with
   * @return A std::future that will eventually hold the result of the invocation
   */
  template <class F, class... Args>
  auto Async(F&& f, Args&&... args) {
    // Create a new task
    auto task = TaskFactory::Create(std::forward<F>(f), std::forward<Args>(args)...);
    auto future = task->GetFuture();

    {
      std::unique_lock<std::mutex> lock(tasks_m_);

      if (thread_stop_signal_.ShouldStop()) {
        throw std::runtime_error("Worker::Async: Worker is already stopping");
      }

      // Queue the new task (and erase its type)
      tasks_.push(std::move(task));
    }

    // Wake the worker thread
    tasks_cv_.notify_all();

    return future;
  }

 private:
  /** Thread function. */
  void Work();

  std::queue<std::unique_ptr<TaskInterface>> tasks_;
  std::mutex tasks_m_;
  std::condition_variable tasks_cv_;

  // The mutex and condition_variable inside StopSignal are made redundant by this class.
  // thread_stop_signal_ could just be a bool if we didn't care about using ThreadContainer.
  common::StopSignal thread_stop_signal_;
  common::ThreadContainer thread_;
};

} // namespace pando
} // namespace pnd
