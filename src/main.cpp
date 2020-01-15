#define DISABLE_TEST

#include "app/app.h"

int main(int argc, char **argv) {
  app::App &app = app::App::getInstance(argc, argv);
  app.run();
}
