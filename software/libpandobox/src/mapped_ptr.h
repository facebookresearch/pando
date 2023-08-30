#pragma once

#include <sys/mman.h>

#include <cassert>
#include <memory>

namespace pnd {
namespace libpandobox {

/** Implements RAII around mmap & munmap using std::shared_ptr with a custom deleter. */
template <class T>
struct MappedPtr : std::shared_ptr<T> {
  MappedPtr() = default;

  MappedPtr(int prot, int flags, int fd, off_t offset)
      : std::shared_ptr<T>::shared_ptr{
            static_cast<T*>(mmap(nullptr, sizeof(T), prot, flags, fd, offset)),
            [](T* p) { munmap((void*)p, sizeof(T)); }} {
    assert(this->get() && "Unable to mmap memory in MappedPtr");
  }
};

} // namespace libpandobox
} // namespace pnd
