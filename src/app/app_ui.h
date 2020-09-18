#pragma once

#include "types.h"
#include <array>

struct ImGuiIO;
namespace app {
class AppUi {
private:
  static constexpr int HISTORY_SIZE = 50;
  ImGuiIO &io;
  std::array<float, HISTORY_SIZE> dtHistory;
  std::array<float, HISTORY_SIZE> fpsHistory;

  /* RenderSystem data */
  /* PBR */
  struct PBRTextures {
    uint envMap;
    uint diffuseConvMap;
    uint specularConvMap;
    uint brdfLUT;
  } pbrTextures;
  /* PostProcess */
  struct PostProcessValues {
    float gamma;
    /*Mode Enum*/
    float exporuse;
    float splitViewProgress;
  } postProcessValuesA;
  PostProcessValues postProcessValuesB;

  void childImageView(const char *lable, uint texture);
  void showRenderSystemWindow(bool *pclose);
  void showStatsWindow(bool *pclose);

  /* Menu bar */
  bool shouldClose;
  void showFileMenu();
  void showMainMenuBar();

public:
  AppUi();

  void addFps(int fps);
  void show();
  void showFrame(uint textureId, int texWidth, int texHeight);

  [[nodiscard]] bool getShouldClose() const;
};
} // namespace app
