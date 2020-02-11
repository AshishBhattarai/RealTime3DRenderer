#pragma once

#include "types.h"

namespace render_system {
class RenderSystem;
}

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

  render_system::RenderSystem *newRenderSystem(float ar);

private:
  AppConfig();
  ~AppConfig();
};

} // namespace app
