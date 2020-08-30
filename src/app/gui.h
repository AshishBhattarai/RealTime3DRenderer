#pragma once

namespace app {
class Input;
class Gui {
private:
  void mapInput(Input &input);

public:
  Gui(Input &input);
};
} // namespace app
