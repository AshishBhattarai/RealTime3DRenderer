#pragma once

#include <string>

class GLFWwindow;
namespace app {

class Display {
private:
  static constexpr int MAJOR_VERSION = 4;
  static constexpr int MINOR_VERSION = 6;

  std::string title;
  int width;
  int height;
  GLFWwindow *window;

  friend class Input;

public:
  Display(std::string_view title, int width, int height);
  ~Display();

  // swap buffer & poll events
  void update();

  // setters
  void setShouldClose(bool close);
  void setSwapInterval(int value);

  // getters
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  float getAspectRatio() const { return (float)width / height; }

  // time since the window was initialize usually in nano or micro seconds
  float getTime() const;
  bool shouldClose() const;
  bool isFocused() const;
};
} // namespace app
