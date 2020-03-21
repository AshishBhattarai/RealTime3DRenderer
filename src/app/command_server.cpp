#include "command_server.h"
#include "utils/slogger.h"

using namespace asio;
using namespace asio::ip;

namespace app {
// connection receiving buffer size
namespace ReceiveBufferSize {
static constexpr uint MIN_BUFFER_SIZE = 4;
static constexpr uint MAX_BUFFER_SIZE = 14;
static constexpr uint HEADER_SIZE = 2;
} // namespace ReceiveBufferSize

class CommandServer::Connection
    : public std::enable_shared_from_this<Connection> {
public:
  Connection(io_service &ios) : sock(ios) { readBuf.fill(0); }
  ~Connection() { DEBUG_SLOG("Connection destoryed."); }

  std::shared_ptr<Connection> getPtr() { return shared_from_this(); }

  tcp::socket &getSocket() { return sock; }

  void start() {
    async_read(sock, buffer(readBuf),
               transfer_at_least(ReceiveBufferSize::MIN_BUFFER_SIZE),
               std::bind(&Connection::handleRead, shared_from_this(),
                         std::placeholders::_1, std::placeholders::_2));
  }

private:
  tcp::socket sock;
  std::array<char, ReceiveBufferSize::MAX_BUFFER_SIZE> readBuf;

  void handleRead(const error_code &ec, size_t bytesTransferred) {
    if (ec == error::eof || ec == error::connection_reset) {
      CSLOG("Client disconnected.");
    } else if (ec) {
      CSLOG("Connection Error: ", ec.message());
    } else {
      const char *header = readBuf.data();
      const char *body = header + ReceiveBufferSize::HEADER_SIZE;
      readBuf.fill(0);
      start();
    }
  }
};

CommandServer::CommandServer(uint port, uint threadPoolSize)
    : ios(), acceptor(ios, tcp::endpoint(tcp::v4(), port)),
      work(make_work_guard(ios)) {
  for (uint i = 0; i < threadPoolSize; ++i) {
    threads.push_back(std::thread([&ios = ios]() { ios.run(); }));
  }
}

CommandServer::~CommandServer() {
  work.reset();
  for (uint i = 0; i < threads.size(); ++i) {
    threads[i].join();
  }
}

void CommandServer::start() {
  auto connection = std::make_shared<CommandServer::Connection>(ios);
  acceptor.async_accept(connection->getSocket(),
                        std::bind(&CommandServer::handleAccept, this,
                                  connection, std::placeholders::_1));
}

void CommandServer::handleAccept(std::shared_ptr<Connection> connection,
                                 const error_code &ec) {
  if (!ec) {
    DEBUG_SLOG("A new client connected.");
    connection->start();
    connections.push_back(connection);
  }
  start();
}
} // namespace app
