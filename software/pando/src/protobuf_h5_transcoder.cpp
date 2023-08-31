#include "protobuf_h5_transcoder.h"

#include <google/protobuf/reflection.h>

#include <cassert>
#include <vector>

namespace pnd {
namespace pando {

void ProtobufH5TranscoderBase::Serialize(
    const google::protobuf::Message& msg,
    std::vector<char>& dst) const {
  // Require that msg has same descriptor as prototype did
  if (msg.GetDescriptor() != descriptor_) {
    throw ProtobufH5TranscoderException("Descriptor doesn't match that of prototype");
  }

  // Determine how many messages have already been serialized into dst
  assert(dst.size() % accessor_.GetH5TypeSize() == 0);
  auto offset = dst.size() / accessor_.GetH5TypeSize();

  // Allocate space for one more message at the end of dst
  ResizeDst(msg, dst);

  // Copy each field in msg to dst
  for (auto& functor : field_copy_functors_) {
    functor(msg, dst, offset);
  }
}

ProtobufH5RowTranscoder::ProtobufH5RowTranscoder(
    const google::protobuf::Message& prototype,
    const std::vector<FieldTypeOverride>& overrides)
    : ProtobufH5TranscoderBase(prototype) {
  // Enumerate the fields in this message, collecting info needed to build the compound HDF5 type
  HDF5CompTypeAccessor::CompTypeSpecifier comptype_specifier;
  for (int field_idx = 0; field_idx < descriptor_->field_count(); ++field_idx) {
    auto field_desc = descriptor_->field(field_idx);

    const std::string& name = field_desc->name();
    HDF5CompTypeFieldDescriptor::CppType field_type;

    auto find_pred = [&name](const FieldTypeOverride& ftr) { return ftr.field_name == name; };
    auto overide = std::find_if(overrides.cbegin(), overrides.cend(), find_pred);
    if (overide != overrides.cend()) {
      field_type = overide->type;
    } else {
      field_type = TypeOfField(field_desc);
    }

    // Discover element count of repeated fields
    size_t field_size = 1;
    if (field_desc->label() == google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      field_size = reflection_->FieldSize(prototype, field_desc);
      if (field_size == 0) {
        throw ProtobufH5TranscoderException("Prototype has empty repeated field");
      }
    }

    comptype_specifier.AddField(name, field_type, field_size);
  }

  // Build the compound HDF5 type
  accessor_ = HDF5CompTypeAccessor{comptype_specifier};

  // Create a FieldCopyFunctor for each field in the message
  for (int field_idx = 0; field_idx < descriptor_->field_count(); ++field_idx) {
    auto proto_fd = descriptor_->field(field_idx);
    auto h5_fd = accessor_.GetFieldDescriptor(proto_fd->name());
    assert(h5_fd);

    // Select a copier function based on the protobuf & CompTypeAccessor field types
    if (proto_fd->label() == google::protobuf::FieldDescriptor::LABEL_REPEATED) {
      // Handle repeated fields
#define SUPPORT_TYPES(PROTO_CPPTYPE, H5_CPPTYPE)                                            \
  if (proto_fd->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE && \
      h5_fd->GetCppType() == HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE) {           \
    auto copy_func = &ProtobufH5RowTranscoder::CopyRepeatedField<                           \
        google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE,                         \
        HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE>;                                 \
    field_copy_functors_.push_back(FieldCopyFunctor{this, proto_fd, h5_fd, copy_func});     \
  } else

      SUPPORT_TYPES(INT32, INT8)
      SUPPORT_TYPES(INT32, INT16)
      SUPPORT_TYPES(INT32, INT32)
      SUPPORT_TYPES(INT64, INT64)
      SUPPORT_TYPES(UINT32, UINT8)
      SUPPORT_TYPES(UINT32, UINT16)
      SUPPORT_TYPES(UINT32, UINT32)
      SUPPORT_TYPES(UINT64, UINT64)
      SUPPORT_TYPES(FLOAT, FLOAT)
      SUPPORT_TYPES(DOUBLE, DOUBLE)
      SUPPORT_TYPES(ENUM, INT32)
#undef SUPPORT_TYPES
      {
        throw std::runtime_error("ProtobufH5RowTranscoder: Unsupported field type combination");
      }

    } else {
      // Handle non-repeated fields
#define SUPPORT_TYPES(PROTO_CPPTYPE, H5_CPPTYPE)                                            \
  if (proto_fd->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE && \
      h5_fd->GetCppType() == HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE) {           \
    auto copy_func = &ProtobufH5RowTranscoder::CopySingularField<                           \
        google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE,                         \
        HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE>;                                 \
    field_copy_functors_.push_back(FieldCopyFunctor{this, proto_fd, h5_fd, copy_func});     \
  } else

      SUPPORT_TYPES(INT32, INT8)
      SUPPORT_TYPES(INT32, INT16)
      SUPPORT_TYPES(INT32, INT32)
      SUPPORT_TYPES(INT64, INT64)
      SUPPORT_TYPES(UINT32, UINT8)
      SUPPORT_TYPES(UINT32, UINT16)
      SUPPORT_TYPES(UINT32, UINT32)
      SUPPORT_TYPES(UINT64, UINT64)
      SUPPORT_TYPES(FLOAT, FLOAT)
      SUPPORT_TYPES(DOUBLE, DOUBLE)
      SUPPORT_TYPES(ENUM, INT32)
#undef SUPPORT_TYPES
      {
        throw std::runtime_error("ProtobufH5RowTranscoder: Unsupported field type combination");
      }
    }
  }
}

void ProtobufH5RowTranscoder::ResizeDst(
    const google::protobuf::Message& msg,
    std::vector<char>& dst) const {
  dst.resize(dst.size() + accessor_.GetH5TypeSize());
}

ProtobufH5ColTranscoder::ProtobufH5ColTranscoder(
    const google::protobuf::Message& prototype,
    const std::vector<FieldTypeOverride>& overrides)
    : ProtobufH5TranscoderBase(prototype) {
  // Enumerate the fields in this message, collecting info needed to build the compound HDF5 type
  HDF5CompTypeAccessor::CompTypeSpecifier comptype_specifier;
  for (int field_idx = 0; field_idx < descriptor_->field_count(); ++field_idx) {
    auto field_desc = descriptor_->field(field_idx);

    // Require all fields to be repeated
    assert(field_desc->label() == google::protobuf::FieldDescriptor::LABEL_REPEATED);

    const std::string& name = field_desc->name();
    HDF5CompTypeFieldDescriptor::CppType field_type;

    auto find_pred = [&name](const FieldTypeOverride& ftr) { return ftr.field_name == name; };
    auto overide = std::find_if(overrides.cbegin(), overrides.cend(), find_pred);
    if (overide != overrides.cend()) {
      field_type = overide->type;
    } else {
      field_type = TypeOfField(field_desc);
    }

    comptype_specifier.AddField(name, field_type, 1);
  }

  // Build the compound HDF5 type
  accessor_ = HDF5CompTypeAccessor{comptype_specifier};

  // Create a FieldCopyFunctor for each field in the message
  for (int field_idx = 0; field_idx < descriptor_->field_count(); ++field_idx) {
    auto proto_fd = descriptor_->field(field_idx);
    auto h5_fd = accessor_.GetFieldDescriptor(proto_fd->name());
    assert(h5_fd);

    auto pt = proto_fd->cpp_type();
    auto ht = h5_fd->GetCppType();

#define SUPPORT_TYPES(PROTO_CPPTYPE, H5_CPPTYPE)                                            \
  if (proto_fd->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE && \
      h5_fd->GetCppType() == HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE) {           \
    auto copy_func = &ProtobufH5ColTranscoder::CopyRepeatedFieldStrided<                    \
        google::protobuf::FieldDescriptor::CPPTYPE_##PROTO_CPPTYPE,                         \
        HDF5CompTypeFieldDescriptor::CPPTYPE_##H5_CPPTYPE>;                                 \
    field_copy_functors_.push_back(FieldCopyFunctor{this, proto_fd, h5_fd, copy_func});     \
  } else

    SUPPORT_TYPES(INT32, INT8)
    SUPPORT_TYPES(INT32, INT16)
    SUPPORT_TYPES(INT32, INT32)
    SUPPORT_TYPES(INT64, INT64)
    SUPPORT_TYPES(UINT32, UINT8)
    SUPPORT_TYPES(UINT32, UINT16)
    SUPPORT_TYPES(UINT32, UINT32)
    SUPPORT_TYPES(UINT64, UINT64)
    SUPPORT_TYPES(FLOAT, FLOAT)
    SUPPORT_TYPES(DOUBLE, DOUBLE)
    SUPPORT_TYPES(ENUM, INT32)
#undef SUPPORT_TYPES
    {
      throw std::runtime_error("ProtobufH5ColTranscoder: Unsupported field type combination");
    }
  }
}

void ProtobufH5ColTranscoder::ResizeDst(
    const google::protobuf::Message& msg,
    std::vector<char>& dst) const {
  // Note length of first repeated field (they must all have the same length)
  auto field_size = reflection_->FieldSize(msg, descriptor_->field(0));
  dst.resize(dst.size() + accessor_.GetH5TypeSize() * field_size);
}

} // namespace pando
} // namespace pnd
