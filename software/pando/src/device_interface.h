#pragma once

#include "pando.h"
#include "session_manager.h"

#include <cstdint>
#include <memory>

namespace pnd {
namespace pando {

/** Interface class used by Pando to control devices.
 * The SessionManager StartParams are:
 *   1. The experiment_id
 *   2. The static (non runtime-editable) config
 *   3. The path that the HDF5 raw file should be created at
 */
class DeviceInterface
    : public common::SessionManager<int32_t, PandoInterface::Config, const char*> {};
} // namespace pando
} // namespace pnd
