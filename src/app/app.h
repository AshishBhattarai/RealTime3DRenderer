#pragma once

#include "types.h"

namespace ecs {
class Coordinator;
}
namespace app {

class Display;
class Input;
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

  void processInput();

private:
  Display *display;
  Input *input;
  ecs::Coordinator &coordinator;
};
} // namespace app
