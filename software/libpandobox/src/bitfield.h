#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace pnd {
namespace libpandobox {

/** Base class for building types that represent bitfields.
 *
 * Here's a brief example of how this class could be used to represent a
 * 32 bit word split into two fields of 4 bits and 28 bits, where the 4 bit
 * field is readonly and the 28 bit field is read/write:
 *
 *   class ExampleBField : BitField<volatile uint32_t> {
 *     static constexpr std::size_t kField0Offset = 0;
 *     static constexpr std::size_t kField0Width = 4;
 *     static constexpr std::size_t kField1Offset = 4;
 *     static constexpr std::size_t kField1Width = 28;
 *
 *    public:
 *     uint32_t ReadField0() {
 *       return ReadField<kField0Width, kField0Offset>();
 *     }
 *     uint32_t ReadField1() {
 *       return ReadField<kField1Width, kField1Offset>();
 *     }
 *     void WriteField1(uint32_t value) {
 *       return WriteField<kField1Width, kField1Offset>(value);
 *     }
 *   };
 *
 * @tparam Storage An unsigned integral type that the fields are packed into
 */
template <class Storage>
struct BitField {
  // Storage must be an unsigned integral type
  static_assert(std::is_unsigned_v<Storage>);
  static_assert(std::is_integral_v<Storage>);

  /** Storage type with const & volatile qualifiers removed. */
  using NvStorage = std::remove_cv_t<Storage>;

  constexpr NvStorage MakeMask(std::size_t width, std::size_t offset = 0) {
    return ((1U << width) - 1U) << offset;
  }

  /** Template function for implementing field getters. */
  template <std::size_t width, std::size_t offset>
  Storage ReadField() {
    return (this->storage_ & MakeMask(width, offset)) >> offset;
  }

  /** Template function for implementing field setters. */
  template <std::size_t width, std::size_t offset>
  void WriteField(NvStorage value) {
    auto mask = MakeMask(width, offset);
    this->storage_ = (this->storage_ & ~mask) | ((value << offset) & mask);
  }

  Storage storage_;
};

} // namespace libpandobox
} // namespace pnd
