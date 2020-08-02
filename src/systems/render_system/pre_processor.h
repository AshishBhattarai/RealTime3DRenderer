#pragma once

#include "shaders/cubemap.h"
#include "texture.h"
#include <functional>

namespace render_system {
/**
 * @brief The PreProcessor class
 * Contains all the pre-processing code.
 * ie:
 * a) Gnerating convolution & integration map for IBLs
 * b) Converting equirectangular maps to cube maps.
 */
class PreProcessor {
private:
  shader::Cubemap cubemapShader;
  shader::Cubemap equirectangularShader;
  GLuint cube; // temp

  Texture renderToCubeMap(int width, int height, uint maxMipLevels,
                          bool genMipMap, shader::Cubemap *const shader,
                          std::function<void(uint mipLevel)> preDrawCall);

public:
  PreProcessor(const shader::StageCodeMap &cubemapShader,
               const shader::StageCodeMap &equirectangularShader);

  Texture equirectangularToCubemap(const Texture &equirectangular);
};
} // namespace render_system
