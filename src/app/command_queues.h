#pragma once

#include "core/shared_queue.h"
#include "types.h"
#include <string>

namespace CommandDto {
struct RTSPConnection {
  std::string ip;
  u16 port;

  RTSPConnection(std::string ip, u16 port) : ip(ip), port(port) {}
  RTSPConnection() : ip(""), port(0) {}

  std::string toRtspEndpoint() const {
    return "rtsp://" + ip + ":" + std::to_string(port) + "/renderstream";
  }
};
} // namespace CommandDto

struct CommandQueues {
  SharedQueue<CommandDto::RTSPConnection> connectionQueue;
};
