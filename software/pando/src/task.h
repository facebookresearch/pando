#pragma once

#include <future>
#include <memory>
#include <utility>

namespace pnd {
namespace pando {

/** Interface class for Task.
 * By accessing a Task instance through this interface, we can erase its type. */
struct TaskInterface {
  virtual void operator()() = 0;
  virtual ~TaskInterface() = default;
};

template <class>
class Task;

/** Class for wrapping a std::future.
 * Gets the result of the stored std::future on demand (possibly triggering lazy evaluation), then
 * makes the result available in a new future. */
template <class R>
class Task<std::future<R>> : public TaskInterface {
 public:
  /** Store an existing std::future */
  Task(std::future<R>&& future) : future_{std::move(future)} {};

  /** Get a new future that will eventually hold the result of calling get on the stored future. */
  auto GetFuture() {
    return pt_.get_future();
  }

 private:
  /** Helper function used to create a std::packaged_task. */
  static R Run(std::future<R>& f) {
    return f.get();
  }

  /** Implementation of TaskInterface::operator().
   * Triggers lazy evaluation of the stored std::future_ */
  void operator()() final {
    pt_(future_);
  }

  std::future<R> future_;
  std::packaged_task<decltype(Run)> pt_{Run};
};

/** Factory class for Task */
struct TaskFactory {
  /** Create a Task by binding arguments to a function or function object.
   * By calling std::async with policy = deferred, we leverage its high quality implementation of
   * the concept of a "deferred task" without actually using it to run that task.
   *
   * @param f Function, function pointer, or function object instance to call
   * @param args The arguments to invoke the function object with
   * @return a std::unique_ptr containing the new Task
   */
  template <class F, class... Args>
  static auto Create(F&& f, Args&&... args) {
    auto fut = std::async(std::launch::deferred, std::forward<F>(f), std::forward<Args>(args)...);
    return std::make_unique<Task<decltype(fut)>>(std::move(fut));
  }
};

} // namespace pando
} // namespace pnd
