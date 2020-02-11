#pragma once

#include "types.h"

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
