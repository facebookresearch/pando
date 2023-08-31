#pragma once

#include "device_interface.h"

#include <memory>

namespace pnd {
namespace pando {

/** Device class representing the pairing of the Basler acA2000-340kM CMOS camera with the
 * SiliconSoftware microEnable 5 marathon ACL camera link framegrabber.
 */
struct BaslerAca2000CL : public DeviceInterface {
  static std::unique_ptr<BaslerAca2000CL> Create();
};

} // namespace pando
} // namespace pnd
