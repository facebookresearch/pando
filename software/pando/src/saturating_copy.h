#pragma once

#include <algorithm>
#include <cstdint>

namespace pnd {
namespace pando {

/** Safely copy a range of unsigned 16 bit integers to a range of unsigned 8 bit integers. */
inline void SaturatingCopy(const uint16_t* first, const uint16_t* last, uint8_t* d_first) {
  while (first != last)
    *d_first++ = std::min(*first++, ((uint16_t)0xFF));
}

} // namespace pando
} // namespace pnd
