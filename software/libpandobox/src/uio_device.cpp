#include "uio_device.h"

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <regex>
#include <stdexcept>

namespace pnd {
namespace libpandobox {

UioDevice::UioDevice(unsigned int uio_dev) {
  mem_size_ = GetMemorySize(uio_dev);
  std::string dev_path = "/dev/uio" + std::to_string(uio_dev);

  uio_fd_ = open(dev_path.c_str(), O_RDWR);

  // Check to be sure we got a working UIO file
  assert(uio_fd_ >= 0);
}

UioDevice::~UioDevice() {
  close(uio_fd_);
}

void UioDevice::UnmaskInterrupt() {
  int irq_clr = 1;
  write(uio_fd_, &irq_clr, sizeof(int));
}

unsigned int UioDevice::WaitForInterrupt(int timeout) {
  pollfd poll_params = {uio_fd_, POLLIN, 0};

  int poll_result = poll(&poll_params, 1, timeout);

  if (poll_result) {
    unsigned int irq_count = 0;
    read(uio_fd_, &irq_count, sizeof(unsigned int));
    UnmaskInterrupt();
    return irq_count;
  }

  return 0;
}

size_t UioDevice::GetMemorySize(unsigned int uio_dev) {
  std::filesystem::path sizePath{"/sys/class/uio"};
  sizePath /= "uio" + std::to_string(uio_dev);
  sizePath /= "maps/map0/size";

  unsigned int size;
  std::ifstream(sizePath) >> std::hex >> size;

  return size;
}

unsigned int UioDevice::FindDevByName(const std::string& dev_name) {
  std::regex uio_re("uio([0-9]+)");

  for (auto& entry : std::filesystem::directory_iterator("/sys/class/uio")) {
    if (!entry.is_directory())
      continue;

    std::string filename = entry.path().filename();
    std::smatch m;
    if (!std::regex_match(filename, m, uio_re))
      continue;

    auto dev_num = static_cast<unsigned int>(std::stoi(m[1]));

    std::string line;
    std::ifstream(entry.path() / "name") >> line;

    if (line == dev_name)
      return dev_num;
  }

  throw std::runtime_error("UIO device not found");
}

} // namespace libpandobox
} // namespace pnd
