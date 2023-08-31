#include "ptu_reader.h"

#include "reporter.h"

#include <fmt/format.h>

namespace pnd {
namespace pando {

PTUReader::PTUReader(const std::string& filename, Record record_type) {
  g_reporter->debug("PTUReader: opening {}", filename);
  file_.open(filename, std::ios::in | std::ios::binary);

  if (!file_.is_open()) {
    throw PTUReaderException("Failed to open file");
  }

  char magic[8];
  if (!file_.read(magic, sizeof(magic))) {
    throw PTUReaderException("Failed to read magic");
  }

  if (std::string(magic) != "PQTTTR") {
    throw PTUReaderException("Bad magic");
  }

  char version[8];
  if (!file_.read(version, sizeof(version))) {
    throw PTUReaderException("Failed to read version");
  }

  // Read all tags in the header
  while (1) {
    HeaderTag tag;
    if (!file_.read(reinterpret_cast<char*>(&tag), sizeof(HeaderTag))) {
      throw PTUReaderException("Failed to read a HeaderTag");
    }

    std::string identifier(tag.identifier);

    if (identifier == FileTagEnd) {
      // This was the end of the header, next read will return records.
      first_record_ = file_.tellg();
      break;
    }

    switch (tag.type) {
      case tyEmpty8:
      case tyBool8:
      case tyBitSet64:
      case tyColor8:
      case tyTDateTime:
        // Ignore
        break;
      case tyInt8:
        if (identifier == TTTRTagNumRecords) {
          num_records_ = *reinterpret_cast<int64_t*>(&tag.value);
          g_reporter->debug("PTUReader: TTResult_NumberOfRecords is {}", num_records_);
        } else if (identifier == TTTRTagTTTRRecType) {
          record_type_ = tag.value;
          g_reporter->debug(
              "PTUReader: TTResultFormat_TTTRRecType is {}", RecTypeToString(record_type_));
        }

      case tyFloat8:
        if (identifier == TTTRTagRes) {
          resolution_ = *reinterpret_cast<double*>(&tag.value);
          g_reporter->debug("PTUReader: MeasDesc_Resolution is {}", resolution_);
        } else if (identifier == TTTRTagGlobRes) {
          global_resolution_ = *reinterpret_cast<double*>(&tag.value);
          g_reporter->debug("PTUReader: MeasDesc_GlobalResolution is {}", global_resolution_);
        } else if (identifier == HWBaseResolution) {
          base_resolution_ = *reinterpret_cast<double*>(&tag.value);
          g_reporter->debug("PTUReader: HW_BaseResolution is {}", base_resolution_);
        }
        break;
      case tyFloat8Array:
      case tyAnsiString:
      case tyWideString:
      case tyBinaryBlob:
        // Ignore, seek past the array
        if (!file_.seekg(tag.value, file_.cur)) {
          throw PTUReaderException("Seek failed");
        }
        break;
      default:
        throw PTUReaderException("Illegal Type identifier found! Broken file?");
    }
  }

  // Sanity check the info we recovered from the header
  if (record_type_ != record_type) {
    auto msg = fmt::format(
        "PTUReader: Expected record type {}, but PTU contains record type {}",
        RecTypeToString(record_type),
        RecTypeToString(record_type_));
    throw PTUReaderException(msg);
  }
}

void PTUReader::Rewind() {
  file_.clear(); // eof and fail bits were set if we reached eof
  file_.seekg(first_record_);
}

PTUReader::~PTUReader() {
  file_.close();
}

size_t PTUReader::GetRecords(Record* dest, size_t count) {
  file_.read(reinterpret_cast<char*>(dest), count * sizeof(Record));
  size_t n_read = file_.gcount();

  if (file_.bad()) {
    throw PTUReaderException("Stream badbit is set");
  }

  size_t n_records = n_read / sizeof(Record);
  return n_records;
}

const char* PTUReader::RecTypeToString(uint64_t rt) {
  switch (rt) {
    case kRtPicoHarp2T3:
      return "PicoHarp T3";
    case kRtPicoHarp2T2:
      return "PicoHarp T2";
    case kRtHydraHarp2T3:
      return "HydraHarp T3";
    case kRtHydraHarp2T2:
      return "HydraHarp T2";
    case kRtMultiHarp2T2:
      return "MultiHarp T2";
    case kRtMultiHarp2T3:
      return "MultiHarp T3";
    default:
      return "UNKNOWN";
  }
}

} // namespace pando
} // namespace pnd
