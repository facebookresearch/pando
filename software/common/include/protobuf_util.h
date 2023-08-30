#pragma once

#include <zmq.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>

#include <algorithm> // std:copy
#include <cassert>
#include <iterator> // std::distance
#include <stdexcept>

namespace pnd {
namespace common {

/** Serialize a protobuf message directly into a ZMQ message.
 * @param buf the protobuf message to serialize
 * @param msg the zmq message_t to write into
 */
static inline void ProtobufSerialize(
    const google::protobuf::MessageLite& buf,
    zmq::message_t& msg) {
  msg.rebuild(buf.ByteSizeLong());
  google::protobuf::io::ArrayOutputStream array(msg.data(), msg.size());
  google::protobuf::io::CodedOutputStream coded(&array);

  buf.SerializeWithCachedSizes(&coded);
}

/** Deserialize a protobuf message directly from a ZMQ message.
 * @param msg the zmq message_t to read from
 * @param buf the protobuf message to deserialize into
 */
static inline void ProtobufDeserialize(
    const zmq::message_t& msg,
    google::protobuf::MessageLite& buf) {
  google::protobuf::io::ArrayInputStream array(msg.data(), msg.size());
  google::protobuf::io::CodedInputStream coded(&array);

  auto ret = buf.ParseFromCodedStream(&coded);
  if (!ret) {
    throw std::runtime_error("ProtobufDeserialize: encountered malformed data");
  }

  if (!coded.ConsumedEntireMessage()) {
    throw std::runtime_error("ProtobufDeserialize: parsing didn't consume all bytes");
  }
}

/** Copy a range of elements into a RepeatedField.
 * Yes, we really do have to implement this ourselves. It's only useful when the RepeatedField being
 * assigned to is allocated on an arena, because move assigning a temporary RepeatedField
 * constructred off of the arena using the range constructor requires an additional copy operation.
 * @param src_begin The beginning of the source range
 * @param src_end The end of the source range
 * @param dest RepeatedField instance to copy into
 */
template <class InputIt, class Element>
void AssignRepeatedField(
    InputIt src_begin,
    InputIt src_end,
    google::protobuf::RepeatedField<Element>* dest) {
  // Copied from implementation of RepeatedField<Element>::CopyFrom
  auto new_size = std::distance(src_begin, src_end);
  dest->Clear();
  dest->Reserve(new_size);
  dest->AddNAlreadyReserved(new_size);
  assert(std::distance(dest->begin(), dest->end()) == new_size);
  std::copy(src_begin, src_end, dest->begin());
}
} // namespace common
} // namespace pnd
