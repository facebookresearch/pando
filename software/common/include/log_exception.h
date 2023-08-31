#pragma once

#include "reporter.h"

#include <boost/type_index.hpp>

#include <exception>

namespace pnd {
namespace common {

inline void LogException(bool rethrow = true) {
  if (auto eptr = std::current_exception()) {
    try {
      std::rethrow_exception(eptr);
    } catch (const std::exception& e) {
      g_reporter->critical("{}: {}", boost::typeindex::type_id_runtime(e).pretty_name(), e.what());
    } catch (...) {
      g_reporter->critical("Unknown Exception.");
    }

    if (rethrow) {
      std::rethrow_exception(eptr);
    }
  }
}

} // namespace common
} // namespace pnd
