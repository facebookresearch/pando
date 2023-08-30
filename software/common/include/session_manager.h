#pragma once

#include <chrono>
#include <memory>
#include <mutex>

namespace pnd {
namespace common {

namespace detail {

class SessionManagerBase {
 public:
  virtual ~SessionManagerBase() = default;

 protected:
  friend struct SessionTerminator;

  /** Stop threads, release resources, etc.
   * Implement in most derived class.
   */
  virtual void StopImpl() = 0;

  std::mutex session_mutex_;
  std::unique_lock<std::mutex> session_lock_;
};

/** Custom deleter for SessionManagerBase that ends the current session instead of deleting. */
struct SessionTerminator {
  void operator()(SessionManagerBase* s) {
    s->StopImpl();
    s->session_lock_.unlock();
  }
};

} // namespace detail

/** Handle representing an active session. */
using SessionHandle = std::unique_ptr<detail::SessionManagerBase, detail::SessionTerminator>;

/** Base class for objects that have some concept of a "Session" that can be started and
 * stopped. Enforces the following invariants:
 *   - Only one session may exist at a time (no repeated Starts)
 *   - Destructing the SessionHandle returned by SessionManager::Start ends theassociated session
 * @tparam StartParams The argument types for the Start member function.
 */
template <class... StartParams>
class SessionManager : public detail::SessionManagerBase {
 public:
  /** Open a session.
   * @return The handle associated with the new session
   */

  template <class... StartArgs>
  SessionHandle Start(StartArgs&&... startArgs) {
    session_lock_ = std::unique_lock<std::mutex>(session_mutex_);
    SessionHandle session_handle(this);
    start_time_ = std::chrono::steady_clock::now();
    StartImpl(std::forward<StartArgs>(startArgs)...);

    return session_handle;
  }

 protected:
  /** Allocate resources, Initialize state, start threads, etc.
   * Implement in most derived class.
   */
  virtual void StartImpl(StartParams...) = 0;

  std::chrono::steady_clock::time_point start_time_;
};

} // namespace common
} // namespace pnd
