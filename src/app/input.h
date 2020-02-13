#pragma once

#include <functional>
#include <map>
#include <queue>

/* Directly copied from glfw.h */

#define INPUT_KEY_UNKNOWN -1

/* Printable keys */
#define INPUT_KEY_SPACE 32
#define INPUT_KEY_APOSTROPHE 39 /* ' */
#define INPUT_KEY_COMMA 44      /* , */
#define INPUT_KEY_MINUS 45      /* - */
#define INPUT_KEY_PERIOD 46     /* . */
#define INPUT_KEY_SLASH 47      /* / */
#define INPUT_KEY_0 48
#define INPUT_KEY_1 49
#define INPUT_KEY_2 50
#define INPUT_KEY_3 51
#define INPUT_KEY_4 52
#define INPUT_KEY_5 53
#define INPUT_KEY_6 54
#define INPUT_KEY_7 55
#define INPUT_KEY_8 56
#define INPUT_KEY_9 57
#define INPUT_KEY_SEMICOLON 59 /* ; */
#define INPUT_KEY_EQUAL 61     /* = */
#define INPUT_KEY_A 65
#define INPUT_KEY_B 66
#define INPUT_KEY_C 67
#define INPUT_KEY_D 68
#define INPUT_KEY_E 69
#define INPUT_KEY_F 70
#define INPUT_KEY_G 71
#define INPUT_KEY_H 72
#define INPUT_KEY_I 73
#define INPUT_KEY_J 74
#define INPUT_KEY_K 75
#define INPUT_KEY_L 76
#define INPUT_KEY_M 77
#define INPUT_KEY_N 78
#define INPUT_KEY_O 79
#define INPUT_KEY_P 80
#define INPUT_KEY_Q 81
#define INPUT_KEY_R 82
#define INPUT_KEY_S 83
#define INPUT_KEY_T 84
#define INPUT_KEY_U 85
#define INPUT_KEY_V 86
#define INPUT_KEY_W 87
#define INPUT_KEY_X 88
#define INPUT_KEY_Y 89
#define INPUT_KEY_Z 90
#define INPUT_KEY_LEFT_BRACKET 91  /* [ */
#define INPUT_KEY_BACKSLASH 92     /* \ */
#define INPUT_KEY_RIGHT_BRACKET 93 /* ] */
#define INPUT_KEY_GRAVE_ACCENT 96  /* ` */
#define INPUT_KEY_WORLD_1 161      /* non-US #1 */
#define INPUT_KEY_WORLD_2 162      /* non-US #2 */

/* Function keys */
#define INPUT_KEY_ESCAPE 256
#define INPUT_KEY_ENTER 257
#define INPUT_KEY_TAB 258
#define INPUT_KEY_BACKSPACE 259
#define INPUT_KEY_INSERT 260
#define INPUT_KEY_DELETE 261
#define INPUT_KEY_RIGHT 262
#define INPUT_KEY_LEFT 263
#define INPUT_KEY_DOWN 264
#define INPUT_KEY_UP 265
#define INPUT_KEY_PAGE_UP 266
#define INPUT_KEY_PAGE_DOWN 267
#define INPUT_KEY_HOME 268
#define INPUT_KEY_END 269
#define INPUT_KEY_CAPS_LOCK 280
#define INPUT_KEY_SCROLL_LOCK 281
#define INPUT_KEY_NUM_LOCK 282
#define INPUT_KEY_PRINT_SCREEN 283
#define INPUT_KEY_PAUSE 284
#define INPUT_KEY_F1 290
#define INPUT_KEY_F2 291
#define INPUT_KEY_F3 292
#define INPUT_KEY_F4 293
#define INPUT_KEY_F5 294
#define INPUT_KEY_F6 295
#define INPUT_KEY_F7 296
#define INPUT_KEY_F8 297
#define INPUT_KEY_F9 298
#define INPUT_KEY_F10 299
#define INPUT_KEY_F11 300
#define INPUT_KEY_F12 301
#define INPUT_KEY_F13 302
#define INPUT_KEY_F14 303
#define INPUT_KEY_F15 304
#define INPUT_KEY_F16 305
#define INPUT_KEY_F17 306
#define INPUT_KEY_F18 307
#define INPUT_KEY_F19 308
#define INPUT_KEY_F20 309
#define INPUT_KEY_F21 310
#define INPUT_KEY_F22 311
#define INPUT_KEY_F23 312
#define INPUT_KEY_F24 313
#define INPUT_KEY_F25 314
#define INPUT_KEY_KP_0 320
#define INPUT_KEY_KP_1 321
#define INPUT_KEY_KP_2 322
#define INPUT_KEY_KP_3 323
#define INPUT_KEY_KP_4 324
#define INPUT_KEY_KP_5 325
#define INPUT_KEY_KP_6 326
#define INPUT_KEY_KP_7 327
#define INPUT_KEY_KP_8 328
#define INPUT_KEY_KP_9 329
#define INPUT_KEY_KP_DECIMAL 330
#define INPUT_KEY_KP_DIVIDE 331
#define INPUT_KEY_KP_MULTIPLY 332
#define INPUT_KEY_KP_SUBTRACT 333
#define INPUT_KEY_KP_ADD 334
#define INPUT_KEY_KP_ENTER 335
#define INPUT_KEY_KP_EQUAL 336
#define INPUT_KEY_LEFT_SHIFT 340
#define INPUT_KEY_LEFT_CONTROL 341
#define INPUT_KEY_LEFT_ALT 342
#define INPUT_KEY_LEFT_SUPER 343
#define INPUT_KEY_RIGHT_SHIFT 344
#define INPUT_KEY_RIGHT_CONTROL 345
#define INPUT_KEY_RIGHT_ALT 346
#define INPUT_KEY_RIGHT_SUPER 347
#define INPUT_KEY_MENU 348

#define INPUT_KEY_LAST GLFW_KEY_MENU

/* modifier bits */
#define INPUT_MOD_SHIFT 0x0001
/*! @brief If this bit is set one or more Control keys were held down.
 *
 *  If this bit is set one or more Control keys were held down.
 */
#define INPUT_MOD_CONTROL 0x0002
/*! @brief If this bit is set one or more Alt keys were held down.
 *
 *  If this bit is set one or more Alt keys were held down.
 */
#define INPUT_MOD_ALT 0x0004
/*! @brief If this bit is set one or more Super keys were held down.
 *
 *  If this bit is set one or more Super keys were held down.
 */
#define INPUT_MOD_SUPER 0x0008
/*! @brief If this bit is set the Caps Lock key is enabled.
 *
 *  If this bit is set the Caps Lock key is enabled and the @ref
 *  GLFW_LOCK_KEY_MODS input mode is set.
 */
#define INPUT_MOD_CAPS_LOCK 0x0010
/*! @brief If this bit is set the Num Lock key is enabled.
 *
 *  If this bit is set the Num Lock key is enabled and the @ref
 *  GLFW_LOCK_KEY_MODS input mode is set.
 */
#define GLFW_MOD_NUM_LOCK 0x0020

#define INPUT_MOUSE_BUTTON_1 0
#define INPUT_MOUSE_BUTTON_2 1
#define INPUT_MOUSE_BUTTON_3 2
#define INPUT_MOUSE_BUTTON_4 3
#define INPUT_MOUSE_BUTTON_5 4
#define INPUT_MOUSE_BUTTON_6 5
#define INPUT_MOUSE_BUTTON_7 6
#define INPUT_MOUSE_BUTTON_8 7
#define INPUT_MOUSE_BUTTON_LAST INPUT_MOUSE_BUTTON_8
#define INPUT_MOUSE_BUTTON_LEFT INPUT_MOUSE_BUTTON_1
#define INPUT_MOUSE_BUTTON_RIGHT INPUT_MOUSE_BUTTON_2
#define INPUT_MOUSE_BUTTON_MIDDLE INPUT_MOUSE_BUTTON_3

// cursor status
#define INPUT_CURSOR_NORMAL 0x00034001
#define INPUT_CURSOR_HIDDEN 0x00034002
#define INPUT_CURSOR_DISABLED 0x00034003

// keyboard keys + mouse button
#define INPUT_RELEASE 0
#define INPUT_PRESS 1
#define INPUT_REPEAT 2

namespace app {
class Display;
class Input {
public:
  struct KeyEvent {
    int key, code, action, modifiers;
    double time;

    KeyEvent(int key, int code, int action, int modifiers, double time)
        : key(key), code(code), action(action), modifiers(modifiers),
          time(time) {}
  };
  struct CursorPos {
    float xPos;
    float yPos;
    CursorPos(float xPos, float yPos) : xPos(xPos), yPos(yPos) {}

    friend CursorPos operator-(const CursorPos &lhs, const CursorPos &rhs);
    bool operator!=(const CursorPos &rhs) {
      return (xPos != rhs.xPos || yPos != rhs.yPos);
    }
  };

  friend CursorPos operator-(const CursorPos &lhs, const CursorPos &rhs) {
    return CursorPos(lhs.xPos - rhs.xPos, lhs.yPos - rhs.yPos);
  }

  using KeyCallback = std::function<void(const KeyEvent &keyEvent)>;
  using CursorCallback =
      std::function<void(const CursorPos &dt)>; // dt - cursor pos delta

private:
  const Display &display;
  int cursorMode;

  CursorPos lastCursorPos;
  std::map<int, bool> keys;
  std::map<int, KeyCallback> keyCallbacks;
  CursorCallback cursorCallback;
  std::queue<KeyEvent> unhandledKeys;
  std::queue<CursorPos> unhandledCursorPos;

public:
  Input(const Display &display);
  void addKeyCallback(int key, const KeyCallback &callback) {
    keyCallbacks[key] = callback;
  }
  void addCursorCallback(const CursorCallback &callback) {
    cursorCallback = callback;
  }
  const CursorPos &getLastCursorPos() const { return lastCursorPos; }
  bool getKey(int key) { return keys[key]; }
  void setCursorMode(int mode);
  void toggleCursorMode() {
    if (cursorMode == INPUT_CURSOR_NORMAL)
      cursorMode = INPUT_CURSOR_DISABLED;
    else
      cursorMode = INPUT_CURSOR_NORMAL;
    setCursorMode(cursorMode);
  }
  void update();
};
} // namespace app
