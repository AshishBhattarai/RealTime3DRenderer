#include "core/buffer.h"
#include "core/serializer.h"
#include "types.h"
#include <third_party/asio_noexcept.h>
#include <third_party/catch.hpp>

using namespace asio;
using namespace asio::ip;

class CommandClient {
private:
  static constexpr u16 PORT = 8003;
  Serializer &serializer;
  io_service ios;
  tcp::endpoint ep;
  tcp::socket sock;

public:
  CommandClient()
      : serializer(Serializer::getInstance()), ios(), ep(address_v4(), PORT),
        sock(ios, ep.protocol()) {}
  void connect() { sock.connect(ep); }

  bool sendRTSPConnectRequest(u32 ip, u16 port) {
    Buffer buf(9);
    error_code ec;
    u8 messageType = 1;
    u8 clientId = 1;
    u8 state = 1;
    serializer.pack(buf, 0, messageType, clientId, state, ip, port);
    return asio::write(sock, asio::buffer(buf.data(), buf.getSize())) == 9;
  }

  void join() { ios.run(); }
};

TEST_CASE("CommandServer RTSP Connect Test", "[RTSP_CONNECT]") {
  CommandClient client;
  client.connect();
  auto success = client.sendRTSPConnectRequest(0, 8554);
  client.join();
  if (!success)
    INFO("Failed to send rtsp connect request.")
  REQUIRE(success);
}
