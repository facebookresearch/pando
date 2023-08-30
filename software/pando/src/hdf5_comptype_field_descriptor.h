#pragma once

#include "mutex.h"

#include <H5Cpp.h>

#include <cstdint>

namespace pnd {
namespace pando {

class HDF5CompTypeFieldDescriptor {
 public:
  /** Enumeration of types that we support as members of a compound type */
  enum CppType {
    CPPTYPE_INT8,
    CPPTYPE_INT16,
    CPPTYPE_INT32,
    CPPTYPE_INT64,
    CPPTYPE_UINT8,
    CPPTYPE_UINT16,
    CPPTYPE_UINT32,
    CPPTYPE_UINT64,
    CPPTYPE_FLOAT,
    CPPTYPE_DOUBLE,
    MAX_CPPTYPE
  };

  auto GetCppType() const {
    return type_;
  };

  auto GetArrayLen() const {
    return array_len_;
  };

 private:
  template <class T>
  struct TypeTraitsImpl {
    using Rep = T; /**< Arithmetic type used to store the field value. */
    static constexpr size_t size = sizeof(Rep); /**< Size of Rep */
  };

 public:
  /** Mapping of the members of enum CppType to fundamental types. */
  template <CppType type>
  struct TypeTraits;
  template <>
  struct TypeTraits<CPPTYPE_INT8> : TypeTraitsImpl<int8_t> {};
  template <>
  struct TypeTraits<CPPTYPE_INT16> : TypeTraitsImpl<int16_t> {};
  template <>
  struct TypeTraits<CPPTYPE_INT32> : TypeTraitsImpl<int32_t> {};
  template <>
  struct TypeTraits<CPPTYPE_INT64> : TypeTraitsImpl<int64_t> {};
  template <>
  struct TypeTraits<CPPTYPE_UINT8> : TypeTraitsImpl<uint8_t> {};
  template <>
  struct TypeTraits<CPPTYPE_UINT16> : TypeTraitsImpl<uint16_t> {};
  template <>
  struct TypeTraits<CPPTYPE_UINT32> : TypeTraitsImpl<uint32_t> {};
  template <>
  struct TypeTraits<CPPTYPE_UINT64> : TypeTraitsImpl<uint64_t> {};
  template <>
  struct TypeTraits<CPPTYPE_FLOAT> : TypeTraitsImpl<float> {};
  template <>
  struct TypeTraits<CPPTYPE_DOUBLE> : TypeTraitsImpl<double> {};

  /** Mapping of the members of enum CppType to H5::PredType pseudo-constants. */
  static const H5::PredType& GetPredType(CppType type) {
    switch (type) {
      case CPPTYPE_INT8:
        return H5::PredType::NATIVE_INT8;
      case CPPTYPE_INT16:
        return H5::PredType::NATIVE_INT16;
      case CPPTYPE_INT32:
        return H5::PredType::NATIVE_INT32;
      case CPPTYPE_INT64:
        return H5::PredType::NATIVE_INT64;
      case CPPTYPE_UINT8:
        return H5::PredType::NATIVE_UINT8;
      case CPPTYPE_UINT16:
        return H5::PredType::NATIVE_UINT16;
      case CPPTYPE_UINT32:
        return H5::PredType::NATIVE_UINT32;
      case CPPTYPE_UINT64:
        return H5::PredType::NATIVE_UINT64;
      case CPPTYPE_FLOAT:
        return H5::PredType::NATIVE_FLOAT;
      case CPPTYPE_DOUBLE:
        return H5::PredType::NATIVE_DOUBLE;
      default:
        throw std::runtime_error("GetPredType: Unknown CppType");
    }
  }

  /** Mapping of H5::PredType pseudo-constants to the members of enum CppType.
   * There's not a great way of doing this since the PredTypes are a) Not real types (they're
   * const references to H5::PredType instances) and b) Not compile time constants (They're
   * dynamically initialized). */
  static CppType GetCppType(const H5::DataType& pred_type) {
    H5LockGuard l;

    for (int i = 0; i != MAX_CPPTYPE; ++i) {
      auto type = static_cast<CppType>(i);
      if (GetPredType(type) == pred_type) { // equality operator calls H5Tequal()
        return type;
      }
    }
    throw std::runtime_error("GetCppType: Unknown PredType");
  }

 private:
  friend class HDF5CompTypeAccessor;

  /** Private constructor, accessible only by HDF5CompTypeAccessor */
  HDF5CompTypeFieldDescriptor(CppType type, size_t array_len, size_t offset)
      : type_{type}, array_len_{array_len}, offset_{offset} {};

  CppType type_; /**< The type of a single element in the field */
  size_t array_len_; /**< The number of elements in the field (1 for non array types) */
  size_t offset_; /**< The byte offset of the field in the compound type */
};

} // namespace pando
} // namespace pnd
