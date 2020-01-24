#pragma once

#include "types.h"

namespace ecs {
class Coordinator;
}
namespace app {

class Display;
/**
 * Represents the renderer application, handels and manages WINDOW(Context), ECS
 * & UI
 */
class App : NonCopyable {
public:
  // lazy init instance
  static App &getInstance(int argc, char **argv) {
    static App instance(argc, argv);
    return instance;
  }
  void run();

private:
  App(int argc, char **argv);
  ~App();

private:
  Display *display;
  ecs::Coordinator &coordinator;
};
} // namespace app
