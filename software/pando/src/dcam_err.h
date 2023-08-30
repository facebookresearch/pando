#pragma once

#include < dcamapi4.h>

#include <cstring>
#include <exception>

namespace pnd {
namespace pando {
namespace dcam {

/** Turns DCAMERR values into exception strings. */
struct Exception : std::exception {
  Exception(DCAMERR err, HDCAM hdcam = nullptr) : err_{err} {
    DCAMDEV_STRING param{};
    param.size = sizeof(param);
    param.text = msg_;
    param.textbytes = sizeof(msg_);
    param.iString = err;

    if (failed(dcamdev_getstring(hdcam, &param))) {
      strncpy(msg_, "dcamdev_getstring failed", sizeof(msg_));
    }
  }

  const char* what() const final {
    return msg_;
  }

  DCAMERR err() const {
    return err_;
  }

 private:
  DCAMERR err_;
  char msg_[256];
};

} // namespace dcam
} // namespace pando
} // namespace pnd