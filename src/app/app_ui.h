#pragma once

#include "components/light.h"
#include "components/model.h"
#include "components/transform.h"
#include "ecs/default_events.h"
#include "ecs/event_manager.h"
#include "types.h"
#include <array>
#include <map>
#include <string>
#include <vector>

// TODO: Split this into multiple class ??
struct ImGuiIO;
namespace app {
class AppUi : public ecs::Receiver<event::EntityChanged> {
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

  /* ECS data */
  struct Entity {
    std::string id;
    std::string sig;
  };
  std::map<EntityId, Entity> entities;

  void childImageView(const char *lable, Texture &texture, int *currentFace, int *currentLod);
  void childSelectableColumn(std::vector<std::vector<std::string>> columns, int &selected);

  /* Component Nodes */
  component::Transform showTransformComponent(const component::Transform &transform);
  component::Light showLightComponent(const component::Light &light);
  component::Model showModelComponent(const component::Model &model);

  /* dockable Windows */
  void showRenderSystemWindow(bool *pclose);
  void showStatsWindow(bool *pclose);
  void showEntityWinow(bool *pclose);

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

  /* Receive Events */
  void receive(const event::EntityChanged &event);
};
} // namespace app
