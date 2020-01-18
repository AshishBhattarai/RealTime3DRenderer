#define DISABLE_TEST

#include "app/app.h"
#include "ecs/system_manager.h"

int main(int argc, char **argv) {
  app::App &app = app::App::getInstance(argc, argv);
  app.run();
}
