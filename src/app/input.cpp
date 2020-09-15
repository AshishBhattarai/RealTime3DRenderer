#include "input.h"
#include "display.h"
#include "types.h"
#include <GLFW/glfw3.h>

namespace app {
Input::Input(Display &display)
    : display(display),
      cursorMode(CursorMode::NORMAL), lastCursorPos{display.getDisplaySize().x / 2.0f,
                                                    display.getDisplaySize().y / 2.0f} {
  GLFWwindow *window = display.window;
  //  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetWindowUserPointer(window, this);
  // key press callback
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int code, int action, int mods) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    /* static_cast unsafe?
     * If the key(int) doesn't match ony enums(Keys) it'll be a garbage enum
     * (won't equate to any enums).
     */
    self->unhandledKeys.push({mods, static_cast<Key>(key), static_cast<Action>(action)});
  });
  // cursor pos callback
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xPos, double yPos) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->unhandledCursorPos.push({(float)xPos, (float)yPos});
  });
  // scroll offset callback
  glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->unhandledScrollOffset.push({(float)xoffset, (float)yoffset});
  });
  // mouse button callback
  glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->unhandledButtons.push(
        {mods, static_cast<MouseButton>(button), static_cast<Action>(action)});
  });
  // char callback
  glfwSetCharCallback(window, [](GLFWwindow *window, uint c) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->unhandledChars.push({c});
  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->display.fboSize.x = width;
    self->display.fboSize.y = height;
  });

  glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
    Input *self = static_cast<Input *>(glfwGetWindowUserPointer(window));
    self->display.displaySize.x = width;
    self->display.displaySize.y = height;
  });

  setCursorMode(cursorMode);
}

void Input::setCursorMode(CursorMode mode) {
  cursorMode = mode;
  if (cursorModeCallback) cursorModeCallback(mode);
  glfwSetInputMode(display.window, GLFW_CURSOR, toUnderlying<CursorMode>(mode));
}

void Input::toggleCursorMode() {
  if (cursorMode == CursorMode::NORMAL)
    cursorMode = CursorMode::DISABLED;
  else
    cursorMode = CursorMode::NORMAL;
  setCursorMode(cursorMode);
}

void Input::setCursorPos(CursorPos pos) { glfwSetCursorPos(display.window, pos.x, pos.y); }

void Input::update() {
  /* CursorPos Events */
  while (!unhandledCursorPos.empty()) {
    const CursorPos &pos = unhandledCursorPos.front();
    unhandledCursorPos.pop();
    CursorPos dt = pos - lastCursorPos;
    lastCursorPos = pos;
    for (CursorCallback &callback : cursorCallbacks) {
      callback(dt);
    }
  }
  /* ScrollOffset Events */
  while (!unhandledScrollOffset.empty()) {
    const ScrollOffset &offset = unhandledScrollOffset.front();
    unhandledScrollOffset.pop();
    for (ScrollOffsetCallback &callback : scrollCallbacks) {
      callback(offset);
    }
  }
  /* MouseButton Events */
  while (!unhandledButtons.empty()) {
    const MouseButtonEvent &event = unhandledButtons.front();
    unhandledButtons.pop();
    auto it = buttonCallbacks.find(event.button);
    if (it != buttonCallbacks.end()) {
      for (ButtonCallback &callback : it->second) {
        callback(event);
      }
    }
    it = buttonCallbacks.find(MouseButton::ANY);
    if (it != buttonCallbacks.end()) {
      for (ButtonCallback &callback : it->second) {
        callback(event);
      }
    }
    bool pressed = event.action == Action::PRESS || event.action == Action::REPEAT;
    mouseButtons[event.button] = pressed;
  }
  /* Keyboard Keys Events */
  while (!unhandledKeys.empty()) {
    const KeyEvent &event = unhandledKeys.front();
    unhandledKeys.pop();
    auto it = keyCallbacks.find(event.key);
    if (it != keyCallbacks.end()) {
      for (KeyCallback &callback : it->second) {
        callback(event);
      }
    }
    it = keyCallbacks.find(Key::ANY);
    if (it != keyCallbacks.end()) {
      for (KeyCallback &callback : it->second) {
        callback(event);
      }
    }
    bool pressed = event.action == Action::PRESS || event.action == Action::REPEAT;
    keys[event.key] = pressed;
  }
  /* Char Events  */
  while (!unhandledChars.empty()) {
    // TODO: const& if members added to CharEvent
    CharEvent event = unhandledChars.front();
    unhandledChars.pop();
    for (CharCallback callback : charCallbacks) {
      callback(event);
    }
  }
}

/* Check if wrapped enum values match GLFW marcros */
/* Keys */
static_assert(toUnderlying<Input::Key>(Input::Key::UNKNOWN) == GLFW_KEY_UNKNOWN);
static_assert(toUnderlying<Input::Key>(Input::Key::SPACE) == GLFW_KEY_SPACE);
static_assert(toUnderlying<Input::Key>(Input::Key::APOSTROPHE) == GLFW_KEY_APOSTROPHE);
static_assert(toUnderlying<Input::Key>(Input::Key::COMMA) == GLFW_KEY_COMMA);
static_assert(toUnderlying<Input::Key>(Input::Key::MINUS) == GLFW_KEY_MINUS);
static_assert(toUnderlying<Input::Key>(Input::Key::PERIOD) == GLFW_KEY_PERIOD);
static_assert(toUnderlying<Input::Key>(Input::Key::SLASH) == GLFW_KEY_SLASH);
static_assert(toUnderlying<Input::Key>(Input::Key::ZERO) == GLFW_KEY_0);
static_assert(toUnderlying<Input::Key>(Input::Key::ONE) == GLFW_KEY_1);
static_assert(toUnderlying<Input::Key>(Input::Key::TWO) == GLFW_KEY_2);
static_assert(toUnderlying<Input::Key>(Input::Key::THREE) == GLFW_KEY_3);
static_assert(toUnderlying<Input::Key>(Input::Key::FOUR) == GLFW_KEY_4);
static_assert(toUnderlying<Input::Key>(Input::Key::FIVE) == GLFW_KEY_5);
static_assert(toUnderlying<Input::Key>(Input::Key::SIX) == GLFW_KEY_6);
static_assert(toUnderlying<Input::Key>(Input::Key::SEVEN) == GLFW_KEY_7);
static_assert(toUnderlying<Input::Key>(Input::Key::EIGHT) == GLFW_KEY_8);
static_assert(toUnderlying<Input::Key>(Input::Key::NINE) == GLFW_KEY_9);
static_assert(toUnderlying<Input::Key>(Input::Key::SEMICOLON) == GLFW_KEY_SEMICOLON);
static_assert(toUnderlying<Input::Key>(Input::Key::EQUAL) == GLFW_KEY_EQUAL);
static_assert(toUnderlying<Input::Key>(Input::Key::A) == GLFW_KEY_A);
static_assert(toUnderlying<Input::Key>(Input::Key::B) == GLFW_KEY_B);
static_assert(toUnderlying<Input::Key>(Input::Key::C) == GLFW_KEY_C);
static_assert(toUnderlying<Input::Key>(Input::Key::D) == GLFW_KEY_D);
static_assert(toUnderlying<Input::Key>(Input::Key::E) == GLFW_KEY_E);
static_assert(toUnderlying<Input::Key>(Input::Key::F) == GLFW_KEY_F);
static_assert(toUnderlying<Input::Key>(Input::Key::G) == GLFW_KEY_G);
static_assert(toUnderlying<Input::Key>(Input::Key::H) == GLFW_KEY_H);
static_assert(toUnderlying<Input::Key>(Input::Key::I) == GLFW_KEY_I);
static_assert(toUnderlying<Input::Key>(Input::Key::J) == GLFW_KEY_J);
static_assert(toUnderlying<Input::Key>(Input::Key::K) == GLFW_KEY_K);
static_assert(toUnderlying<Input::Key>(Input::Key::L) == GLFW_KEY_L);
static_assert(toUnderlying<Input::Key>(Input::Key::M) == GLFW_KEY_M);
static_assert(toUnderlying<Input::Key>(Input::Key::N) == GLFW_KEY_N);
static_assert(toUnderlying<Input::Key>(Input::Key::O) == GLFW_KEY_O);
static_assert(toUnderlying<Input::Key>(Input::Key::P) == GLFW_KEY_P);
static_assert(toUnderlying<Input::Key>(Input::Key::Q) == GLFW_KEY_Q);
static_assert(toUnderlying<Input::Key>(Input::Key::R) == GLFW_KEY_R);
static_assert(toUnderlying<Input::Key>(Input::Key::S) == GLFW_KEY_S);
static_assert(toUnderlying<Input::Key>(Input::Key::T) == GLFW_KEY_T);
static_assert(toUnderlying<Input::Key>(Input::Key::U) == GLFW_KEY_U);
static_assert(toUnderlying<Input::Key>(Input::Key::V) == GLFW_KEY_V);
static_assert(toUnderlying<Input::Key>(Input::Key::W) == GLFW_KEY_W);
static_assert(toUnderlying<Input::Key>(Input::Key::X) == GLFW_KEY_X);
static_assert(toUnderlying<Input::Key>(Input::Key::Y) == GLFW_KEY_Y);
static_assert(toUnderlying<Input::Key>(Input::Key::Z) == GLFW_KEY_Z);
static_assert(toUnderlying<Input::Key>(Input::Key::LEFT_BRACKET) == GLFW_KEY_LEFT_BRACKET);
static_assert(toUnderlying<Input::Key>(Input::Key::BACKSLASH) == GLFW_KEY_BACKSLASH);
static_assert(toUnderlying<Input::Key>(Input::Key::RIGHT_BRACKET) == GLFW_KEY_RIGHT_BRACKET);
static_assert(toUnderlying<Input::Key>(Input::Key::GRAVE_ACCENT) == GLFW_KEY_GRAVE_ACCENT);
static_assert(toUnderlying<Input::Key>(Input::Key::WORLD_1) == GLFW_KEY_WORLD_1);
static_assert(toUnderlying<Input::Key>(Input::Key::WORLD_2) == GLFW_KEY_WORLD_2);

/* Function keys */
static_assert(toUnderlying<Input::Key>(Input::Key::ESCAPE) == GLFW_KEY_ESCAPE);
static_assert(toUnderlying<Input::Key>(Input::Key::ENTER) == GLFW_KEY_ENTER);
static_assert(toUnderlying<Input::Key>(Input::Key::TAB) == GLFW_KEY_TAB);
static_assert(toUnderlying<Input::Key>(Input::Key::BACKSPACE) == GLFW_KEY_BACKSPACE);
static_assert(toUnderlying<Input::Key>(Input::Key::INSERT) == GLFW_KEY_INSERT);
static_assert(toUnderlying<Input::Key>(Input::Key::DELETE) == GLFW_KEY_DELETE);
static_assert(toUnderlying<Input::Key>(Input::Key::RIGHT) == GLFW_KEY_RIGHT);
static_assert(toUnderlying<Input::Key>(Input::Key::LEFT) == GLFW_KEY_LEFT);
static_assert(toUnderlying<Input::Key>(Input::Key::DOWN) == GLFW_KEY_DOWN);
static_assert(toUnderlying<Input::Key>(Input::Key::UP) == GLFW_KEY_UP);
static_assert(toUnderlying<Input::Key>(Input::Key::PAGE_UP) == GLFW_KEY_PAGE_UP);
static_assert(toUnderlying<Input::Key>(Input::Key::PAGE_DOWN) == GLFW_KEY_PAGE_DOWN);
static_assert(toUnderlying<Input::Key>(Input::Key::HOME) == GLFW_KEY_HOME);
static_assert(toUnderlying<Input::Key>(Input::Key::END) == GLFW_KEY_END);
static_assert(toUnderlying<Input::Key>(Input::Key::CAPS_LOCK) == GLFW_KEY_CAPS_LOCK);
static_assert(toUnderlying<Input::Key>(Input::Key::SCROLL_LOCK) == GLFW_KEY_SCROLL_LOCK);
static_assert(toUnderlying<Input::Key>(Input::Key::NUM_LOCK) == GLFW_KEY_NUM_LOCK);
static_assert(toUnderlying<Input::Key>(Input::Key::PRINT_SCREEN) == GLFW_KEY_PRINT_SCREEN);
static_assert(toUnderlying<Input::Key>(Input::Key::PAUSE) == GLFW_KEY_PAUSE);
static_assert(toUnderlying<Input::Key>(Input::Key::F1) == GLFW_KEY_F1);
static_assert(toUnderlying<Input::Key>(Input::Key::F2) == GLFW_KEY_F2);
static_assert(toUnderlying<Input::Key>(Input::Key::F3) == GLFW_KEY_F3);
static_assert(toUnderlying<Input::Key>(Input::Key::F4) == GLFW_KEY_F4);
static_assert(toUnderlying<Input::Key>(Input::Key::F5) == GLFW_KEY_F5);
static_assert(toUnderlying<Input::Key>(Input::Key::F6) == GLFW_KEY_F6);
static_assert(toUnderlying<Input::Key>(Input::Key::F7) == GLFW_KEY_F7);
static_assert(toUnderlying<Input::Key>(Input::Key::F8) == GLFW_KEY_F8);
static_assert(toUnderlying<Input::Key>(Input::Key::F9) == GLFW_KEY_F9);
static_assert(toUnderlying<Input::Key>(Input::Key::F10) == GLFW_KEY_F10);
static_assert(toUnderlying<Input::Key>(Input::Key::F11) == GLFW_KEY_F11);
static_assert(toUnderlying<Input::Key>(Input::Key::F12) == GLFW_KEY_F12);
static_assert(toUnderlying<Input::Key>(Input::Key::F13) == GLFW_KEY_F13);
static_assert(toUnderlying<Input::Key>(Input::Key::F14) == GLFW_KEY_F14);
static_assert(toUnderlying<Input::Key>(Input::Key::F15) == GLFW_KEY_F15);
static_assert(toUnderlying<Input::Key>(Input::Key::F16) == GLFW_KEY_F16);
static_assert(toUnderlying<Input::Key>(Input::Key::F17) == GLFW_KEY_F17);
static_assert(toUnderlying<Input::Key>(Input::Key::F18) == GLFW_KEY_F18);
static_assert(toUnderlying<Input::Key>(Input::Key::F19) == GLFW_KEY_F19);
static_assert(toUnderlying<Input::Key>(Input::Key::F20) == GLFW_KEY_F20);
static_assert(toUnderlying<Input::Key>(Input::Key::F21) == GLFW_KEY_F21);
static_assert(toUnderlying<Input::Key>(Input::Key::F22) == GLFW_KEY_F22);
static_assert(toUnderlying<Input::Key>(Input::Key::F23) == GLFW_KEY_F23);
static_assert(toUnderlying<Input::Key>(Input::Key::F24) == GLFW_KEY_F24);
static_assert(toUnderlying<Input::Key>(Input::Key::F25) == GLFW_KEY_F25);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_0) == GLFW_KEY_KP_0);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_1) == GLFW_KEY_KP_1);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_2) == GLFW_KEY_KP_2);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_3) == GLFW_KEY_KP_3);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_4) == GLFW_KEY_KP_4);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_5) == GLFW_KEY_KP_5);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_6) == GLFW_KEY_KP_6);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_7) == GLFW_KEY_KP_7);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_8) == GLFW_KEY_KP_8);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_9) == GLFW_KEY_KP_9);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_DECIMAL) == GLFW_KEY_KP_DECIMAL);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_DIVIDE) == GLFW_KEY_KP_DIVIDE);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_MULTIPLY) == GLFW_KEY_KP_MULTIPLY);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_SUBTRACT) == GLFW_KEY_KP_SUBTRACT);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_ADD) == GLFW_KEY_KP_ADD);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_ENTER) == GLFW_KEY_KP_ENTER);
static_assert(toUnderlying<Input::Key>(Input::Key::KP_EQUAL) == GLFW_KEY_KP_EQUAL);
static_assert(toUnderlying<Input::Key>(Input::Key::LEFT_SHIFT) == GLFW_KEY_LEFT_SHIFT);
static_assert(toUnderlying<Input::Key>(Input::Key::LEFT_CONTROL) == GLFW_KEY_LEFT_CONTROL);
static_assert(toUnderlying<Input::Key>(Input::Key::LEFT_ALT) == GLFW_KEY_LEFT_ALT);
static_assert(toUnderlying<Input::Key>(Input::Key::LEFT_SUPER) == GLFW_KEY_LEFT_SUPER);
static_assert(toUnderlying<Input::Key>(Input::Key::RIGHT_SHIFT) == GLFW_KEY_RIGHT_SHIFT);
static_assert(toUnderlying<Input::Key>(Input::Key::RIGHT_CONTROL) == GLFW_KEY_RIGHT_CONTROL);
static_assert(toUnderlying<Input::Key>(Input::Key::RIGHT_ALT) == GLFW_KEY_RIGHT_ALT);
static_assert(toUnderlying<Input::Key>(Input::Key::RIGHT_SUPER) == GLFW_KEY_RIGHT_SUPER);
static_assert(toUnderlying<Input::Key>(Input::Key::MENU) == GLFW_KEY_MENU);

/* check if menu is last key on glfw */
static_assert(toUnderlying<Input::Key>(Input::Key::MENU) == GLFW_KEY_LAST);

/* mod bits */
static_assert(toUnderlying<Input::Mod>(Input::Mod::SHIFT) == GLFW_MOD_SHIFT);
static_assert(toUnderlying<Input::Mod>(Input::Mod::CONTROL) == GLFW_MOD_CONTROL);
static_assert(toUnderlying<Input::Mod>(Input::Mod::ALT) == GLFW_MOD_ALT);
static_assert(toUnderlying<Input::Mod>(Input::Mod::SUPER) == GLFW_MOD_SUPER);
static_assert(toUnderlying<Input::Mod>(Input::Mod::CAPS_LOCK) == GLFW_MOD_CAPS_LOCK);
static_assert(toUnderlying<Input::Mod>(Input::Mod::NUM_LOCK) == GLFW_MOD_NUM_LOCK);

/* mouse button */
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::ONE) == GLFW_MOUSE_BUTTON_1);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::TWO) == GLFW_MOUSE_BUTTON_2);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::THREE) == GLFW_MOUSE_BUTTON_3);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::FOUR) == GLFW_MOUSE_BUTTON_4);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::FIVE) == GLFW_MOUSE_BUTTON_5);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::SIX) == GLFW_MOUSE_BUTTON_6);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::SEVEN) == GLFW_MOUSE_BUTTON_7);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::EIGHT) == GLFW_MOUSE_BUTTON_8);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::LEFT) == GLFW_MOUSE_BUTTON_LEFT);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::RIGHT) ==
              GLFW_MOUSE_BUTTON_RIGHT);
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::MIDDLE) ==
              GLFW_MOUSE_BUTTON_MIDDLE);

/**/
static_assert(toUnderlying<Input::MouseButton>(Input::MouseButton::EIGHT) ==
              GLFW_MOUSE_BUTTON_LAST);

/* cursor status */
static_assert(toUnderlying<Input::CursorMode>(Input::CursorMode::NORMAL) == GLFW_CURSOR_NORMAL);
static_assert(toUnderlying<Input::CursorMode>(Input::CursorMode::HIDDEN) == GLFW_CURSOR_HIDDEN);
static_assert(toUnderlying<Input::CursorMode>(Input::CursorMode::DISABLED) == GLFW_CURSOR_DISABLED);
} // namespace app
