#pragma once
#include "shaders/gui_shader.h"
#include "types.h"
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
  struct TextureProperties {
    GLenum target;
    GLuint id;
    int face;
    int lod;
  };

  GuiRenderer(const shader::StageCodeMap &codeMap);
  ~GuiRenderer();

  void render();

  static GLuint generateTextureMask(GLuint id, GLenum target, u8 face = 0);
  static TextureProperties decodeTextureMask(GLuint id);
};

} // namespace render_system
