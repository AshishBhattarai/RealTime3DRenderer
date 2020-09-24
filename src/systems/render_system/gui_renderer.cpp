#include "gui_renderer.h"
#include "systems/render_system/shaders/config.h"
#include "systems/render_system/shaders/program.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

namespace render_system {

GuiRenderer::GuiRenderer(const shader::StageCodeMap &codeMap)
    : shader(codeMap), vao(0), vbo(0), ebo(0), fontTexture(0) {

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  // setup attributes
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glEnableVertexAttribArray(shader::gui::vertex::attribute::POSITION_LOC);
  glEnableVertexAttribArray(shader::gui::vertex::attribute::TEXCOORD0_LOC);
  glEnableVertexAttribArray(shader::gui::vertex::attribute::COLOR_LOC);

  glVertexAttribPointer(shader::gui::vertex::attribute::POSITION_LOC, 2, GL_FLOAT, GL_FALSE,
                        sizeof(ImDrawVert), (void *)IM_OFFSETOF(ImDrawVert, pos));
  glVertexAttribPointer(shader::gui::vertex::attribute::TEXCOORD0_LOC, 2, GL_FLOAT, GL_FALSE,
                        sizeof(ImDrawVert), (void *)IM_OFFSETOF(ImDrawVert, uv));
  glVertexAttribPointer(shader::gui::vertex::attribute::COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                        sizeof(ImDrawVert), (void *)IM_OFFSETOF(ImDrawVert, col));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  buildFonts();
}

GuiRenderer::~GuiRenderer() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteTextures(1, &fontTexture);

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->TexID = 0;
}

void GuiRenderer::buildFonts() {
  // Build texture atlas for fonts
  ImGuiIO &io = ImGui::GetIO();
  unsigned char *data; // pixel data
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);
  // load into opengl
  glBindTexture(GL_TEXTURE_2D, fontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // store the id
  io.Fonts->TexID = (ImTextureID)(intptr_t)fontTexture;
}

void GuiRenderer::setupRenderState(ImDrawData *drawData, const glm::ivec2 fbSize) {
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  GLenum currentClipOrigin = 0;
  /* glClipControl is only on opengl 4.5+ */
  glGetIntegerv(GL_CLIP_ORIGIN, (GLint *)&currentClipOrigin);
  glViewport(0, 0, fbSize.x, fbSize.y);

  // TODO: change ortho mat only on resize ??
  float L = drawData->DisplayPos.x;
  float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
  float T = drawData->DisplayPos.y;
  float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
  if (currentClipOrigin == GL_UPPER_LEFT) {
    // Swap top and bottom if origin is upper left
    float tmp = T;
    T = B;
    B = tmp;
  }
  glm::mat4 orthoProjection = glm::ortho(L, R, B, T);
  shader.bind();
  shader.loadProjectionMat(orthoProjection);
  glBindSampler(shader::gui::fragment::TEXTURE_BND, 0); // reset texture properties
  glActiveTexture(GL_TEXTURE0 + shader::gui::fragment::TEXTURE_BND);
  glBindVertexArray(vao);
}

void GuiRenderer::render() {
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui::Render();
  ImDrawData *drawData = ImGui::GetDrawData();
  /**
   * Avoid rendering when minimized, scale coordinates for retina displays
   * (screen coordinates != framebuffer coordinates)
   */
  int fbWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
  int fbHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
  if (fbWidth <= 0 || fbHeight <= 0) return;

  // backup state
  GLint lastPolygonMode, lastViewport[4], lastScissorBox[4];
  GLenum lastBlendSrcRgb, lastBlendDstRgb, lastBlendSrcAlpha, lastBlendDstAlpha, lastBlendEquRgb,
      lastBlendEquAlpha;
  glGetIntegerv(GL_POLYGON_MODE, &lastPolygonMode);
  glGetIntegerv(GL_VIEWPORT, lastViewport);
  glGetIntegerv(GL_SCISSOR_BOX, lastScissorBox);
  glGetIntegerv(GL_BLEND_SRC_RGB, (GLint *)&lastBlendSrcRgb);
  glGetIntegerv(GL_BLEND_DST_RGB, (GLint *)&lastBlendDstRgb);
  glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint *)&lastBlendSrcAlpha);
  glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint *)&lastBlendDstAlpha);
  glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint *)&lastBlendEquRgb);
  glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint *)&lastBlendEquAlpha);
  GLboolean lastEnableBlend = glIsEnabled(GL_BLEND);
  GLboolean lastEnableCullFace = glIsEnabled(GL_CULL_FACE);
  GLboolean lastEnableDepthTest = glIsEnabled(GL_DEPTH_TEST);
  GLboolean lastEnableScissorTest = glIsEnabled(GL_SCISSOR_TEST);

  // set state
  setupRenderState(drawData, glm::ivec2(fbWidth, fbHeight));

  // render
  // (0,0) unless using multi-viewports
  ImVec2 clipOff = drawData->DisplayPos;
  // (1,1) unless using retina display which are often (2,2)
  ImVec2 clipScale = drawData->FramebufferScale;

  // Render command lists
  for (int n = 0; n < drawData->CmdListsCount; n++) {
    const ImDrawList *cmdList = drawData->CmdLists[n];

    // Upload vertex/index buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmdList->VtxBuffer.Size * (int)sizeof(ImDrawVert),
                 (const GLvoid *)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)cmdList->IdxBuffer.Size * (int)sizeof(ImDrawIdx),
                 (const GLvoid *)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

    for (int cmdI = 0; cmdI < cmdList->CmdBuffer.Size; cmdI++) {
      const ImDrawCmd *pcmd = &cmdList->CmdBuffer[cmdI];
      if (pcmd->UserCallback) {
        /**
         * User callback, registered via ImDrawList::AddCallback()
         * (ImDrawCallback_ResetRenderState is a special callback value used by the user to request
         * the renderer to reset render state.)
         */
        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
          setupRenderState(drawData, glm::ivec2(fbWidth, fbHeight));
        else
          pcmd->UserCallback(cmdList, pcmd);
      } else {
        // Project scissor/clipping rectangles into framebuffer space
        ImVec4 clipRect;
        clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
        clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
        clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
        clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

        if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f &&
            clipRect.w >= 0.0f) {
          // Apply scissor/clipping rectangle
          glScissor((int)clipRect.x, (int)(fbHeight - clipRect.w), (int)(clipRect.z - clipRect.x),
                    (int)(clipRect.w - clipRect.y));

          // Bind texture, Draw
          const GLuint id = (GLuint)(intptr_t)pcmd->TextureId;
          const TextureProperties properties = decodeTextureMask(id);
          shader.loadFace(properties.face);
          shader.loadLod((float)properties.lod);
          if (properties.face)
            glActiveTexture(GL_TEXTURE0 + shader::gui::fragment::TEXTURE_CUBE_BND);
          else
            glActiveTexture(GL_TEXTURE0 + shader::gui::fragment::TEXTURE_BND);
          glBindTexture(properties.target, properties.id);
          glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                                   sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                   (void *)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)),
                                   (GLint)pcmd->VtxOffset);
        }
      }
    }
  }

  // reset state
  glBlendEquationSeparate(lastBlendEquRgb, lastBlendEquAlpha);
  glBlendFuncSeparate(lastBlendSrcRgb, lastBlendDstRgb, lastBlendSrcAlpha, lastBlendDstAlpha);
  if (lastEnableBlend)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
  if (lastEnableCullFace)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
  if (lastEnableDepthTest)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
  if (lastEnableScissorTest)
    glEnable(GL_SCISSOR_TEST);
  else
    glDisable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, (GLenum)lastPolygonMode);
  glViewport(lastViewport[0], lastViewport[1], lastViewport[2], lastViewport[3]);
  glScissor(lastScissorBox[0], lastScissorBox[1], lastScissorBox[2], lastScissorBox[3]);
}

GLuint GuiRenderer::generateTextureMask(GLuint id, GLenum target, u8 face, u8 lod) {
  GLuint newId = id & 0x007FFFFF; // clear old, MSB 9-bit is for 0[FACE][MIP]
  face = std::clamp(face, (u8)0, (u8)6);
  lod = std::clamp(lod, (u8)0, (u8)10);
  switch (target) {
  case GL_TEXTURE_CUBE_MAP:
    // MSB is always 0 then mask top 4-bit for face
    newId = ((0x0F & face) << 27) | ((0x0F & lod) << 23) | newId;
    break;
  }
  return newId;
}
GuiRenderer::TextureProperties GuiRenderer::decodeTextureMask(GLuint id) {
  u8 lodFace = id >> 23;    // MSB 9 bit for id
  u8 face = (lodFace >> 4); // MSB 4-bit is face, if available
  u8 lod = (lodFace & 0x0F);
  id = id & 0x007FFFFF;
  if (face)
    return {GL_TEXTURE_CUBE_MAP, id, face, lod};
  else
    return {GL_TEXTURE_2D, id, 0, 0};
}

}; // namespace render_system
