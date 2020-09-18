#pragma once

struct ImGuiIO;
namespace app {
class Input;
class Display;
class GuiManager {
private:
  ImGuiIO &io;
  void mapInput(Input &input);
  void showDockSpace();

public:
  GuiManager(Input &input);
  void newFrame(float dt, Input &input, Display &display);
};
} // namespace app
