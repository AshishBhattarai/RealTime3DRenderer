#include "app_ui.h"
#include "systems/render_system/gui_renderer.h"
#include "systems/render_system/texture.h"
#include "types.h"
#include <imgui/imgui.h>
#include <numeric>
#include <string>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui/imgui_internal.h>

static int fpsHistoryI = 0;
static int dtHistoryI = 0;

/* Window status */
static bool statsOpen = true;
static bool renderSystemOpen = true;
static int sceneScale = 68;

static uint globalWindowsFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;

namespace app {
AppUi::AppUi() : io(ImGui::GetIO()), shouldClose(false) { fpsHistory.fill(60); }

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

void AppUi::showRenderSystemWindow(bool *pclose) {
  if (ImGui::Begin("Render System", pclose, globalWindowsFlags)) {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Renderer");
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
        childImageView("Specular Convolution Map", pbrTextures.specularConvMap, &specularConvFace,
                       &specularConvLod);
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
    ImGui::SliderInt("Scene scale", &sceneScale, 20.0f, 100.0f, "%d%%", ImGuiSliderFlags_NoInput);
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
      if (ImGui::MenuItem("Entity List", NULL, false, false)) {
      } // Disabled item
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
    ImGui::Image((ImTextureID)(uptr)textureId, sceneSize, ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::End();
}

bool AppUi::getShouldClose() const { return shouldClose; }

AppUi::Texture AppUi::createTexture(uint id, uint target) {
  int w, h;
  GLenum GlTarget = (GLenum)target;
  glBindTexture(GlTarget, id);
  glGetTexLevelParameteriv(GlTarget, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GlTarget, 0, GL_TEXTURE_HEIGHT, &h);
  glBindTexture(GlTarget, 0);
  return {id, w, h, target};
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

} // namespace app
