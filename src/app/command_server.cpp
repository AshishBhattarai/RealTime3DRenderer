#include "command_server.h"
#include "core/serializer.h"
#include "utils/slogger.h"

using namespace asio;
using namespace asio::ip;

namespace app {
// connection receiving buffer size
namespace ReceiveBufferSize {
// static constexpr uint MIN_BUFFER_SIZE = 8;
// static constexpr uint MAX_BUFFER_SIZE = 15;
static constexpr uint MAX_BODY_SIZE = 12;
static constexpr uint HEADER_SIZE = 3;
} // namespace ReceiveBufferSize

// connection message header
struct MessageHeader {
  enum class Type : u8 { RTSPCOMMAND = 0x01, INPUTCOMMAND = 0x02 };

  u8 messageType;
  u8 clientId;
  u8 state;
};

// conection message body types (depends on header)
enum class RTSPHeaderState : u8 { CONNECT = 0x01, DISCONNECT = 0x02 };
namespace MessageBody {
struct RTSPConnectBody {
  static constexpr size_t size = 6;
  u32 ip;
  u16 port;
};
}; // namespace MessageBody

class CommandServer::Connection
    : public std::enable_shared_from_this<Connection> {
public:
  Connection(io_service &ios)
      : sock(ios), headerBuf(ReceiveBufferSize::HEADER_SIZE),
        bodyBuf(ReceiveBufferSize::MAX_BODY_SIZE) {
    headerBuf.clear();
    bodyBuf.clear();
  }
  ~Connection() { DEBUG_SLOG("Connection destoryed."); }

  std::shared_ptr<Connection> getPtr() { return shared_from_this(); }

  tcp::socket &getSocket() { return sock; }

  void start() { readHeader(); }

private:
  tcp::socket sock;
  Serializer &serializer = Serializer::getInstance();
  Buffer headerBuf;
  Buffer bodyBuf;

  void readHeader() {
    async_read(sock, buffer(headerBuf.data(), headerBuf.getSize()),
               std::bind(&Connection::handleReadHeader, shared_from_this(),
                         std::placeholders::_1, std::placeholders::_2));
  }

  void readBody(const MessageHeader &messageHeader) {
    // process header
    const auto messageType =
        static_cast<MessageHeader::Type>(messageHeader.messageType);
    size_t bodySize = ReceiveBufferSize::MAX_BODY_SIZE;
    switch (messageType) {
    case MessageHeader::Type::RTSPCOMMAND: {
      const auto commandState =
          static_cast<RTSPHeaderState>(messageHeader.state);
      if (commandState == RTSPHeaderState::CONNECT) {
        bodySize = MessageBody::RTSPConnectBody::size;
      } else {
        bodySize = 0;
        // TODO: server command queue
      }
    } break;

    default:
      // invalid messageType
      CSLOG("Invalid messageType", messageHeader.messageType, "received.");
      bodySize = 0;
    }
    if (bodySize)
      async_read(sock, buffer(bodyBuf.data(), bodySize),
                 std::bind(&Connection::handleReadBody, shared_from_this(),
                           std::placeholders::_1, std::placeholders::_2,
                           bodySize, messageType));
  }

  void handleReadHeader(const error_code &ec, size_t bytesTransferred) {
    if (ec == error::eof || ec == error::connection_reset) {
      CSLOG("Client disconnected.");
    } else if (ec) {
      CSLOG("Connection Error: ", ec.message());
    } else if (bytesTransferred != ReceiveBufferSize::HEADER_SIZE) {
      CSLOG("Insufficient header data received from client.",
            sock.remote_endpoint().address().to_string());
    } else {
      MessageHeader messageHeader;
      serializer.unPack(headerBuf, 0, messageHeader.messageType,
                        messageHeader.clientId, messageHeader.state);
      headerBuf.clear();
      readBody(messageHeader);
    }
  }

  void handleReadBody(const error_code &ec, size_t bytesTransferred,
                      size_t bodySize, MessageHeader::Type messageHeaderType) {
    if (ec == error::eof || ec == error::connection_reset) {
      CSLOG("Client disconnected.");
    } else if (ec) {
      CSLOG("Connection Error: ", ec.message());
    } else if (bytesTransferred != bodySize) {
      CSLOG("Insufficient header data received from client.",
            sock.remote_endpoint().address().to_string());
    } else {
      switch (messageHeaderType) {
      case MessageHeader::Type::RTSPCOMMAND:
        handleRTSPConnectionRequest();
        break;

      case MessageHeader::Type::INPUTCOMMAND:
        // TODO
        break;
      }
      bodyBuf.clear();
      readHeader();
    }
  }

  void handleRTSPConnectionRequest() {
    MessageBody::RTSPConnectBody rtspConnectionBody;
    serializer.unPack(bodyBuf, 0, rtspConnectionBody.ip,
                      rtspConnectionBody.port);
  }
}; // namespace app

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
