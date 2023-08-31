
#include "thread_prio.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace pnd {
namespace common {
namespace thread_prio {

void SetProcessPriorityClass(PriorityClass pc) {
#ifdef _WIN32
  auto proc = GetCurrentProcess();
  switch (pc) {
    case PriorityClass::kNormal:
      SetThreadPriority(proc, NORMAL_PRIORITY_CLASS);
      break;
    case PriorityClass::kAboveNormal:
      SetThreadPriority(proc, ABOVE_NORMAL_PRIORITY_CLASS);
      break;
    case PriorityClass::kRealtime:
      SetThreadPriority(proc, REALTIME_PRIORITY_CLASS);
      break;
  }
#endif
}

void SetThreadPrio(ThreadPriority tp) {
#ifdef _WIN32
  auto thread = GetCurrentThread();
  switch (tp) {
    case ThreadPriority::kNormal:
      SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);
      break;
    case ThreadPriority::kAboveNormal:
      SetThreadPriority(thread, THREAD_PRIORITY_ABOVE_NORMAL);
      break;
    case ThreadPriority::kTimeCritical:
      SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL);
      break;
  }
#endif
}

} // namespace thread_prio
} // namespace common
} // namespace pnd
