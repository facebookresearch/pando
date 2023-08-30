#include "worker.h"

namespace pnd {
namespace pando {

Worker::Worker(const std::string& thread_name, common::thread_prio::ThreadPriority priority)
    : thread_([&] { Work(); }, thread_name, &thread_stop_signal_, priority) {}

Worker::~Worker() {
  {
    std::unique_lock<std::mutex> lock(tasks_m_);
    thread_.Stop();
  }
  tasks_cv_.notify_all();
  thread_.Join();
}

void Worker::Work() {
  while (true) {
    std::unique_ptr<TaskInterface> task;
    {
      std::unique_lock<std::mutex> lock(tasks_m_);

      // Wait for a task (or a request to exit)
      tasks_cv_.wait(lock, [&] { return !tasks_.empty() || thread_stop_signal_.ShouldStop(); });

      // If tasks_ is empty, we were asked to stop
      if (tasks_.empty()) {
        return;
      }

      task = std::move(tasks_.front());
      tasks_.pop();
    }

    // Run the task
    (*task)();
  }
}

} // namespace pando
} // namespace pnd
