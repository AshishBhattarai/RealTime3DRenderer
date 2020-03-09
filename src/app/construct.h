#pragma once

namespace render_system {
class RenderSystem;
}
namespace app {
/**
 * @brief The Construct class
 *
 * Is responsible for creating systems that require configuration(like render
 * system) for the app
 */
class Construct {
private:
  friend class App;
  Construct();

  render_system::RenderSystem *newRenderSystem(int width, int height);
};

}; // namespace app
