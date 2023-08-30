#pragma once

#include <cstdint>
#include <list>
#include <string>

#include "pandoboxd_rpc.pb.h"

namespace pnd {
namespace pandoboxd {

struct PandoboxdInterface {
  static constexpr char kProxyEgressTcpEp[] = "tcp://*:5064";
  static constexpr char kProxyIngressInprocEp[] = "inproc://pandoboxd_proxy_ingress";

  /** Error Codes ( Enumerator values copied from protobuf schema) */
  enum class ErrorNum {
    NONE = proto::PandoboxdTransaction::STATUS_OK,
    UNSPECIFIED = proto::PandoboxdTransaction::STATUS_ERR_UNSPECIFIED,
    UNIMPLEMENTED = proto::PandoboxdTransaction::STATUS_ERR_UNIMPLEMENTED,
    ALREADY_STARTED = proto::PandoboxdTransaction::STATUS_ERR_ALREADY_STARTED,
    ALREADY_STOPPED = proto::PandoboxdTransaction::STATUS_ERR_ALREADY_STOPPED,
  };

  struct Config {
    uint32_t camera_frame_trigger_0_period_10ns;
    uint32_t camera_frame_trigger_1_period_10ns;
  };

  struct Report {
    std::string severity;
    std::string message;
  };

  using Reports = std::list<Report>;

  virtual ~PandoboxdInterface() = default;

  virtual ErrorNum Ping() = 0;
  virtual ErrorNum Start(int32_t experiment_id, const Config& config) = 0;
  virtual ErrorNum Stop() = 0;
  virtual ErrorNum GetReports(std::list<Report>& dest) = 0;
};

} // namespace pandoboxd
} // namespace pnd
