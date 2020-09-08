#include "display.h"
#define GLFW_INCLUDE_NONE
#include "common.h"
#include "failure_code.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace app {

Display::Display(std::string_view title, glm::ivec2 displaySize)
    : title(title), displaySize(displaySize), fboSize(0, 0) {

  glfwSetErrorCallback(
      [](int error, const char *description) { SLOG("[GLFW_CALLBACK]", error, description); });
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
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  // create window
  window = glfwCreateWindow(
      displaySize.x, displaySize.y,
      (this->title + " " + std::to_string(displaySize.x) + "x" + std::to_string(displaySize.y))
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
  glViewport(0, 0, displaySize.x, displaySize.y);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  hideWindow();

  // init cursor shapes, can add new shape with glfwCreateCursor()
  mouseCursors[CursorShape::ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  mouseCursors[CursorShape::IBEAM] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  mouseCursors[CursorShape::VRESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  mouseCursors[CursorShape::HRESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  mouseCursors[CursorShape::HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

  glfwGetFramebufferSize(window, &fboSize.x, &fboSize.y);
}

Display::~Display() {
  // destory crusors
  for (auto &cursor : mouseCursors) {
    glfwDestroyCursor(cursor.second);
    cursor.second = NULL;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  DEBUG_SLOG("Display destoryed.");
}

void Display::update() {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void Display::setShouldClose(bool close) { glfwSetWindowShouldClose(window, close); }
void Display::setSwapInterval(int value) { glfwSwapInterval(value); }
void Display::setCursorShape(CursorShape shape) { glfwSetCursor(window, mouseCursors[shape]); }

float Display::getTime() const { return (float)glfwGetTime(); }
bool Display::shouldClose() const { return glfwWindowShouldClose(window); }
bool Display::isFocused() const { return glfwGetWindowAttrib(window, GLFW_FOCUSED); }

void Display::hideWindow() { glfwHideWindow(window); }
void Display::showWindow() { glfwShowWindow(window); }
} // namespace app
