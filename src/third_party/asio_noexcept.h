#pragma once
#include "utils/slogger.h"
#include <asio.hpp>
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
