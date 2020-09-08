#pragma once

#include <glm/glm.hpp>
#include <map>
#include <string>

class GLFWwindow;
class GLFWcursor;
namespace app {

class Display {
public:
  enum class CursorShape { ARROW, IBEAM, CROSSHAIR, HAND, HRESIZE, VRESIZE };

private:
  static constexpr int MAJOR_VERSION = 4;
  static constexpr int MINOR_VERSION = 6;
  static constexpr int CURSOR_SHAPE_COUNT = 6;

  std::string title;
  glm::ivec2 displaySize;
  glm::ivec2 fboSize;
  GLFWwindow *window;
  std::map<CursorShape, GLFWcursor *> mouseCursors;

  friend class Input;

public:
  Display(std::string_view title, glm::ivec2 displaySize);
  ~Display();

  // swap buffer & poll events
  void update();

  // setters
  void setShouldClose(bool close);
  void setSwapInterval(int value);
  void setCursorShape(CursorShape shape);

  // getters
  [[nodiscard]] float getAspectRatio() const { return (float)displaySize.x / displaySize.y; }
  [[nodiscard]] glm::ivec2 getDisplaySize() const { return displaySize; }
  [[nodiscard]] glm::ivec2 getFboSize() const { return fboSize; }

  // time since the window was initialize usually in nano or micro seconds
  [[nodiscard]] float getTime() const;
  [[nodiscard]] bool shouldClose() const;
  [[nodiscard]] bool isFocused() const;

  void hideWindow();
  void showWindow();
};
} // namespace app
