
#include "thread_name.h"

#ifdef _WIN32
#include <Windows.h>
#include <processthreadsapi.h>
#include <stringapiset.h>
#endif

namespace pnd {
namespace common {

void SetThreadName(const std::string& thread_name) {
#ifdef _WIN32
  // Convert std::string to std::wstring, because SetThreadDescription takes wchar_t*
  auto cchWideChar =
      MultiByteToWideChar(CP_UTF8, 0, thread_name.c_str(), thread_name.size(), nullptr, 0);
  std::wstring thread_name_w(cchWideChar, '\0');
  MultiByteToWideChar(
      CP_UTF8, 0, thread_name.c_str(), thread_name.size(), thread_name_w.data(), cchWideChar);

  SetThreadDescription(GetCurrentThread(), thread_name_w.c_str());
#endif
}

} // namespace common
} // namespace pnd
