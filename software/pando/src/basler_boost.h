#include "device_interface.h"

#include <memory>

namespace pnd {
namespace pando {

/**
 * Class for interfaceing with a CXP-12 CoaXpress capture card.
 */
struct BaslerBoost : public DeviceInterface {
  static std::unique_ptr<BaslerBoost> Create();

  virtual ~BaslerBoost() = default;
};

} // namespace pando
} // namespace pnd
