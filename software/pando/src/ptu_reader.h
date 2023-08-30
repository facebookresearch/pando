#pragma once

#include <cstdint>
#include <exception>
#include <fstream>

namespace pnd {
namespace pando {

class PTUReader {
 public:
  class PTUReaderException : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  using Record = uint32_t;

  PTUReader(const std::string& filename, uint32_t record_type);
  ~PTUReader();
  size_t GetRecords(Record* dest, size_t count);
  void Rewind();
  double getResolution() {
    return resolution_;
  }

  // Selected record types
  // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $03 (PicoHarp)
  static constexpr uint32_t kRtPicoHarp2T3 = 0x00010303;
  // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $03 (PicoHarp)
  static constexpr uint32_t kRtPicoHarp2T2 = 0x00010203;
  // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $03 (T3), HW: $04 (HydraHarp)
  static constexpr uint32_t kRtHydraHarp2T3 = 0x01010304;
  // (SubID = $01 ,RecFmt: $01) (V2), T-Mode: $02 (T2), HW: $04 (HydraHarp)
  static constexpr uint32_t kRtHydraHarp2T2 = 0x01010204;
  // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $03 (T3), HW: $07 (MultiHarp150N)
  static constexpr uint32_t kRtMultiHarp2T3 = 0x00010307;
  // (SubID = $00 ,RecFmt: $01) (V1), T-Mode: $02 (T2), HW: $07 (MultiHarp150N)
  static constexpr uint32_t kRtMultiHarp2T2 = 0x00010207;

 private:
  struct HeaderTag {
    char identifier[32]; // Identifier of the tag
    int32_t index; // Index for multiple tags or -1
    uint32_t type; // Type of tag ty..... see const section
    uint64_t value; // Value of tag.
  };

  // Selected header tag IDs
  static constexpr char TTTRTagTTTRRecType[] = "TTResultFormat_TTTRRecType";
  static constexpr char TTTRTagNumRecords[] = "TTResult_NumberOfRecords";
  static constexpr char TTTRTagRes[] = "MeasDesc_Resolution";
  static constexpr char TTTRTagGlobRes[] = "MeasDesc_GlobalResolution";
  static constexpr char FileTagEnd[] = "Header_End";
  static constexpr char HWBaseResolution[] = "HW_BaseResolution";

  // Header tag types
  static constexpr uint32_t tyEmpty8 = 0xFFFF0008;
  static constexpr uint32_t tyBool8 = 0x00000008;
  static constexpr uint32_t tyInt8 = 0x10000008;
  static constexpr uint32_t tyBitSet64 = 0x11000008;
  static constexpr uint32_t tyColor8 = 0x12000008;
  static constexpr uint32_t tyFloat8 = 0x20000008;
  static constexpr uint32_t tyTDateTime = 0x21000008;
  static constexpr uint32_t tyFloat8Array = 0x2001FFFF;
  static constexpr uint32_t tyAnsiString = 0x4001FFFF;
  static constexpr uint32_t tyWideString = 0x4002FFFF;
  static constexpr uint32_t tyBinaryBlob = 0xFFFFFFFF;

  const char* RecTypeToString(uint64_t rt);

  std::ifstream file_;
  std::ifstream::pos_type first_record_;

  int64_t num_records_ = -1;
  uint64_t record_type_ = 0;
  double resolution_ = 0.0;
  double base_resolution_ = 0.0;
  double global_resolution_ = 0.0;
};

} // namespace pando
} // namespace pnd
