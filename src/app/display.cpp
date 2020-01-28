#include "display.h"
#define GLFW_INCLUDE_NONE
#include "common.h"
#include "failure_code.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace app {

Display::Display(std::string_view title, int width, int height)
    : title(title), width(width), height(height) {

  glfwSetErrorCallback([](int error, const char *description) {
    SLOG("[GLFW_CALLBACK]", error, description);
  });
  // init glfw
  if (!glfwInit()) {
    assert(false && "Failed to init GLFW.");
    exit(FailureCode::GLFW_INIT_FAILURE);
  }

  // window hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  //  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  // create window
  window = glfwCreateWindow(
      width, height,
      (this->title + " " + std::to_string(width) + "x" + std::to_string(height))
          .c_str(),
      NULL, NULL);
  if (!window) {
    glfwTerminate();
    assert(false && "Failed to create glfw window.");
    exit(FailureCode::GLFW_CREATE_WINDOW_FAILURE);
  }
  glfwMakeContextCurrent(window);

  // init glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwDestroyWindow(window);
    glfwTerminate();
    assert(false && "Failed to init glad.");
    exit(FailureCode::GLAD_INIT_FALUIRE);
  }
  glfwSwapInterval(1);
  glViewport(0, 0, width, height);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Display::~Display() {
  glfwDestroyWindow(window);
  glfwTerminate();
  DEBUG_SLOG("Display destoryed.");
}

void Display::update() {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void Display::setShouldClose(bool close) {
  glfwSetWindowShouldClose(window, close);
}

void Display::setSwapInterval(int value) { glfwSwapInterval(value); }

double Display::getTime() const { return glfwGetTime(); }
bool Display::shouldClose() const { return glfwWindowShouldClose(window); }
bool Display::isFocused() const {
  return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

} // namespace app
