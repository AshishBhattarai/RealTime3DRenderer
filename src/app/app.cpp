#include "app.h"

#include "ecs/coordinator.h"
#include <iostream>

namespace app {
App::App(int, char **) { std::cout << "App constructed." << std::endl; }

void App::run() {
  std::cout << "App running." << std::endl;
  std::cout << "Basic test works." << std::endl;
}

App::~App() { std::cout << "App destroyed." << std::endl; }
} // namespace app
