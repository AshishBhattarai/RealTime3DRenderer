#include "app_ui.h"
#include "systems/render_system/texture.h"
#include "types.h"
#include <imgui/imgui.h>
#include <string>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui/imgui_internal.h>

namespace app {

AppUi::AppUi() : fullScreenFrame(false) {}

void AppUi::showFrame(uint textureId, int texWidth, int texHeight) {
  ImGuiIO &io = ImGui::GetIO();
  ImVec2 displaySize = io.DisplaySize;

  ImGui::SetNextWindowPos(displaySize * 0.5f, fullScreenFrame ? ImGuiCond_Always : ImGuiCond_Once,
                          ImVec2(0.5f, 0.5f));

  std::string title =
      "Rendered Scene " + std::to_string(texWidth) + "x" + std::to_string(texHeight) + " [SCALED]";
  if (!ImGui::Begin(title.c_str(), 0,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize)) {
    // collapsed
    ImGui::End();
    return;
  }

  float ar = texWidth / (float)texHeight;
  int width = ar * displaySize.y;
  ImVec2 sceneSize = fullScreenFrame ? displaySize : (ImVec2((float)width, displaySize.y) * 0.7f);

  ImGui::Image((ImTextureID)(uptr)textureId, sceneSize, ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
}

void AppUi::show() {}
} // namespace app
