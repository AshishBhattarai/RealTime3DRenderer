#include <cstdlib>
#define DISABLE_TEST

#include "app/app.h"
#include "app/loaders.h"
#include "components/light.h"
#include "components/model.h"
#include "components/transform.h"
#include "core/image.h"
#include "ecs/coordinator.h"
#include "systems/render_system/render_defaults.h"
#include "systems/render_system/render_system.h"
#include "utils/slogger.h"

int main(int argc, char **argv) {
  // Register Components & systems
  using namespace ecs;
  Coordinator &coordinator = Coordinator::getInstance();
  ComponentFamily transformFamily = coordinator.registerComponent<component::Model>();
  ComponentFamily meshFamily = coordinator.registerComponent<component::Transform>();
  coordinator.registerComponent<component::Light>();

  ecs::Signature sig;
  sig.set(transformFamily, true);
  sig.set(meshFamily, true);
  coordinator.registerSystem<render_system::RenderSystem>(sig);
  sig.reset();

  /**
   * Note
   * Helper systems or sub-systems are registered from their parent systems.
   */

  app::App app(argc, argv);
  app.run();
}
