#pragma once

#include "types.h"

namespace app {
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
};
} // namespace app
