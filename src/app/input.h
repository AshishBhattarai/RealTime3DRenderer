#pragma once

#include "types.h"
#include <functional>
#include <map>
#include <queue>

namespace app {
class Display;
class Input {
public:
  /* Key Codes, integer values are from GLFW */
  enum class Key : s16 {
    /* Unknown key */
    UNKNOWN = -1,
    /* Printable keys */
    SPACE = 32,
    APOSTROPHE = 39, /* ' */
    COMMA = 44,      /* , */
    MINUS = 45,      /* - */
    PERIOD = 46,     /* . */
    SLASH = 47,      /* / */
    ZERO = 48,
    ONE = 49,
    TWO = 50,
    THREE = 51,
    FOUR = 52,
    FIVE = 53,
    SIX = 54,
    SEVEN = 55,
    EIGHT = 56,
    NINE = 57,
    SEMICOLON = 59, /* ; */
    EQUAL = 61,     /* = */
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LEFT_BRACKET = 91,  /* [ */
    BACKSLASH = 92,     /* \ */
    RIGHT_BRACKET = 93, /* ] */
    GRAVE_ACCENT = 96,  /* ` */
    WORLD_1 = 161,      /* non-US #1 */
    WORLD_2 = 162,      /* non-US #2 */
    /* Function keys */
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE = 261,
    RIGHT = 262,
    LEFT = 263,
    DOWN = 264,
    UP = 265,
    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,
    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    KP_0 = 320,
    KP_1 = 321,
    KP_2 = 322,
    KP_3 = 323,
    KP_4 = 324,
    KP_5 = 325,
    KP_6 = 326,
    KP_7 = 327,
    KP_8 = 328,
    KP_9 = 329,
    KP_DECIMAL = 330,
    KP_DIVIDE = 331,
    KP_MULTIPLY = 332,
    KP_SUBTRACT = 333,
    KP_ADD = 334,
    KP_ENTER = 335,
    KP_EQUAL = 336,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348
  };

  /* modifier bits */
  enum class Mod : u16 {
    SHIFT = 0x0001,
    /*! @brief If this bit is set one or more Control keys were held down.
     *
     *  If this bit is set one or more Control keys were held down.
     */
    CONTROL = 0x0002,
    /*! @brief If this bit is set one or more Alt keys were held down.
     *
     *  If this bit is set one or more Alt keys were held down.
     */
    ALT = 0x0004,
    /*! @brief If this bit is set one or more Super keys were held down.
     *
     *  If this bit is set one or more Super keys were held down.
     */
    SUPER = 0x0008,
    /*! @brief If this bit is set the Caps Lock key is enabled.
     *
     *  If this bit is set the Caps Lock key is enabled and the @ref
     *  GLFW_LOCK_KEY_MODS input mode is set.
     */
    CAPS_LOCK = 0x0010,
    /*! @brief If this bit is set the Num Lock key is enabled.
     *
     *  If this bit is set the Num Lock key is enabled and the @ref
     *  GLFW_LOCK_KEY_MODS input mode is set.
     */
    NUM_LOCK = 0x0020
  };

  /* mouse buttons */
  enum class Mouse : u8 {
    BUTTON_1 = 0,
    BUTTON_2 = 1,
    BUTTON_3 = 2,
    BUTTON_4 = 3,
    BUTTON_5 = 4,
    BUTTON_6 = 5,
    BUTTON_7 = 6,
    BUTTON_8 = 7,
    BUTTON_LAST = BUTTON_8,
    BUTTON_LEFT = BUTTON_1,
    BUTTON_RIGHT = BUTTON_2,
    BUTTON_MIDDLE = BUTTON_3
  };

  /* cursor Mode*/
  enum class CursorMode : u32 { NORMAL = 0x00034001, HIDDEN = 0x00034002, DISABLED = 0x00034003 };

  /* keyboard keys + mouse button action */
  enum class Action { RELEASE = 0, PRESS = 1, REPEAT = 2 };

  struct KeyEvent {
    Key key;
    Action action;
    Mod modifiers;
    double time; // keyevent time TODO: Use standard c++ timestamp?
  };

  struct CursorPos {
    float xPos;
    float yPos;
    CursorPos(float xPos, float yPos) : xPos(xPos), yPos(yPos) {}

    friend CursorPos operator-(const CursorPos &lhs, const CursorPos &rhs) {
      return CursorPos(lhs.xPos - rhs.xPos, lhs.yPos - rhs.yPos);
    }

    bool operator!=(const CursorPos &rhs) { return (xPos != rhs.xPos || yPos != rhs.yPos); }
  };

  using KeyCallback = std::function<void(const KeyEvent &keyEvent)>;
  using CursorCallback = std::function<void(const CursorPos &dt)>; // dt - cursor pos delta

private:
  const Display &display;
  CursorMode cursorMode;
  CursorPos lastCursorPos;
  std::map<Key, bool> keys;

  /* Event callbacks */
  std::map<Key, KeyCallback> keyCallbacks;
  CursorCallback cursorCallback;

  /* Event queues */
  std::queue<KeyEvent> unhandledKeys;
  std::queue<CursorPos> unhandledCursorPos;

public:
  Input(const Display &display);
  void addKeyCallback(Key key, const KeyCallback &callback) { keyCallbacks[key] = callback; }
  void addCursorCallback(const CursorCallback &callback) { cursorCallback = callback; }
  [[nodiscard]] const CursorPos &getLastCursorPos() const { return lastCursorPos; }
  [[nodiscard]] bool getKey(Key key) { return keys[key]; }
  void setCursorMode(CursorMode mode);
  void toggleCursorMode();
  void update();
};
} // namespace app
