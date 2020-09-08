#pragma once

namespace app {
class Input;
class Display;
class Gui {
private:
  void mapInput(Input &input);

public:
  Gui(Input &input);
  void newFrame(float dt, Input &input, Display &display);
};
} // namespace app
