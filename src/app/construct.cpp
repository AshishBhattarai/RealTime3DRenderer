#include "construct.h"

#include "core/buffer.h"
#include "core/image.h"
#include "loaders.h"
#include "systems/render_system/render_system.h"

namespace app {
using namespace render_system;

Construct::Construct() {}

render_system::RenderSystem *Construct::newRenderSystem(int width, int height) {
  // TODO: Move this to a function in app and remove this class
  /* Init RenderSystem */
  Image checkerImage;
  bool status = Loaders::loadImage(checkerImage, "resources/defaults/checker.bmp");
  Buffer flatForwardVertex, flatForwardFragment, textureForwardVertex, textureForwardFragment,
      skyboxVertex, cubemapVertex, cubemapFragment, equirectangularFragment, visualPrepVertex,
      visualPrepFragment, iblConvolutionFragment, iblSpecularConvolutionFragment,
      iblBrdfIntegrationFragment, guiVertex, guiFragment;
  status = Loaders::loadBinaryFile(flatForwardVertex, "shaders/flat_forward_material_vert.spv");
  status = Loaders::loadBinaryFile(flatForwardFragment, "shaders/flat_forward_material_frag.spv");
  status =
      Loaders::loadBinaryFile(textureForwardVertex, "shaders/texture_forward_material_vert.spv");
  status =
      Loaders::loadBinaryFile(textureForwardFragment, "shaders/texture_forward_material_frag.spv");
  status = Loaders::loadBinaryFile(skyboxVertex, "shaders/skybox_vert.spv");
  status = Loaders::loadBinaryFile(cubemapVertex, "shaders/cubemap_vert.spv");
  status = Loaders::loadBinaryFile(cubemapFragment, "shaders/cubemap_frag.spv");
  status = Loaders::loadBinaryFile(equirectangularFragment, "shaders/equirectangular_frag.spv");
  status = Loaders::loadBinaryFile(visualPrepVertex, "shaders/visualprep_vert.spv");
  status = Loaders::loadBinaryFile(visualPrepFragment, "shaders/visualprep_frag.spv");
  status =
      Loaders::loadBinaryFile(iblConvolutionFragment, "shaders/ibl_diffuse_convolution_frag.spv");
  status = Loaders::loadBinaryFile(iblSpecularConvolutionFragment,
                                   "shaders/ibl_specular_convolution_frag.spv");
  status =
      Loaders::loadBinaryFile(iblBrdfIntegrationFragment, "shaders/ibl_brdf_integration_map.spv");
  status = Loaders::loadBinaryFile(guiVertex, "shaders/gui_vert.spv");
  status = Loaders::loadBinaryFile(guiFragment, "shaders/gui_frag.spv");

  return new RenderSystem(
      {checkerImage,
       /**
        * const shader::StageCodeMap &flatForwardShader;
        * const shader::StageCodeMap &textureForwardShader; const shader::StageCodeMap
        * &skyboxShader; const shader::StageCodeMap &cubemapShader; const shader::StageCodeMap
        * &equirectangularShader; const shader::StageCodeMap &visualPrepShader; const
        * shader::StageCodeMap &iblConvolutionShader; const shader::StageCodeMap
        * &iblSpecularConvolutionShader; const shader::StageCodeMap &iblBrdfIntegrationShader;
        * guiVertex;
        * guiFragment
        */
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, flatForwardVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, flatForwardFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, textureForwardVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, textureForwardFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, skyboxVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, cubemapFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, cubemapFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, equirectangularFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, visualPrepVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, visualPrepFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, iblConvolutionFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, iblSpecularConvolutionFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, visualPrepVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, iblBrdfIntegrationFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, guiVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, guiFragment}},
       width, height, width / (float)height});
} // namespace app
} // namespace app
