#pragma once

#include "components/light.h"
#include "components/model.h"
#include "components/transform.h"
#include "ecs/default_events.h"
#include "ecs/event_manager.h"
#include "types.h"
#include <array>
#include <map>
#include <optional>
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

  struct EditorState {
    struct {
      float scale;
      bool showPlane;
    } gridPlaneState;
  };

  /* Coordinate space state for calculating screenspace coords of an object */
  struct CoordinateSpaceState {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec4 viewPort;
    glm::vec3 camPos;
  };

private:
  static constexpr int HISTORY_SIZE = 50;
  ImGuiIO &io;
  std::array<float, HISTORY_SIZE> dtHistory;
  std::array<float, HISTORY_SIZE> fpsHistory;

  /* Gizmo mode */
  enum class GizmoMode { TRANSLATION, ROTATION, SCALE };

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
  glm::mat4 projectionMat;
  EditorState editorState;
  CoordinateSpaceState coordinateSpaceState;

  void childImageView(const char *lable, Texture &texture, int *currentFace, int *currentLod);
  void childSelectableColumn(std::vector<std::vector<std::string>> columns, int &selected);

  std::optional<glm::vec2> worldToScene(glm::vec3 pos);
  void showGizmo(const component::Transform &transform, const GizmoMode mode);

  /* Component Nodes */
  component::Transform showTransformComponent(const component::Transform &transform);
  component::Light showLightComponent(const component::Light &light);
  component::Model showModelComponent(const component::Model &model);

  /* Editor Settings */
  void showGridPlaneSettings();

  /* dockable Windows */
  void showRenderSystemWindow(bool *pclose);
  void showStatsWindow(bool *pclose);
  void showEntityWinow(bool *pclose);
  void showSettingsWindow(bool *pclose);

  /* Menu bar */
  bool shouldClose;
  void showFileMenu();
  void showMainMenuBar();

  Texture createTexture(uint id, uint target);
  float distanceFromSeg(glm::vec2 a, glm::vec2 b, glm::vec2 p);

public:
  AppUi();

  void addFps(int fps);
  void show();
  void showFrame(uint textureId, int texWidth, int texHeight);

  [[nodiscard]] bool getShouldClose() const;
  [[nodiscard]] EditorState getEditorState() const;

  void setEnvMap(uint id, uint target);
  void setDiffuseConvMap(uint id, uint target);
  void setSpecularConvMap(uint id, uint target);
  void setBrdfLUT(uint id, uint target);
  void setCoordinateSpaceState(const CoordinateSpaceState &state);

  /* Receive Events */
  void receive(const event::EntityChanged &event);
};
} // namespace app
