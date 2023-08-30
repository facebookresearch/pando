#pragma once

#include "mapped_ptr.h"

#include <string>

namespace pnd {
namespace libpandobox {

/** Class to encapsulate UIO Device management.
 *
 * This is intended as a base to build simple UIO Device drivers. It deals with the UIO device
 * file and handling of interrupts.
 */
class UioDevice {
 public:
  UioDevice(unsigned int uio_dev);

  UioDevice(const std::string& dev_name) : UioDevice(FindDevByName(dev_name)) {}

  ~UioDevice();

  /** Unmasks interrupts in the UIO driver.
   *
   * Before attempting to wait for interrupts, interrupts must be
   * unmasked by writing to the UIO File Descriptor.
   */
  void UnmaskInterrupt();

  /** Waits for an interrupt up to <timeout> milliseconds.
   *
   * If the timeout expires, returns 0, otherwise returns the number
   * of interrupts since the last attempt.
   *
   * Interrupts are automatically unmasked after an interrupt is
   * encountered.
   */
  unsigned int WaitForInterrupt(int timeout = -1);

  static size_t GetMemorySize(unsigned int uio_dev);

  /** Find the device number of a UIO device with the given name.
   * If multiple such devices exist, the lowest device number is returned.
   * @param dev_name the name to search for
   * @return A UIO device number (ie. x in /dev/uiox)
   */
  static unsigned int FindDevByName(const std::string& dev_name);

 protected:
  int uio_fd_;
  size_t mem_size_;
};

/** Simple class for accesssing registers exposed via a UIO device.
 * @tparam T The struct representing the register layout of the underlying device
 */
template <typename T>
class UioRegisters : private UioDevice {
 public:
  using UioDevice::UioDevice;

  T* operator->() {
    return this->registers_.operator->();
  }

  const T* operator->() const {
    return this->registers_.operator->();
  }

 private:
  MappedPtr<T> registers_{PROT_READ | PROT_WRITE, MAP_SHARED, uio_fd_, 0};
};

} // namespace libpandobox
} // namespace pnd
