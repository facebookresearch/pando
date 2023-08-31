#include "pando.h"

#include <H5Cpp.h>

#include <vector>

namespace pnd {
namespace pando {
class ConfigH5Transcoder {
 public:
  static void Write(H5::H5File& file, PandoInterface::Config config);

 private:
  static void WriteAttribute(const std::string& name, int value, H5::Group metadata);
  static void WriteAttribute(const std::string& name, const std::string& value, H5::Group metadata);
  static void WriteAttribute(const std::string& name, uint64_t value, H5::Group metadata);
  static void WriteAttribute(const std::string& name, uint32_t value, H5::Group metadata);
  static void
  WriteAttribute(const std::string& name, const std::map<int, int>& value, H5::Group metadata);
  static void
  WriteAttribute(const std::string& name, const std::list<int32_t>& value, H5::Group metadata);
  static void WriteAttribute(const std::string& name, double value, H5::Group metadata);
  static void WriteAttribute(const std::string& name, bool value, H5::Group metadata);
  static void
  WriteAttribute(const std::string& name, const std::vector<int32_t>& value, H5::Group metadata);
};
} // namespace pando
} // namespace pnd
