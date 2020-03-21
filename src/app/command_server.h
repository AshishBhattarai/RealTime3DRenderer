#pragma once

#include "asio_noexcept.h"
#include <memory>
#include <vector>

namespace app {
class CommandServer {
  class Connection;

public:
  CommandServer(uint port, uint threadPoolSize);
  ~CommandServer();

  void start();

private:
  asio::io_service ios;
  asio::ip::tcp::acceptor acceptor;
  asio::executor_work_guard<asio::io_context::executor_type> work;
  std::vector<std::thread> threads;
  std::vector<std::weak_ptr<Connection>> connections;
  void handleAccept(std::shared_ptr<Connection> connection,
                    const asio::error_code &ec);
};
} // namespace app
