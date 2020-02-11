#define DISABLE_TEST

#include "app/app.h"
#include "app/loaders.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "ecs/coordinator.h"
#include "systems/render_system/render_defaults.h"
#include "systems/render_system/render_system.h"
#include "utils/image.h"
#include "utils/slogger.h"

int main(int argc, char **argv) {
  // Register Components & systems
  using namespace ecs;
  Coordinator &coordinator = Coordinator::getInstance();
  ComponentFamily transformFamily =
      coordinator.registerComponent<component::Mesh>();
  ComponentFamily meshFamily =
      coordinator.registerComponent<component::Transform>();

  ecs::Signature sig;
  sig.set(transformFamily, true);
  sig.set(meshFamily, true);
  coordinator.registerSystem<render_system::RenderSystem>(sig);
  sig.reset();

  app::App app(argc, argv);
  app.run();
}
