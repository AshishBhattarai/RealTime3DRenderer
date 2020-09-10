#pragma once
#include "shaders/gui_shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class ImDrawData;
namespace render_system {
class GuiRenderer {
private:
  shader::GuiShader shader;

  /**
   * NOTE: recreate vao every time to allow multi context renderering, vao's aren't shared.
   * 	   check imgui opengl3 example.
   **/
  // render buffers for ui
  GLuint vao, vbo, ebo, fontTexture;

  /**
   * Build font bitmap
   */
  void buildFonts();

  /**
   * To be called on specific imgui state
   **/
  void setupRenderState(ImDrawData *drawData, const glm::ivec2 fbSize);

public:
  GuiRenderer(const shader::StageCodeMap &codeMap);
  void render();
  ~GuiRenderer();
};

} // namespace render_system
