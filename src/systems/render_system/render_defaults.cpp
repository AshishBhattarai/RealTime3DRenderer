#include "render_defaults.h"
#include "texture.h"
#include "types.h"
#include "utils/image.h"
#include <string>

namespace render_system {
RenderDefaults::RenderDefaults(const Image *checkerImage)
    : checkerTexture(0), blackTexture(0), whiteTexture(0),
      camera(glm::vec3(0, 0, -10.0f)) {

  assert(checkerImage && "Invalid checker image received.");

  Texture texture = Texture(*checkerImage);
  this->checkerTexture = texture.moveId();
  texture.loadTexture((const uchar[]){255, 255, 255, 255}, 1, 1, 4);
  this->blackTexture = texture.moveId();
  texture.loadTexture((const uchar[]){0, 0, 0, 0}, 1, 1, 4);
  this->whiteTexture = texture.moveId();

  // default flat material
  flatMaterial.ao = 1.0f;
  flatMaterial.albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
  flatMaterial.emission = glm::vec3(0.0f, 0.0f, 0.0f);
  flatMaterial.metallic = 0.0f;
  flatMaterial.roughtness = 0.0f;
  flatMaterial.shaderType = ShaderType::FLAT_FORWARD_SHADER;

  // default material
  material.albedo = checkerTexture;
  material.normal = blackTexture;
  material.emission = blackTexture;
  material.metallicRoughness = blackTexture;
  material.ao = whiteTexture;
  material.shaderType = ShaderType::FORWARD_SHADER;
}

RenderDefaults::~RenderDefaults() {
  glDeleteTextures(1, &checkerTexture);
  glDeleteTextures(1, &blackTexture);
  glDeleteTextures(1, &whiteTexture);
  checkerTexture = 0;
  blackTexture = 0;
  whiteTexture = 0;
}
} // namespace render_system
