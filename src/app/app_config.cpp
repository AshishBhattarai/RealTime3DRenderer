#include "app_config.h"
#include "loaders.h"
#include "systems/render_system/render_system.h"
#include "utils/buffer.h"
#include "utils/image.h"

namespace app {
using namespace render_system;

render_system::RenderSystem *AppConfig::newRenderSystem(float ar) {
  /* Init RenderSystem */
  Image checkerImage;
  bool status =
      Loaders::loadImage(checkerImage, "resources/defaults/checker.bmp");
  Buffer flatForwardVertex;
  Buffer flatForwardFragment;
  status = Loaders::loadBinaryFile(flatForwardVertex,
                                   "shaders/flat_forward_vert.spv");
  status = Loaders::loadBinaryFile(flatForwardFragment,
                                   "shaders/flat_forward_frag.spv");
  return new RenderSystem(RenderSystemConfig(
      checkerImage,
      shader::StageCodeMap{
          {shader::ShaderStage::VERTEX_SHADER, flatForwardVertex},
          {shader::ShaderStage::FRAGMENT_SHADER, flatForwardFragment}},
      ar));
}

AppConfig::AppConfig() {}
AppConfig::~AppConfig() {}

} // namespace app
