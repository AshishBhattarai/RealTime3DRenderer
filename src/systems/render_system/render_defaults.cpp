#include "render_defaults.h"
#include "core/image.h"
#include "texture.h"
#include "types.h"
#include <string>

namespace render_system {
RenderDefaults::RenderDefaults(const Image *checkerImage)
    : checkerTexture(0), blackTexture(0), whiteTexture(0),
      camera(glm::vec3(0, 0, -10.0f)) {

  assert(checkerImage && "Invalid checker image received.");

  this->checkerTexture = Texture(*checkerImage).release();
  this->blackTexture =
      Texture((const uchar[]){255, 255, 255, 255}, 1, 1, 4).release();
  this->whiteTexture = Texture((const uchar[]){0, 0, 0, 0}, 1, 1, 4).release();
  ;
} // namespace render_system

RenderDefaults::~RenderDefaults() {
  glDeleteTextures(1, &checkerTexture);
  glDeleteTextures(1, &blackTexture);
  glDeleteTextures(1, &whiteTexture);
  checkerTexture = 0;
  blackTexture = 0;
  whiteTexture = 0;
}
} // namespace render_system
