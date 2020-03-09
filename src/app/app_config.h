#pragma once

#include "types.h"
#include "utils/slogger.h"
#include <cstdlib>

/**
 * Using ASIO with ASIO_NO_EXCEPTIONS=1,
 * Need to override asio::detail::throw_exception
 */
namespace asio::detail {
template <typename Exception> void throw_exception(const Exception &e) {
  SLOG("[ASIO_EXCEPTION] Fatal error:", e.what());
  exit(-1);
}
} // namespace asio::detail

namespace app {

/**
 * @brief The AppConfig class
 *
 * Configuring app
 */
class AppConfig : NonCopyable {
public:
  static AppConfig &getInstance() {
    static AppConfig instance;
    return instance;
  }

private:
  AppConfig();
  ~AppConfig();
};

} // namespace app
