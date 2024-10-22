// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_WAIT_FOR_EXIT_HPP
#define BOOST_PROCESS_DETAIL_POSIX_WAIT_FOR_EXIT_HPP

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/posix/child_handle.hpp>
#include <system_error>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace boost { namespace process { namespace detail { namespace posix {

inline void wait(const child_handle &p, int & exit_code, std::error_code &ec) noexcept
{
    pid_t ret;
    int status;

    do
    {
        ret = ::waitpid(p.pid, &status, 0);
    }
    while (((ret == -1) && (errno == EINTR)) ||
           (ret != -1 && !WIFEXITED(status) && !WIFSIGNALED(status)));

    if (ret == -1)
        ec = boost::process::detail::get_last_error();
    else
    {
        ec.clear();
        exit_code = status;
    }
}

inline void wait(const child_handle &p, int & exit_code) noexcept
{
    std::error_code ec;
    wait(p, exit_code, ec);
    boost::process::detail::throw_error(ec, "waitpid(2) failed in wait");
}

template< class Clock, class Duration >
inline bool wait_until(
        const child_handle &p,
        int & exit_code,
        const std::chrono::time_point<Clock, Duration>& time_out,
        std::error_code & ec) noexcept
{

    ::sigset_t  sigset;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);

    auto get_timespec =
            [](const Duration & dur)
            {
                ::timespec ts;
                ts.tv_sec  = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
                ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() % 1000000000;
                return ts;
            };

    pid_t ret;
    int status;

    struct ::sigaction old_sig;
    if (-1 == ::sigaction(SIGCHLD, nullptr, &old_sig))
    {
        ec = get_last_error();
        return false;
    }

    bool timed_out;
#if defined(BOOST_POSIX_HAS_SIGTIMEDWAIT)
    do
    {
        auto ts = get_timespec(time_out - Clock::now());
        auto ret_sig = ::sigtimedwait(&sigset, nullptr, &ts);
        errno = 0;
        ret = ::waitpid(p.pid, &status, WNOHANG);

        if ((ret_sig == SIGCHLD) && (old_sig.sa_handler != SIG_DFL) && (old_sig.sa_handler != SIG_IGN))
            old_sig.sa_handler(ret);

        if (ret == 0)
        {
            timed_out = Clock::now() >= time_out;
            if (timed_out)
                return false;
        }
    }
    while ((ret == 0) ||
          (((ret == -1) && errno == EINTR) ||
           ((ret != -1) && !WIFEXITED(status) && !WIFSIGNALED(status))));
#else
    //if we do not have sigtimedwait, we fork off a child process  to get the signal in time
    pid_t timeout_pid = ::fork();
    if (timeout_pid  == -1)
    {
        ec = boost::process::detail::get_last_error();
        return true;
    }
    else if (timeout_pid == 0)
    {
        auto ts = get_timespec(time_out - Clock::now());
        ::timespec rem;
        // We need to check the return value of ::nanosleep before deciding whether to continue
        // looping or not.
        // ::nanosleep(&ts, &rem);
        // while (rem.tv_sec > 0 || rem.tv_nsec > 0)
        //     ::nanosleep(&rem, &rem);
        int ret = ::nanosleep(&ts, &rem);
        while (ret != 0) {
            if (errno == EFAULT || errno == EINVAL) {
                break;
            }
            ret = ::nanosleep(&rem, &rem);
        }
        ::exit(0);
    }

    struct child_cleaner_t
    {
        pid_t pid;
        ~child_cleaner_t()
        {
            int res;
            // Below code is incorrect, SIGTERM maps to 15 not -15:
            // ::kill(pid, -15);
            // ::waitpid(pid, &res, WNOHANG);
            // This means the spawned child process isn't guaranteed to have died.
            // With the bug in the loop above this generates a zombie process that eats CPU cycles.
            // We still call ::waitpid to ensure the child process truly died.
            ::kill(pid, SIGTERM);
            ::waitpid(pid, &res, 0);
        }
    };
    child_cleaner_t child_cleaner{timeout_pid};

    do
    {
        int ret_sig = 0;
        if ((::waitpid(timeout_pid, &status, WNOHANG) != 0)
         && (WIFEXITED(status) || WIFSIGNALED(status)))
            ret_sig = ::sigwait(&sigset, nullptr);
        errno = 0;

        ret = ::waitpid(p.pid, &status, WNOHANG);

        if ((ret_sig == SIGCHLD) &&
            (old_sig.sa_handler != SIG_DFL) && (old_sig.sa_handler != SIG_IGN))
            old_sig.sa_handler(ret);

        if (ret <= 0)
        {
            timed_out = Clock::now() >= time_out;
            if (timed_out)
                return false;
        }
    }
    while ((ret == 0) ||
           (((ret == -1) && errno == EINTR) ||
            ((ret != -1) && !WIFEXITED(status) && !WIFSIGNALED(status))));
#endif

    if (ret == -1)
        ec = boost::process::detail::get_last_error();
    else
    {
        ec.clear();
        exit_code = status;
    }

    return true;
}

template< class Clock, class Duration >
inline bool wait_until(
        const child_handle &p,
        int & exit_code,
        const std::chrono::time_point<Clock, Duration>& time_out)
{
    std::error_code ec;
    bool b = wait_until(p, exit_code, time_out, ec);
    boost::process::detail::throw_error(ec, "waitpid(2) failed in wait_until");
    return b;
}

template< class Rep, class Period >
inline bool wait_for(
        const child_handle &p,
        int & exit_code,
        const std::chrono::duration<Rep, Period>& rel_time,
        std::error_code & ec) noexcept
{
    return wait_until(p, exit_code, std::chrono::steady_clock::now() + rel_time, ec);
}

template< class Rep, class Period >
inline bool wait_for(
        const child_handle &p,
        int & exit_code,
        const std::chrono::duration<Rep, Period>& rel_time)
{
    std::error_code ec;
    bool b = wait_for(p, exit_code, rel_time, ec);
    boost::process::detail::throw_error(ec, "waitpid(2) failed in wait_for");
    return b;
}

}}}}

#endif
