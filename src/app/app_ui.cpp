#include "app_ui.h"
#include "../utils/slogger.h"
#include "common.h"
#include "components/transform.h"
#include "ecs/coordinator.h"
#include "ecs/default_events.h"
#include "ecs/event_manager.h"
#include "systems/render_system/gui_renderer.h"
#include "systems/render_system/texture.h"
#include "types.h"
#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui/imgui.h>
#include <numeric>
#include <string>
#include <utility>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui/imgui_internal.h>

static int fpsHistoryI = 0;
static int dtHistoryI = 0;

/* Window status */
static bool statsOpen = true;
static bool renderSystemOpen = true;
static bool entityWindowOpen = true;
static bool settingWindowOpen = true;
static int sceneScale = 68;
static glm::vec4 sceneViewport;

static uint globalWindowsFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;

namespace app {
AppUi::AppUi()
    : io(ImGui::GetIO()), entities(),
      projectionMat(), editorState{{40.0f, true}}, gizmoState{false, false, GizmoMode::TRANSLATION,
                                                              glm::vec3(0.0f), glm::vec2(0.0f)},
      shouldClose(false) {
  fpsHistory.fill(60);
  ecs::Coordinator::getInstance().eventManager.subscribe<event::EntityChanged>(*this);
}

void AppUi::childImageView(const char *lable, Texture &texture, int *currentFace, int *currentLod) {
  if (ImGui::TreeNode(lable)) {
    ImTextureID texId = (ImTextureID)(uptr)texture.id;
    float texW = (float)256;
    float texH = (float)256;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 uvMin = ImVec2(0.0f, 0.0f);                 // Top-left
    ImVec2 uvMax = ImVec2(1.0f, 1.0f);                 // Lower-right
    ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 borderCol = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
    ImGui::Image(texId, ImVec2(texW, texH), uvMin, uvMax, tintCol, borderCol);
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      float regionSize = 32.0f;
      float regionX = io.MousePos.x - pos.x - regionSize * 0.5f;
      float regionY = io.MousePos.y - pos.y - regionSize * 0.5f;
      float zoom = 4.0f;
      if (regionX < 0.0f) {
        regionX = 0.0f;
      } else if (regionX > texW - regionSize) {
        regionX = texH - regionSize;
      }
      if (regionY < 0.0f) {
        regionY = 0.0f;
      } else if (regionY > texW - regionSize) {
        regionY = texH - regionSize;
      }
      ImGui::Text("Min: (%.2f, %.2f)", regionX, regionY);
      ImGui::Text("Max: (%.2f, %.2f)", regionX + regionSize, regionY + regionSize);
      ImVec2 uv0 = ImVec2((regionX) / texW, (regionY) / texH);
      ImVec2 uv1 = ImVec2((regionX + regionSize) / texW, (regionY + regionSize) / texH);
      ImGui::Image(texId, ImVec2(regionSize * zoom, regionSize * zoom), uv0, uv1, tintCol,
                   borderCol);
      ImGui::EndTooltip();
    }
    ImGui::Text("%.0fx%.0f", texW, texH);
    static const char *items[] = {
        "0", "1", "2", "3", "4", "5",
    };
    if (currentFace) {
      ImGui::PushItemWidth(35);
      ImGui::Combo("Face", currentFace, items, IM_ARRAYSIZE(items));
      texture.id = render_system::GuiRenderer::generateTextureMask(texture.id, texture.target,
                                                                   *currentFace + 1);
      ImGui::PopItemWidth();
    }
    if (currentLod) {
      u8 face = 0;
      if (currentFace) {
        ImGui::SameLine();
        face = *currentFace + 1;
      }
      ImGui::PushItemWidth(35);
      ImGui::Combo("Lod", currentLod, items, IM_ARRAYSIZE(items));
      texture.id = render_system::GuiRenderer::generateTextureMask(texture.id, texture.target, face,
                                                                   *currentLod);
      ImGui::PopItemWidth();
    }
    ImGui::TreePop();
  }
}

void AppUi::childSelectableColumn(std::vector<std::vector<std::string>> columns, int &selected) {
  for (size_t i = 0; i < columns.size(); ++i) {
    char label[32];
    sprintf(label, "##%04d", (int)i);
    if (ImGui::Selectable(label, selected == (int)i, ImGuiSelectableFlags_SpanAllColumns))
      selected = i;
    // bool hovered = ImGui::IsItemHovered();
    ImGui::SameLine();
    for (const auto &column : columns[i]) {
      ImGui::TextUnformatted(column.c_str());
      ImGui::NextColumn();
    }
  }
}

std::optional<glm::vec2> AppUi::worldToScene(glm::vec3 pos) {
  glm::vec4 clip = coordinateSpaceState.projectionMatrix * coordinateSpaceState.viewMatrix *
                   glm::vec4(pos, 1.0f);
  // clip x, y, z must be in [-w, w] and w > 0
  glm::vec3 ndc = glm::vec3(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w);
  if (clip.w > 0 && ndc.x > -1.0f && ndc.x < 1.0f && ndc.y < 1.0f && ndc.y > -1.0f) {
    return std::optional(glm::vec2((ndc.x + 1.0f) / 2.0f, (1.0f - ndc.y) / 2.0f) *
                             glm::vec2(sceneViewport.z, sceneViewport.w) +
                         glm::vec2(sceneViewport.x, sceneViewport.y));
  } else
    return std::nullopt;
}

void AppUi::showGizmo(const component::Transform &transform) {
  // translation gimzo test
  glm::vec3 pos = transform.position();
  auto scene = worldToScene(pos);
  if (scene.has_value()) {
    // draw translation gizmo
    // three axis x, y, z
    auto lines = std::array{glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)};
    auto drawList = ImGui::GetForegroundDrawList();
    glm::vec3 camToObj = glm::normalize(transform.position() - coordinateSpaceState.camPos);
    float distS = (glm::length(coordinateSpaceState.camPos - pos)) * 0.15f;

    std::array<bool, 3> hovered = {false, false, false};
    uint hoveredIndex = 0;

    constexpr float thickness = 4.0f;
    for (uint i = 0; i < 3; ++i) {
      auto slineOpt = worldToScene(pos + lines[i] * distS);
      if (!slineOpt.has_value()) continue;

      auto sline = slineOpt.value();
      // check line collision

      /* Check if the lines are hovered */
      ImVec2 mousePos = ImGui::GetMousePos();
      float dist = distanceFromSeg(scene.value(), sline, glm::vec2(mousePos.x, mousePos.y));
      drawList->AddCircle(mousePos, 2.0f, IM_COL32(255, 0, 0, 255));
      bool hover = !glm::step(2.5f, dist);

      /* Color based on hover and view */
      float factor = 1.0f - glm::smoothstep(0.85f, 0.95f, abs(camToObj[i]));
      if (factor == 0.0f) hover = false; // no hover on hidden axis
      auto color = IM_COL32(255 * lines[i].x, 255 * lines[i].y, 255 * lines[i].z, 255 * factor);
      if (hover && !gizmoState.isActive) {
        float x = glm::max(lines[i].x, 0.8f);
        float y = glm::max(lines[i].y, 0.8f);
        float z = glm::max(lines[i].z, 0.8f);
        color = IM_COL32(255 * x, 255 * y, 255 * z, 255 * factor);
        hovered[i] = true;
        hoveredIndex = i;
      }

      switch (gizmoState.mode) {
      case GizmoMode::TRANSLATION: {
        /**
         * Translation Gizmo
         */
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(scene.value().x, scene.value().y),
                                                ImVec2(sline.x, sline.y), color, thickness);
        glm::vec2 toLine = glm::normalize(sline - scene.value());
        glm::vec2 orthoDir = glm::normalize(glm::vec2(toLine.y, -toLine.x)) * 12.0f;

        ImVec2 p1 = ImVec2(sline.x, sline.y) + ImVec2(toLine.x, toLine.y) * 12.0f;
        ImVec2 p2 = ImVec2(orthoDir.x, orthoDir.y) + ImVec2(sline.x, sline.y);
        ImVec2 p3 = ImVec2(-orthoDir.x, -orthoDir.y) + ImVec2(sline.x, sline.y);

        drawList->AddTriangleFilled(p1, p2, p3, color);
      } break;

      case GizmoMode::ROTATION: {
        /**
         * Rotation Gizmo
         */
        // convert vector on plane (ie: yz for pitch(x-asix)) to angle on the plane.
        float angleStart = atan2f(camToObj[(i + 2) % 3], camToObj[(i + 1) % 3]); // atan(z, y) for x
        angleStart += M_PI * 0.5f; // rotate 90 deg, so that semi-circle faces the camera.
        ImVec2 circlePos[30];      // 30 segments
        int actualSize = 0;
        for (uint j = 0; j < 30; ++j) {
          float ng = angleStart + M_PI * j / 30.0f; // draw semi-circle 180 deg
          // convert angle back to vector in respective palne (ie: zy for pitch(x-axis)
          glm::vec3 vec = glm::vec3(std::cos(ng), std::sin(ng), 0.0f);
          // for yz(pitch) we need vec3(0, cos(ng) , sing(ng))
          glm::vec3 wpos = glm::vec3(vec[2 - i], vec[(3 - i) % 3], vec[(4 - i) % 3]) * distS +
                           transform.position();
          // screen space position
          auto spos = worldToScene(wpos);
          if (spos.has_value()) {
            circlePos[j] = ImVec2(spos.value().x, spos.value().y);
            ++actualSize;
          } else {
            break;
          }
        }
        auto color = IM_COL32(255 * lines[i].x, 255 * lines[i].y, 255 * lines[i].z, 255);
        drawList->AddPolyline(circlePos, actualSize, color, false, thickness);

        /**
         * TODO: cameraRay to circle plane / rotation plane collision.
         **/
      } break;

      case GizmoMode::SCALE: {
        /**
         * Scale Gizmo
         */
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(scene.value().x, scene.value().y),
                                                ImVec2(sline.x, sline.y), color, thickness);
      } break;
      }
    }

    // hover state and axis
    gizmoState.isHovered = hovered[0] || hovered[1] || hovered[2];
    if (gizmoState.isHovered && !gizmoState.isActive) {
      gizmoState.axis = lines[hoveredIndex];
    }
  }
}

component::Transform AppUi::showTransformComponent(const component::Transform &transform) {
  glm::vec3 position = transform.position();
  glm::vec3 rotation = glm::degrees(transform.rotationEuler());
  float scale = transform.scale().x;
  ImGui::Text("Transformation");
  ImGui::DragFloat3("Translate##1T", glm::value_ptr(position), 0.05f, 0.0f, 0.0f, "%.2f");
  ImGui::DragFloat3("Rotate##1T", glm::value_ptr(rotation), 1.0f, 0.0f, 0.0f, "%.2f");
  ImGui::DragFloat("Scale##1T", &scale, 0.05f, 0.1f, 0.0f, "%.2f");
  scale = (scale <= 0.1) ? 0.1 : scale;
  // TODO: Fix gimble-lock, instead of using euler angle to store rotation use quat

  if (ImGui::RadioButton("Translate", gizmoState.mode == GizmoMode::TRANSLATION)) {
    gizmoState.mode = GizmoMode::TRANSLATION;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Rotation", gizmoState.mode == GizmoMode::ROTATION)) {
    gizmoState.mode = GizmoMode::ROTATION;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Scale", gizmoState.mode == GizmoMode::SCALE)) {
    gizmoState.mode = GizmoMode::SCALE;
  }
  showGizmo(transform);
  ImVec2 worldMousePos = ImGui::GetMousePos();
  if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
    if (gizmoState.isHovered) {
      gizmoState.isActive = true;
      auto objScene = worldToScene(position);
      if (objScene.has_value())
        gizmoState.dif = objScene.value() - glm::vec2(worldMousePos.x, worldMousePos.y);
    }
  } else {
    gizmoState.isActive = false;
  }

  if (gizmoState.isActive) {
    float distS = (glm::length(coordinateSpaceState.camPos - position)) * 0.10f;
    auto objScene = worldToScene(position);
    auto dif = objScene.value() - glm::vec2(worldMousePos.x, worldMousePos.y);
    if (objScene.has_value()) {
      switch (gizmoState.mode) {
      case GizmoMode::TRANSLATION: {
        auto dot = glm::dot(dif, 100.0f * gizmoState.dif);
        float neg = (dot >= 0.0f) ? 1.0f : -1.0f;
        auto vec =
            (dot / (glm::length(100.0f * gizmoState.dif) * glm::length(100.0f * gizmoState.dif))) *
            (100.0f * gizmoState.dif);
        position += (glm::length(vec) - glm::length(gizmoState.dif)) * gizmoState.axis *
                    (1 / 30.0f) * neg * distS;
        break;
      }
      case GizmoMode::ROTATION:
        break;
      case GizmoMode::SCALE: {
        auto ss = glm::length(dif) - glm::length(gizmoState.dif);
        scale += ss * (1 / 30.0f);
        gizmoState.dif = dif;
        break;
      }
      };
    }
  }

  auto oldRot = glm::degrees(transform.rotationEuler());
  auto difRot = rotation - oldRot;
  glm::quat quatRot = transform.rotation();
  if (difRot.x != 0.0f) {
    quatRot *= glm::angleAxis(glm::radians(difRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
  }
  if (difRot.y != 0.0f) {
    quatRot *= glm::angleAxis(glm::radians(difRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
  }
  if (difRot.z != 0.0f) {
    quatRot *= glm::angleAxis(glm::radians(difRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
  }

  return component::Transform(position, quatRot, glm::vec3(scale));
}

component::Light AppUi::showLightComponent(const component::Light &light) {
  glm::vec4 color = glm::vec4(light.color, 0.0f);
  float intensity = light.intensity;
  float range = light.range;
  LightType type = light.type;
  static const char *items[] = {"POINT_LIGHT", "DIRECTION_LIGHT", "SPOT_LIGHT", "AREA_LIGHT"};
  int typeSelected = (int)light.type;
  ImGui::Text("Light");
  ImGui::ColorEdit4("Color##1L", glm::value_ptr(color));
  ImGui::DragFloat("Intensity##1T", &intensity, 0.05f, 0.0f, 0.0f, "%.2f");
  ImGui::DragFloat("Range##1T", &range, 0.05f, 0.0f, 0.0f, "%.2f");
  ImGui::Combo("Type##1T", &typeSelected, items, IM_ARRAYSIZE(items));
  return component::Light(glm::vec3(color), intensity, range, type);
}

component::Model AppUi::showModelComponent(const component::Model &model) {
  // TODO: create a resource manager with all mesh and material ids
  ImGui::Text("Model");
  // TODO: mesh & material selection by name
  ImGui::TextUnformatted(std::string("MeshID: " + std::to_string(model.meshId)).c_str());
  return model;
}

void AppUi::showGridPlaneSettings() {
  ImGui::DragFloat("Scale##1GP", &editorState.gridPlaneState.scale, 2.0f, 0.0f, 0.0f, "%.2f");
  ImGui::Checkbox("ShowPlane##1GPC", &editorState.gridPlaneState.showPlane);
}

void AppUi::showLoadedMeshList() {
  // current width
  ImVec2 size = ImGui::GetWindowSize();
  // begin entity list window
  static int selected = 1;
  if (ImGui::BeginChild("Meshes", ImVec2(0, size.y / 2), true,
                        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)) {
    if (ImGui::BeginMenuBar()) {
      ImGui::Text("Meshes");
      ImGui::EndMenuBar();
    }
    ImGui::Columns(2, "Meshes");
    ImGui::Separator();
    ImGui::Text("ID");
    ImGui::NextColumn();
    ImGui::Text("Name");
    ImGui::NextColumn();
    ImGui::Separator();
    std::vector<std::vector<std::string>> data;
    for (const auto &mesh : loadedMeshes) {
      data.emplace_back(std::vector<std::string>{std::to_string(mesh.meshId), mesh.meshName});
    }
    childSelectableColumn(data, selected);
    ImGui::Columns(1);
    ImGui::Separator();
  }
  ImGui::EndChild();
  if (selected > -1) {
    if (ImGui::BeginChild("MeshProperties", ImVec2(0, size.y / 2), true,
                          ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)) {
      if (ImGui::BeginMenuBar()) {
        ImGui::Text("Properties");
        ImGui::EndMenuBar();
      }
      const auto &mesh = loadedMeshes[selected];
      ImGui::Text("Name:       ");
      ImGui::SameLine();
      ImGui::Text(mesh.meshName.c_str());
      ImGui::Text("Textured:   ");
      ImGui::SameLine();
      ImGui::Text(std::to_string(mesh.hasTexCoords).c_str());
      ImGui::Text("Primitives: ");
      ImGui::SameLine();
      ImGui::Text(std::to_string(mesh.numPrimitive).c_str());

      // materialId to name table
      int selectedM = -1;
      ImGui::NewLine();
      ImGui::Text("Materials: ");
      ImGui::Columns(2, "Materials");
      ImGui::Separator();
      ImGui::Text("ID");
      ImGui::NextColumn();
      ImGui::Text("Name");
      ImGui::NextColumn();
      ImGui::Separator();
      {
        std::vector<std::vector<std::string>> data;
        for (const auto &mat : mesh.matIdToNameMap) {
          data.emplace_back(std::vector<std::string>{std::to_string(mat.first), mat.second});
        }
        childSelectableColumn(data, selectedM);
      }
      ImGui::Columns(1);
      ImGui::Separator();

      // primitiveId to matId
      ImGui::NewLine();
      ImGui::Text("Primitives: ");
      ImGui::Columns(2, "Primitives");
      ImGui::Separator();
      ImGui::Text("ID");
      ImGui::NextColumn();
      ImGui::Text("Material");
      ImGui::NextColumn();
      ImGui::Separator();
      {
        std::vector<std::vector<std::string>> data;
        for (const auto &prim : mesh.primIdToMatId) {
          data.emplace_back(
              std::vector<std::string>{std::to_string(prim.first), std::to_string(prim.second)});
        }
        childSelectableColumn(data, selectedM);
      }
      ImGui::Columns(1);
      ImGui::Separator();
    }
    ImGui::EndChild();
  }
}

void AppUi::showRenderSystemWindow(bool *pclose) {
  if (ImGui::Begin("Render System", pclose, globalWindowsFlags)) {
    ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("RenderSystemTabBar", tabBarFlags)) {
      if (ImGui::BeginTabItem("Renderer")) {
        if (ImGui::CollapsingHeader("PBR")) {
          if (ImGui::TreeNode("PBR Indirect Lighting Maps")) {
            static int envMapFace = 0;
            static int envMapLod = 0;
            childImageView("Environment Map", pbrTextures.envMap, &envMapFace, &envMapLod);
            static int diffuseConvFace = 0;
            static int diffuseConvLod = 0;
            childImageView("Diffuse Convolution Map", pbrTextures.diffuseConvMap, &diffuseConvFace,
                           &diffuseConvLod);
            static int specularConvFace = 0;
            static int specularConvLod = 0;
            childImageView("Specular Convolution Map", pbrTextures.specularConvMap,
                           &specularConvFace, &specularConvLod);
            childImageView("BRDF LUT Map", pbrTextures.brdfLUT, nullptr, nullptr);
            ImGui::TreePop();
          }
        }
        if (ImGui::CollapsingHeader("PostProcess")) {
          static bool check = false;
          if (ImGui::TreeNode("Placeholder")) {
            ImGui::Checkbox("Enable", &check);
            ImGui::TreePop();
          }
        }
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("GPU Data")) {
        showLoadedMeshList();
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }
  ImGui::End();
}

void AppUi::showStatsWindow(bool *pclose) {
  dtHistory[++dtHistoryI % HISTORY_SIZE] = ImGui::GetIO().DeltaTime;

  if (ImGui::Begin("Stats", pclose, globalWindowsFlags)) {
    float avg = std::accumulate(dtHistory.begin(), dtHistory.end(), 0.0f) / (float)HISTORY_SIZE;
    ImGui::PlotLines("Frame Times", dtHistory.data(), HISTORY_SIZE, 0,
                     ("avg: " + std::to_string(avg)).c_str());
    avg = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0f) / (float)HISTORY_SIZE;
    ImGui::PlotLines("FPS", fpsHistory.data(), HISTORY_SIZE, 0,
                     ("avg: " + std::to_string(avg)).c_str(), 0.0f, 60.0f, ImVec2(0, 80.0f));
  }
  ImGui::End();
}

void AppUi::showEntityWinow(bool *pclose) {
  ImGui::Begin("Entity Window", pclose, globalWindowsFlags);
  ImGui::AlignTextToFramePadding();
  // current width
  ImVec2 size = ImGui::GetWindowSize();
  // begin entity list window
  static int selected = 1;
  if (ImGui::BeginChild("Entites", ImVec2(0, size.y / 2), true,
                        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)) {

    ImGui::Columns(2, "Entites");
    ImGui::Separator();
    ImGui::Text("ID");
    ImGui::NextColumn();
    ImGui::Text("ComplentFlag");
    ImGui::NextColumn();
    ImGui::Separator();
    std::vector<std::vector<std::string>> data;
    for (const auto &e : entities) {
      data.emplace_back(std::vector<std::string>{e.second.id, e.second.sig});
    }
    if (ImGui::BeginMenuBar()) {
      ImGui::Text("Entites");
      ImGui::EndMenuBar();
    }
    childSelectableColumn(data, selected);
    ImGui::Columns(1);
    ImGui::Separator();
  }
  ImGui::EndChild();

  // components
  if (selected > -1 && ((size_t)selected < entities.size())) {
    ImGui::BeginChild("Components", ImVec2(0, size.y / 2), true,
                      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);
    if (ImGui::BeginMenuBar()) {
      ImGui::Text("Components");
      ImGui::EndMenuBar();
    }
    if (auto it = entities.find(selected + 1); it != entities.end()) {
      EntityId id = it->first;
      auto &coordinator = ecs::Coordinator::getInstance();
      if (coordinator.hasComponent<component::Transform>(id)) {
        auto &transform = coordinator.getComponent<component::Transform>(id);
        transform = showTransformComponent(transform);
      }
      if (coordinator.hasComponent<component::Light>(id)) {
        auto &light = coordinator.getComponent<component::Light>(id);
        light = showLightComponent(light);
      }
      if (coordinator.hasComponent<component::Model>(id)) {
        auto &model = coordinator.getComponent<component::Model>(id);
        model = showModelComponent(model);
      }
    }
    ImGui::EndChild();
  }
  ImGui::End();
} // namespace app

void AppUi::showSettingsWindow(bool *pclose) {
  if (ImGui::Begin("Editor Settings", pclose, globalWindowsFlags)) {
    if (ImGui::CollapsingHeader("Scene")) {
      ImGui::SliderInt("Scale", &sceneScale, 20.0f, 100.0f, "%d%%", ImGuiSliderFlags_NoInput);
      if (ImGui::TreeNode("GridPlaneSettings")) {
        showGridPlaneSettings();
        ImGui::TreePop();
      }
    }
  }
  ImGui::End();
}

void AppUi::showFileMenu() {
  ImGui::MenuItem("(WIP menu)", NULL, false, false);
  if (ImGui::MenuItem("New")) {
  }
  if (ImGui::MenuItem("Open", "Ctrl+O")) {
  }
  if (ImGui::BeginMenu("Open Recent")) {
    ImGui::MenuItem("fish_hat.c");
    ImGui::MenuItem("fish_hat.inl");
    ImGui::MenuItem("fish_hat.h");
    if (ImGui::BeginMenu("More..")) {
      ImGui::MenuItem("Hello");
      ImGui::MenuItem("Sailor");
      if (ImGui::BeginMenu("Recurse..")) {
        showFileMenu();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenu();
  }
  if (ImGui::MenuItem("Save", "Ctrl+S")) {
  }
  if (ImGui::MenuItem("Save As..")) {
  }

  ImGui::Separator();
  if (ImGui::BeginMenu("Options")) {
    static bool enabled = true;
    ImGui::MenuItem("Enabled", "", &enabled);
    ImGui::BeginChild("child", ImVec2(0, 60), true);
    for (int i = 0; i < 10; i++)
      ImGui::Text("Scrolling Text %d", i);
    ImGui::EndChild();
    static float f = 0.5f;
    static int n = 0;
    ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
    ImGui::InputFloat("Input", &f, 0.1f);
    ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Colors")) {
    float sz = ImGui::GetTextLineHeight();
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
      const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz),
                                                ImGui::GetColorU32((ImGuiCol)i));
      ImGui::Dummy(ImVec2(sz, sz));
      ImGui::SameLine();
      ImGui::MenuItem(name);
    }
    ImGui::EndMenu();
  }

  // Here we demonstrate appending again to the "Options" menu (which we already created above)
  // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options")
  // twice. In a real code-base using it would make senses to use this feature from very different
  // code locations.
  if (ImGui::BeginMenu("Options")) // <-- Append!
  {
    static bool b = true;
    ImGui::Checkbox("SomeOption", &b);
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Disabled", false)) // Disabled
  {
    IM_ASSERT(0);
  }
  if (ImGui::MenuItem("Checked", NULL, true)) {
  }
  if (ImGui::MenuItem("Quit", "Alt+F4")) {
    shouldClose = true;
  }
}
void AppUi::showMainMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      showFileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Toolbars")) {
      if (ImGui::MenuItem("Stats", NULL, statsOpen)) {
        statsOpen = true;
      }
      if (ImGui::MenuItem("Render System", NULL, renderSystemOpen)) {
        renderSystemOpen = true;
      }
      if (ImGui::MenuItem("Entity Window", NULL, entityWindowOpen)) {
        entityWindowOpen = true;
      }
      if (ImGui::MenuItem("Entity List", NULL, false, false)) {
      } // Disabled item
      if (ImGui::MenuItem("Editor Setting", NULL, settingWindowOpen)) {
        settingWindowOpen = true;
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X")) {
      }
      if (ImGui::MenuItem("Copy", "CTRL+C")) {
      }
      if (ImGui::MenuItem("Paste", "CTRL+V")) {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void AppUi::show() {
  showMainMenuBar();
  if (statsOpen) {
    showStatsWindow(&statsOpen);
  }
  if (renderSystemOpen) {
    showRenderSystemWindow(&renderSystemOpen);
  }
  if (entityWindowOpen) {
    showEntityWinow(&entityWindowOpen);
  }
  if (settingWindowOpen) {
    showSettingsWindow(&settingWindowOpen);
  }
}

void AppUi::addFps(int fps) { fpsHistory[++fpsHistoryI % HISTORY_SIZE] = fps; }

void AppUi::showFrame(uint textureId, int texWidth, int texHeight) {
  ImVec2 displaySize = io.DisplaySize;
  ImGui::SetNextWindowPos(displaySize * 0.5f, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
  std::string title =
      "Rendered Scene " + std::to_string(texWidth) + "x" + std::to_string(texHeight) + " [SCALED]";
  ImGuiWindowFlags windowFlags = globalWindowsFlags;
  if (ImGui::Begin(title.c_str(), 0, windowFlags)) {
    ImVec2 sceneSize;
    if (displaySize.x > displaySize.y) {
      float ar = texWidth / (float)texHeight;
      float width = ar * displaySize.y;
      float height = displaySize.y;
      sceneSize = (ImVec2(width, height) * sceneScale / 100.0f);
    } else {
      float ar = texHeight / (float)texWidth;
      float width = displaySize.x;
      float height = ar * displaySize.y;
      sceneSize = (ImVec2(width, height) * sceneScale / 100.0f);
    }

    ImVec2 windowPos = ImGui::GetCursorScreenPos();
    sceneViewport = glm::vec4(windowPos.x, windowPos.y, sceneSize.x, sceneSize.y);
    ImGui::Image((ImTextureID)(uptr)textureId, sceneSize, ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::End();
}

bool AppUi::getShouldClose() const { return shouldClose; }

AppUi::EditorState AppUi::getEditorState() const { return editorState; }

AppUi::Texture AppUi::createTexture(uint id, uint target) {
  int w, h;
  GLenum GlTarget = (GLenum)target;
  if (target == (uint)GL_TEXTURE_CUBE_MAP) GlTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  glBindTexture(target, id);
  glGetTexLevelParameteriv(GlTarget, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GlTarget, 0, GL_TEXTURE_HEIGHT, &h);
  glBindTexture(target, 0);
  return {id, w, h, target};
}

float AppUi::distanceFromSeg(glm::vec2 a, glm::vec2 b, glm::vec2 p) {
  auto ab = b - a;
  auto ap = p - a;
  auto projPa = glm::clamp(glm::dot(ab, ap) / glm::dot(ab, ab), 0.0f, 1.0f) * ab;
  return abs(glm::length(ap - projPa));
}

void AppUi::setEnvMap(uint id, uint target) {
  pbrTextures.envMap = createTexture(id, target);
  pbrTextures.envMap.id = render_system::GuiRenderer::generateTextureMask(id, target, 1);
}
void AppUi::setDiffuseConvMap(uint id, uint target) {
  pbrTextures.diffuseConvMap = createTexture(id, target);
  pbrTextures.diffuseConvMap.id = render_system::GuiRenderer::generateTextureMask(id, target, 1);
}
void AppUi::setSpecularConvMap(uint id, uint target) {
  pbrTextures.specularConvMap = createTexture(id, target);
  pbrTextures.specularConvMap.id = render_system::GuiRenderer::generateTextureMask(id, target, 1);
}
void AppUi::setBrdfLUT(uint id, uint target) { pbrTextures.brdfLUT = createTexture(id, target); }

void AppUi::setCoordinateSpaceState(const CoordinateSpaceState &state) {
  coordinateSpaceState = state;
}

void AppUi::addLoadedMeshes(const render_system::ModelRegisterReturn &data) {
  for (int i = 0; i < data.meshIds.size(); ++i) {
    loadedMeshes.emplace_back(GPUMeshMetaData{data.meshIds[i], data.meshNames[i],
                                              data.numPrimitives[i], data.hasTexCoords[i],
                                              data.primIdToMatId[i], data.matIdToNameMap[i]});
  }
}

/* Receive Events */
void AppUi::receive(const event::EntityChanged &event) {
  using event::EntityChanged;
  switch (event.status) {
  case EntityChanged::Status::CREATED:
    entities.emplace(event.entity,
                     Entity{std::to_string(event.entity), event.signature.to_string()});
    break;
  case EntityChanged::Status::UPDATED:
    entities[event.entity] = {std::to_string(event.entity), event.signature.to_string()};
    break;
  case EntityChanged::Status::DELETED:
    entities.erase(event.entity);
    break;
  }
}

} // namespace app
