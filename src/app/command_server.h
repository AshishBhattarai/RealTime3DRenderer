#pragma once

#include "command_queues.h"
#include "third_party/asio_noexcept.h"
#include <memory>
#include <vector>

namespace app {
class CommandServer {
  class Connection;

public:
  CommandServer(uint port, uint threadPoolSize);
  ~CommandServer();

  void start();

  CommandDto::RTSPConnection popConnectionQueue(bool discard = false) {
    CommandDto::RTSPConnection dto;
    commandQueues.connectionQueue.popGetFront(dto, discard);
    return dto;
  }

private:
  CommandQueues commandQueues;
  asio::io_service ios;
  asio::ip::tcp::acceptor acceptor;
  asio::executor_work_guard<asio::io_context::executor_type> work;
  std::vector<std::thread> threads;
  std::vector<std::weak_ptr<Connection>> connections;
  void handleAccept(std::shared_ptr<Connection> connection, const asio::error_code &ec);
};
} // namespace app
