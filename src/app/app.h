#pragma once

namespace app {
class App {
public:
  // delete copy constructor & assignment
  App(const App &) = delete;
  App &operator=(const App &) = delete;

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
