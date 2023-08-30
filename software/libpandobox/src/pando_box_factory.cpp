#include "pando_box_factory.h"

#include "pando_box.h"

#include <stdexcept>

namespace pnd {
namespace libpandobox {

std::unique_ptr<PandoBoxInterface> PandoBoxFactory::Create(bool mocked) {
  if (mocked) {
    throw std::runtime_error("Unimplemented");
  } else {
    return std::make_unique<PandoBox>();
  }
}

} // namespace libpandobox
} // namespace pnd
