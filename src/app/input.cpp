#include "input.h"
#include "display.h"
#include <GLFW/glfw3.h>

namespace app {
Input::Input(const Display &display)
    : display(display),
      cursorPos(display.getWidth() / 2.0f, display.getHeight() / 2.0f),
      lastCursorPos(cursorPos.xPos, cursorPos.xPos), cursorOffset(0.0f, 0.0f) {
  GLFWwindow *window = display.window;
  //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetWindowUserPointer(window, this);
  // key press callback
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int code,
                                int action, int mods) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->unhandledKeys.push(KeyEvent(key, code, action, mods, glfwGetTime()));
  });
  // cursor pos callback
  glfwSetCursorPosCallback(
      window, [](GLFWwindow *window, double xPos, double yPos) {
        Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
        self->lastCursorPos = self->cursorPos;
        self->cursorPos = CursorPos((float)xPos, (float)yPos);
      });
}

void Input::update() {
  CursorPos newOffset = cursorPos - lastCursorPos;
  if (newOffset != cursorOffset) {
    cursorCallback(newOffset);
    cursorOffset = newOffset;
  }
  while (!unhandledKeys.empty()) {
    const KeyEvent &event = unhandledKeys.front();
    unhandledKeys.pop();
    auto it = keyCallbacks.find(event.key);
    if (it != keyCallbacks.end())
      it->second(event);
    bool pressed = event.action == GLFW_PRESS || event.action == GLFW_REPEAT;
    keys[event.key] = pressed;
  }
}
} // namespace app
