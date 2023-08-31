#pragma once

#include "hdf5_comptype_accessor.h"

#include <H5Cpp.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>

namespace pnd {
namespace pando {

/** Base class for ProtobufH5RowTranscoder and ProtobufH5ColTranscoder */
class ProtobufH5TranscoderBase {
 public:
  class ProtobufH5TranscoderException : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  /** Struct for specifying an explicit HDF5 field type */
  struct FieldTypeOverride {
    std::string field_name;
    HDF5CompTypeFieldDescriptor::CppType type;
  };

  ProtobufH5TranscoderBase(const google::protobuf::Message& prototype)
      : descriptor_{prototype.GetDescriptor()}, reflection_{prototype.GetReflection()} {};

  /** Serialize a protobuf message in the format specified by the generated HDF5 compound type. */
  void Serialize(const google::protobuf::Message& msg, std::vector<char>& dst) const;
  // TODO
  // void DeSerialize(const void* src, google::protobuf::Message* msg) const ;

  /** Get a reference to the programatically generate HDF5 compound type. */
  const H5::CompType& GetH5CompType() const {
    return accessor_.GetH5Type();
  }

 protected:
  /** Copies a particular field out of a protobuf messages.
   * Constructor is provided with all the information required to perform this task, then operator()
   * is called over and over again on different protobuf messages.
   */
  class FieldCopyFunctor {
   public:
    /** Type of a function pointer to one of the field copier template specializations. */
    using CopyFuncPointer = void (ProtobufH5TranscoderBase::*)(
        const google::protobuf::Message&,
        const google::protobuf::FieldDescriptor*,
        const HDF5CompTypeFieldDescriptor*,
        std::vector<char>&,
        size_t) const;

    FieldCopyFunctor(
        ProtobufH5TranscoderBase* parent,
        const google::protobuf::FieldDescriptor* proto_fd,
        const HDF5CompTypeFieldDescriptor* h5_fd,
        CopyFuncPointer copy_func)
        : parent_{parent}, proto_fd_{proto_fd}, h5_fd_{h5_fd}, copy_func_{copy_func} {}

    /** Call the copier function with the correct arguments */
    void operator()(const google::protobuf::Message& msg, std::vector<char>& dst, size_t offset)
        const {
      std::invoke(copy_func_, parent_, msg, proto_fd_, h5_fd_, dst, offset);
    }

   private:
    ProtobufH5TranscoderBase* parent_;
    const google::protobuf::FieldDescriptor* proto_fd_;
    const HDF5CompTypeFieldDescriptor* h5_fd_;
    CopyFuncPointer copy_func_;
  };

  /** Called inside of Serialize to resize the output vector.
   * Implemented differently for ProtobufH5RowTranscoder and ProtobufH5ColTranscoder. */
  virtual void ResizeDst(const google::protobuf::Message& msg, std::vector<char>& dst) const = 0;

  /** Select an appropriate HDF5 type for a given protobuf field.
   * @param field_desc Descriptor for the field
   * @return HDF5CompTypeFieldDescriptor::CppType member
   */
  static HDF5CompTypeFieldDescriptor::CppType TypeOfField(
      const google::protobuf::FieldDescriptor* field_desc) {
    switch (field_desc->cpp_type()) {
#define HANDLE_TYPE(PROTO_CPPTYPE, H5_CPPTYPE)                       \
  case google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE: { \
    return HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE;        \
  }
      HANDLE_TYPE(INT32, INT32);
      HANDLE_TYPE(INT64, INT64);
      HANDLE_TYPE(UINT32, UINT32);
      HANDLE_TYPE(UINT64, UINT64);
      HANDLE_TYPE(DOUBLE, DOUBLE);
      HANDLE_TYPE(FLOAT, FLOAT);
      HANDLE_TYPE(ENUM, INT32);
#undef HANDLE_TYPE
      default:
        throw ProtobufH5TranscoderException("Unsupported field type");
    }
  }

  /** Copy a single repeated field from a protobuf message into a buffer arranged according to an
   * HDF5 compound type.
   * @tparam pb_type Enum specifying the type of the field in the source protobuf message
   * @tparam h5_type Enum specifying the type of the field in the destination HDF5 Compound type
   */
  template <
      google::protobuf::FieldDescriptor::CppType pb_type,
      HDF5CompTypeFieldDescriptor::CppType h5_type>
  void CopyRepeatedField(
      const google::protobuf::Message& msg,
      const google::protobuf::FieldDescriptor* proto_fd,
      const HDF5CompTypeFieldDescriptor* h5_fd,
      std::vector<char>& dst,
      size_t offset) const {
    if (reflection_->FieldSize(msg, proto_fd) != h5_fd->GetArrayLen()) {
      throw ProtobufH5TranscoderException("Field size differs from that seen in prototype");
    }

    // GetRepeatedField is deprecated in favor of GetRepeatedFieldRef, but we're using it anyways
    // because access via a RepeatedFieldRef is slow.
    auto& rf = reflection_->GetRepeatedField<PbTypeTraits<pb_type>::Rep>(msg, proto_fd);
    auto output_it = accessor_.GetFieldP<h5_type>(dst, h5_fd, offset);
    std::copy(rf.begin(), rf.end(), output_it);
  }

  /** Copy a single non-repeated field from a protobuf message into a buffer arranged according to
   * an HDF5 compound type.
   * @tparam pb_type Enum specifying the type of the field in the source protobuf message
   * @tparam h5_type Enum specifying the type of the field in the destination HDF5 Compound type
   */
  template <
      google::protobuf::FieldDescriptor::CppType pb_type,
      HDF5CompTypeFieldDescriptor::CppType h5_type>
  void CopySingularField(
      const google::protobuf::Message& msg,
      const google::protobuf::FieldDescriptor* proto_fd,
      const HDF5CompTypeFieldDescriptor* h5_fd,
      std::vector<char>& dst,
      size_t offset) const {
    auto val = std::invoke(PbTypeTraits<pb_type>::getter, reflection_, msg, proto_fd);
    accessor_.SetField<h5_type>(dst, h5_fd, offset, val);
  }

  /** Copy a single repeated field from a protobuf message into multiple buffers arranged according
   * to an HDF5 compound type.
   * This is used by ProtobufH5ColTranscoder.
   * @tparam pb_type Enum specifying the type of the field in the source protobuf message
   * @tparam h5_type Enum specifying the type of the field in the destination HDF5 Compound type
   */
  template <
      google::protobuf::FieldDescriptor::CppType pb_type,
      HDF5CompTypeFieldDescriptor::CppType h5_type>
  void CopyRepeatedFieldStrided(
      const google::protobuf::Message& msg,
      const google::protobuf::FieldDescriptor* proto_fd,
      const HDF5CompTypeFieldDescriptor* h5_fd,
      std::vector<char>& dst,
      size_t offset) const {
    // Require that all fields have the same field size
    auto field_size = reflection_->FieldSize(msg, descriptor_->field(0));
    if (reflection_->FieldSize(msg, proto_fd) != field_size) {
      throw ProtobufH5TranscoderException("Field size differs from others in same message");
    }

    // GetRepeatedField is deprecated in favor of GetRepeatedFieldRef, but we're using it anyways
    // because access via a RepeatedFieldRef is slow.
    auto& rf = reflection_->GetRepeatedField<PbTypeTraits<pb_type>::Rep>(msg, proto_fd);
    size_t row_idx = 0;
    for (auto val : rf) {
      accessor_.SetField<h5_type>(dst, h5_fd, offset + (row_idx++), val);
    }
  }

  const google::protobuf::Descriptor* const descriptor_;
  const google::protobuf::Reflection* const reflection_;

  HDF5CompTypeAccessor accessor_;
  std::vector<FieldCopyFunctor> field_copy_functors_;

 private:
  // Each specialization of PbTypeTraits associates a member of enum
  // google::protobuf::FieldDescriptor::CppType with:
  //   1. A fundamental type (eg. CPPTYPE_INT32 -> google::protobuf::int32)
  //   2. A pointer to a getter function (eg. CPPTYPE_INT32->
  //      &google::protobuf::Reflection::GetInt32)
  template <google::protobuf::FieldDescriptor::CppType type>
  struct PbTypeTraits;
#define ADD_TRAITS(PROTO_CPPTYPE, T, GETTER)                                           \
  template <>                                                                          \
  struct PbTypeTraits<google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE> {    \
    using Rep = T;                                                                     \
    using GetterFuncPointer = T (google::protobuf::Reflection::*)(                     \
        const google::protobuf::Message&,                                              \
        const google::protobuf::FieldDescriptor*) const;                               \
    static constexpr GetterFuncPointer getter = &google::protobuf::Reflection::GETTER; \
  };

  ADD_TRAITS(INT32, google::protobuf::int32, GetInt32);
  ADD_TRAITS(INT64, google::protobuf::int64, GetInt64);
  ADD_TRAITS(UINT32, google::protobuf::uint32, GetUInt32);
  ADD_TRAITS(UINT64, google::protobuf::uint64, GetUInt64);
  ADD_TRAITS(FLOAT, float, GetFloat);
  ADD_TRAITS(DOUBLE, double, GetDouble);
  ADD_TRAITS(ENUM, int, GetEnumValue);
#undef ADD_TRAITS
};

/** A class that performs custom serialization/deserialization of protobuf messges into/out of
 * buffers arranged according to a programatically generated HDF5 compound type.
 *
 * This version serializes data for a dataset where each row represents a single packet. It
 * implements a translation from a protobuf message type to an analogous HDF5 compound type.
 */
class ProtobufH5RowTranscoder : public ProtobufH5TranscoderBase {
 public:
  /** Generate an HDF5 compound type based on a prototype and prepare to serialize into it.
   *
   * The prototype is used to learn the length of any repeated fields in the message (these will be
   * represented as HDF5 array types, which have constant length.)
   *
   * @param prototype A protobuf message of the same type that this instance will operate on
   * @param overrides Optionally specifies explicit HDF5 types for particular message fields
   */
  ProtobufH5RowTranscoder(
      const google::protobuf::Message& prototype,
      const std::vector<FieldTypeOverride>& overrides = {});

 private:
  void ResizeDst(const google::protobuf::Message& msg, std::vector<char>& dst) const override;
};

/** A class that performs custom serialization/deserialization of protobuf messges into/out of
 * buffers arranged according to a programatically generated HDF5 compound type.
 *
 * This version serializes data for a dataset where each packet spread over a variable number of
 * rows. It implements a translation from a protobuf message type containing only repeated fields
 * (each one with the same number of elements) to an array of HDF5 compound types.
 *
 * This is meant to be used for logging time tags.
 */
class ProtobufH5ColTranscoder : public ProtobufH5TranscoderBase {
 public:
  /** Generate an HDF5 compound type based on a prototype and prepare to serialize into it.
   * @param prototype A protobuf message of the same type that this instance will operate on
   * @param overrides Optionally specifies explicit HDF5 types for particular message fields
   */
  ProtobufH5ColTranscoder(
      const google::protobuf::Message& prototype,
      const std::vector<FieldTypeOverride>& overrides = {});

 private:
  void ResizeDst(const google::protobuf::Message& msg, std::vector<char>& dst) const override;
};

} // namespace pando
} // namespace pnd
