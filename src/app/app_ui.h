#pragma once

#include "types.h"
#include <array>

struct ImGuiIO;
namespace app {
class AppUi {
public:
  struct Texture {
    uint id = 0;
    int width = 0;
    int height = 0;
    uint target = 0;
  };

private:
  static constexpr int HISTORY_SIZE = 50;
  ImGuiIO &io;
  std::array<float, HISTORY_SIZE> dtHistory;
  std::array<float, HISTORY_SIZE> fpsHistory;

  /* RenderSystem data */
  /* PBR */
  struct PBRTextures {
    Texture envMap;
    Texture diffuseConvMap;
    Texture specularConvMap;
    Texture brdfLUT;
  } pbrTextures;
  /* PostProcess */
  struct PostProcessValues {
    float gamma;
    /*Mode Enum*/
    float exporuse;
    float splitViewProgress;
  } postProcessValuesA;
  PostProcessValues postProcessValuesB;

  void childImageView(const char *lable, Texture &texture, int *currentFace);
  void showRenderSystemWindow(bool *pclose);
  void showStatsWindow(bool *pclose);

  /* Menu bar */
  bool shouldClose;
  void showFileMenu();
  void showMainMenuBar();

  Texture createTexture(uint id, uint target);

public:
  AppUi();

  void addFps(int fps);
  void show();
  void showFrame(uint textureId, int texWidth, int texHeight);

  [[nodiscard]] bool getShouldClose() const;

  void setEnvMap(uint id, uint target);
  void setDiffuseConvMap(uint id, uint target);
  void setSpecularConvMap(uint id, uint target);
  void setBrdfLUT(uint id, uint target);
};
} // namespace app
