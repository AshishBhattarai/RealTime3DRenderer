#pragma once

#include "types.h"

namespace app {
class AppUi {
private:
  bool fullScreenFrame;
  void showFrameWindow();

public:
  AppUi();
  void show();
  void showFrame(uint textureId, int texWidth, int texHeight);
  void toggleFullScreenFrame() { fullScreenFrame = !fullScreenFrame; }
};
} // namespace app
