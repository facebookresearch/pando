#pragma once

#include "hdf5_comptype_field_descriptor.h"
#include "mutex.h"

#include <H5Cpp.h>

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace pnd {
namespace pando {

/** Helper class for accessing fields inside buffers arranged according to an HDF5 compound type. */
class HDF5CompTypeAccessor {
 public:
  /** Argument type for the constructor which generates a new H5::CompType.
   * This class just exists so that we can repeatedly call AddField before calling the constructor
   * of HDF5CompTypeAccessor.
   */
  class CompTypeSpecifier {
   public:
    /** Add a field (scalar or 1D array) to the compound type specifier.
     * @param name The name of the field
     * @param type The type of the field (or of a each element of an array field)
     * @param array_len The element count for array fields, or 1 for scalar fields
     */
    void AddField(
        const std::string& name,
        HDF5CompTypeFieldDescriptor::CppType type,
        hsize_t array_len = 1) {
      dtype_size_ += HDF5CompTypeFieldDescriptor::GetPredType(type).getSize() * array_len;
      field_specifiers_.push_back(FieldSpecifier{name, type, array_len});
    }

   private:
    friend class HDF5CompTypeAccessor;

    struct FieldSpecifier {
      std::string name;
      HDF5CompTypeFieldDescriptor::CppType type;
      hsize_t array_len = 1;
    };

    std::vector<FieldSpecifier> field_specifiers_;
    hsize_t dtype_size_ = 0;
  };

  /** Construct an accessor with no associated H5::CompType. */
  HDF5CompTypeAccessor() = default;

  /** Constructor which generates a new H5::CompType
   * @param specifier Specification of members in the new H5::CompType that this instance will
   * create and operate on.
   */
  HDF5CompTypeAccessor(const CompTypeSpecifier& specifier)
      : dtype_{static_cast<size_t>(specifier.dtype_size_)}, dtype_size_{specifier.dtype_size_} {
    H5LockGuard l;

    // Build the compound type
    hsize_t offset = 0;
    for (auto& fs : specifier.field_specifiers_) {
      auto& pred_type = HDF5CompTypeFieldDescriptor::GetPredType(fs.type);

      hsize_t field_size = 0;
      if (fs.array_len == 1) {
        dtype_.insertMember(fs.name, offset, pred_type);
        field_size = pred_type.getSize();
      } else {
        H5::ArrayType array_type{pred_type, 1, &fs.array_len};
        dtype_.insertMember(fs.name, offset, array_type);
        field_size = array_type.getSize();
      }

      // Make field descriptor
      descriptors_.emplace(fs.name, HDF5CompTypeFieldDescriptor{fs.type, fs.array_len, offset});
      offset += field_size;
    }
  }

  /** Constructor which takes an existing H5::CompType.
   * @param dtype The compound type this instance will operate on
   */
  HDF5CompTypeAccessor(const H5::DataType& dtype)
      : dtype_{dtype.getId()}, dtype_size_{dtype.getSize()} {
    H5LockGuard l;

    if (dtype.getClass() != H5T_COMPOUND) {
      throw std::runtime_error("HDF5CompTypeAccessor: dtype is not a compound type");
    }

    for (int idx = 0; idx < dtype_.getNmembers(); ++idx) {
      // Get field data
      std::string name = dtype_.getMemberName(idx);
      size_t offset = dtype_.getMemberOffset(idx);

      // Deduce field type and array length
      HDF5CompTypeFieldDescriptor::CppType type;
      hsize_t array_len;
      H5::DataType member_type = dtype_.getMemberDataType(idx);
      if (member_type.getClass() == H5T_ARRAY) {
        H5::DataType element_type = member_type.getSuper();
        type = HDF5CompTypeFieldDescriptor::GetCppType(element_type);

        auto array_size = member_type.getSize();
        auto element_size = element_type.getSize();
        assert(array_size % element_size == 0);
        array_len = array_size / element_size;
      } else {
        type = HDF5CompTypeFieldDescriptor::GetCppType(member_type);
        array_len = 1;
      }

      // Make field descriptor
      descriptors_.emplace(name, HDF5CompTypeFieldDescriptor{type, array_len, offset});
    }
  }

  /** Look up a HDF5CompTypeFieldDescriptor for a single field contained in this compound type.
   * @param name The name of the field to look up
   * @return A pointer to a HDF5CompTypeFieldDescriptor for the named field, or nullptr if no such
   * field existed.
   */
  const HDF5CompTypeFieldDescriptor* GetFieldDescriptor(const std::string& name) const {
    // Check that a field with this name exits
    auto kv_it = descriptors_.find(name);
    if (kv_it != descriptors_.end()) {
      return &(kv_it->second);
    } else {
      return nullptr;
    }
  }

  /** Get a pointer to a field inside a buffer of bytes arranged according to the compound type.
   * The buffer may contain multiple adjacent instances of the compound type.
   * @param buf the sequence of bytes to operate on
   * @param fd  Descriptor for the field to access
   * @param idx Index of the compound type instance to operate on
   *
   * @tparam type The type to return a pointer to
   * @tparam V the type of the provided buffer. Must be a sequence container of bytes
   * (std::vector<uint8_t>, std::string, etc.)
   */
  template <HDF5CompTypeFieldDescriptor::CppType type, class V>
  auto GetFieldP(const V& buf, const HDF5CompTypeFieldDescriptor* fd, size_t idx) const {
    const typename V::value_type* ptr = buf.data() + GetFieldOffset<type, V>(buf, fd, idx);
    return reinterpret_cast<const typename HDF5CompTypeFieldDescriptor::TypeTraits<type>::Rep*>(
        ptr);
  }

  /** @overload */
  template <HDF5CompTypeFieldDescriptor::CppType type, class V>
  auto GetFieldP(V& buf, const HDF5CompTypeFieldDescriptor* fd, size_t idx) const {
    typename V::value_type* ptr = buf.data() + GetFieldOffset<type, V>(buf, fd, idx);
    return reinterpret_cast<typename HDF5CompTypeFieldDescriptor::TypeTraits<type>::Rep*>(ptr);
  }

  /** Convenience function to call GetFieldP and return the value at the pointed-to location. */
  template <HDF5CompTypeFieldDescriptor::CppType type, class V>
  auto GetField(const V& buf, const HDF5CompTypeFieldDescriptor* fd, size_t idx) const {
    return *GetFieldP<type, V>(buf, fd, idx);
  }

  /** Convenience function to call GetFieldP and assign a value to the pointed-to location. */
  template <HDF5CompTypeFieldDescriptor::CppType type, class V>
  void SetField(
      V& buf,
      const HDF5CompTypeFieldDescriptor* fd,
      size_t idx,
      typename HDF5CompTypeFieldDescriptor::TypeTraits<type>::Rep value) const {
    *GetFieldP<type, V>(buf, fd, idx) = value;
  }

  const H5::CompType& GetH5Type() const {
    return dtype_;
  }

  hsize_t GetH5TypeSize() const {
    return dtype_size_;
  }

 private:
  template <HDF5CompTypeFieldDescriptor::CppType type, class V>
  size_t GetFieldOffset(const V& buf, const HDF5CompTypeFieldDescriptor* fd, size_t idx) const {
    static_assert(
        sizeof(typename V::value_type) == sizeof(uint8_t), "size of buf value_type must be 1");

    // Check that pointer isn't nullptr
    assert(fd);

    // Check that the correct specialization is being used
    assert(fd->type_ == type);

    auto buf_offset = idx * dtype_size_ + fd->offset_;

    // Check that we won't read past the end of buf
    auto end = buf_offset + HDF5CompTypeFieldDescriptor::TypeTraits<type>::size * fd->array_len_;
    assert(end <= buf.size());

    return buf_offset;
  }

  H5LockAdapter<H5::CompType> dtype_;
  hsize_t dtype_size_ = 0;
  std::map<std::string, HDF5CompTypeFieldDescriptor> descriptors_;
};
} // namespace pando
} // namespace pnd
