#pragma once

namespace pnd {
namespace common {
namespace thread_prio {

enum class PriorityClass {
  kNormal,
  kAboveNormal,
  kRealtime,
};

void SetProcessPriorityClass(PriorityClass pc);

enum class ThreadPriority {
  kNormal,
  kAboveNormal,
  kTimeCritical,
};

void SetThreadPrio(ThreadPriority tp);

} // namespace thread_prio
} // namespace common
} // namespace pnd
