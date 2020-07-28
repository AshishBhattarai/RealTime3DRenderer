#include "construct.h"

#include "core/buffer.h"
#include "core/image.h"
#include "loaders.h"
#include "systems/render_system/render_system.h"

namespace app {
using namespace render_system;

Construct::Construct() {}

render_system::RenderSystem *Construct::newRenderSystem(int width, int height) {
  /* Init RenderSystem */
  Image checkerImage;
  bool status =
      Loaders::loadImage(checkerImage, "resources/defaults/checker.bmp");
  Buffer flatForwardVertex, flatForwardFragment, skyboxVertex, skyboxFragment,
      skyboxCubeMapFragment, visualPrepVertex, visualPrepFragment,
      iblConvolutionFragment, iblSpecularConvolutionFragment,
      iblBrdfIntegrationFragment;
  status = Loaders::loadBinaryFile(flatForwardVertex,
                                   "shaders/flat_forward_vert.spv");
  status = Loaders::loadBinaryFile(flatForwardFragment,
                                   "shaders/flat_forward_frag.spv");
  status = Loaders::loadBinaryFile(skyboxVertex, "shaders/skybox_vert.spv");
  status = Loaders::loadBinaryFile(skyboxFragment, "shaders/skybox_frag.spv");
  status = Loaders::loadBinaryFile(skyboxCubeMapFragment,
                                   "shaders/skybox_cubemap_frag.spv");
  status =
      Loaders::loadBinaryFile(visualPrepVertex, "shaders/visualprep_vert.spv");
  status = Loaders::loadBinaryFile(visualPrepFragment,
                                   "shaders/visualprep_frag.spv");
  status = Loaders::loadBinaryFile(iblConvolutionFragment,
                                   "shaders/ibl_diffuse_convolution_frag.spv");
  status = Loaders::loadBinaryFile(iblSpecularConvolutionFragment,
                                   "shaders/ibl_specular_convolution_frag.spv");
  status = Loaders::loadBinaryFile(iblBrdfIntegrationFragment,
                                   "shaders/ibl_brdf_integration_map.spv");

  return new RenderSystem(
      {checkerImage,
       shader::StageCodeMap{
           {shader::ShaderStage::VERTEX_SHADER, flatForwardVertex},
           {shader::ShaderStage::FRAGMENT_SHADER, flatForwardFragment}},
       shader::StageCodeMap{
           {shader::ShaderStage::VERTEX_SHADER, skyboxVertex},
           {shader::ShaderStage::FRAGMENT_SHADER, skyboxFragment}},
       shader::StageCodeMap{
           {shader::ShaderStage::VERTEX_SHADER, skyboxVertex},
           {shader::ShaderStage::FRAGMENT_SHADER, skyboxCubeMapFragment}},
       shader::StageCodeMap{
           {shader::ShaderStage::VERTEX_SHADER, visualPrepVertex},
           {shader::ShaderStage::FRAGMENT_SHADER, visualPrepFragment}},
       shader::StageCodeMap{
           {shader::ShaderStage::VERTEX_SHADER, skyboxVertex},
           {shader::ShaderStage::FRAGMENT_SHADER, iblConvolutionFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, skyboxVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER,
                             iblSpecularConvolutionFragment}},
       shader::StageCodeMap{
           {shader::ShaderStage::VERTEX_SHADER, visualPrepVertex},
           {shader::ShaderStage::FRAGMENT_SHADER, iblBrdfIntegrationFragment}},
       width, height, width / (float)height});
} // namespace app
} // namespace app
