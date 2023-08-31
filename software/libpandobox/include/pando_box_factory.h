#pragma once

#include "pando_box_interface.h"

#include <memory>

namespace pnd {
namespace libpandobox {

struct PandoBoxFactory {
  static std::unique_ptr<PandoBoxInterface> Create(bool mocked);
};

} // namespace libpandobox
} // namespace pnd
