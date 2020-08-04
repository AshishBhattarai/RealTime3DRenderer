#pragma once

#include "shaders/cubemap.h"
#include "shaders/ibl_specular_convolution.h"
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
  shader::Cubemap iblDiffuseConvolutionShader;
  shader::IBLSpecularConvolution iblSpecularConvolutionShader;
  shader::Program iblBRDFIntegrationShader;

  GLuint cube, plane; // temp

  Texture renderToCubeMap(int width, int height, uint maxMipLevels,
                          bool genMipMap, shader::Cubemap *const shader,
                          std::function<void(uint mipLevel)> preDrawCall);

public:
  PreProcessor(const shader::StageCodeMap &cubemapShader,
               const shader::StageCodeMap &equirectangularShader,
               const shader::StageCodeMap &iblDiffuseConvolutionShader,
               const shader::StageCodeMap &iblSpecularConvolutionShader,
               const shader::StageCodeMap &iblBRDFIntegrationShader);

  Texture equirectangularToCubemap(const Texture &equirectangular);
  Texture generateIrradianceMap(const Texture &envmap);
  Texture generatePreFilteredMap(const Texture &envmap);
  Texture generateBRDFIntegrationMap();
};
} // namespace render_system
