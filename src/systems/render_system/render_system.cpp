#include "render_system.h"
#include "camera.h"
#include "components/light.h"
#include "components/model.h"
#include "components/transform.h"
#include "core/image.h"
#include "ecs/coordinator.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "scene.h"
#include "shaders/config.h"
#include "utils/slogger.h"

namespace render_system {

/**
 * Sub-system which is used to register lights in RenderSystem
 */
class RenderSystem::LightingSystem : ecs::System<LightingSystem> {
  friend class RenderSystem;
};

/**
 * @brief RenderSystem::initSubSystems
 * Init sub-system to the render system.
 * i) LightSystem
 */
void RenderSystem::initSubSystems() {
  ecs::ComponentFamily lightFamily =
      coordinator.getComponentFamily<component::Light>();
  ecs::ComponentFamily transformFamily =
      coordinator.getComponentFamily<component::Transform>();
  ecs::Signature sig;
  sig.set(lightFamily, true);
  sig.set(transformFamily, true);
  coordinator.registerSystem<LightingSystem>(sig);
  sig.reset();
  lightingSystem = new LightingSystem();
}

bool RenderSystem::initSingletons(const Image &checkerImage) {
  /* init RenderDefaults */
  RenderDefaults::getInstance(&checkerImage);
  return true;
}

RenderSystem::RenderSystem(const RenderSystemConfig &config)
    : status(initSingletons(config.checkerImage)),
      preProcessor(config.cubemapShader, config.equirectangularShader,
                   config.iblConvolutionShader,
                   config.iblSpecularConvolutionShader,
                   config.iblBrdfIntegrationShader),
      renderer(RendererConfig{config.width, config.height, meshes, materials,
                              &RenderDefaults::getInstance().getCamera(),
                              config.flatForwardShader, config.skyboxShader,
                              preProcessor.generateBRDFIntegrationMap()}),
      postProcessor(config.visualPrepShader),
      framebuffer(config.width, config.height), sceneLoader(),
      coordinator(ecs::Coordinator::getInstance()), skybox(nullptr) {
  /* update projection */
  updateProjectionMatrix(config.ar);
  /* setup framebuffer */
  framebuffer.use();
  framebuffer.setColorAttachmentTB(GL_TEXTURE_2D, GL_RGB16F, GL_RGB, GL_FLOAT);
  framebuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);
  assert(framebuffer.isComplete() && "Framebuffer not complete.");
  framebuffer.useDefault();
  /* load default materials */
  auto &renderDefaults = RenderDefaults::getInstance();
  materials.emplace(DEFAULT_MATERIAL_ID,
                    std::unique_ptr<Material>(new Material(
                        {{DEFAULT_MATERIAL_ID, ShaderType::FORWARD_SHADER},
                         renderDefaults.getCheckerTexture(),
                         renderDefaults.getBlackTexture(),
                         renderDefaults.getBlackTexture(),
                         renderDefaults.getBlackTexture(),
                         renderDefaults.getBlackTexture()})));
  materials.emplace(DEFAULT_FLAT_MATERIAL_ID,
                    std::unique_ptr<FlatMaterial>(new FlatMaterial(
                        {BaseMaterial{DEFAULT_FLAT_MATERIAL_ID,
                                      ShaderType::FLAT_FORWARD_SHADER},
                         glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                         glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0, 1.0f})));

  /* Register & init helper systems(sub-systems) */
  initSubSystems();

  /*
   * Handle Lights(Entities with light component)
   */
  lightingSystem->connectEntityRemovedSignal([](const ecs::Entity &) {
    // TODO
  });
}

RenderSystem::~RenderSystem() { delete lightingSystem; }

SceneRegisterReturn
RenderSystem::registerGltfScene(tinygltf::Model &modelData) {
  auto sceneData = sceneLoader.loadScene(modelData);
  std::vector<MeshId> ids;
  std::vector<uint> numPrimitives;
  for (size_t i = 0; i < sceneData.meshes.size(); ++i) {
    std::string name = sceneData.meshNames[i];
    uint meshId = sceneData.meshes[i].id;
    meshes.emplace(meshId, sceneData.meshes[i]);
    numPrimitives.emplace_back(sceneData.meshes[i].primitives.size());
    ids.emplace_back(meshId);
  }
  for (auto &mat : sceneData.materials) {
    materials.emplace(mat->id, std::move(mat));
  }
  return {sceneData.name,           ids,
          sceneData.meshNames,      numPrimitives,
          sceneData.hasTexCoords,   sceneData.primIdToMatId,
          sceneData.matIdToNameList};
}

bool RenderSystem::setSkyBox(Image *image) {
  auto equiTex = Texture(*image, toUnderlying(TextureFlags::DISABLE_MIPMAP));
  skybox =
      std::make_unique<Texture>(preProcessor.equirectangularToCubemap(equiTex));
  globalDiffuseIBL =
      std::make_unique<Texture>(preProcessor.generateIrradianceMap(*skybox));
  globalSpecularIBL =
      std::make_unique<Texture>(preProcessor.generatePreFilteredMap(*skybox));
  return skybox->getId() != 0;
}

std::shared_ptr<Image> RenderSystem::update(float dt) {
  // load preRender data
  framebuffer.use();
  renderer.preRender();

  // load lights
  uint i = 0;
  for (EntityId entity : lightingSystem->getEntites()) {
    if (i == shader::forward::fragment::PointLight::MAX)
      break;
    const auto &transfrom =
        coordinator.getComponent<component::Transform>(entity);
    const auto &light = coordinator.getComponent<component::Light>(entity);
    PointLight pointLight(&transfrom.transformMat[3], &light.color,
                          &light.range, &light.intensity, entity);
    renderer.loadPointLight(pointLight, i);
    ++i;
  }
  renderer.loadPointLightCount(i);

  // draw skybox
  if (skybox) {
    renderer.renderSkybox(*skybox);
  }

  // render entites
  renderer.preRenderMesh(*globalDiffuseIBL, *globalSpecularIBL);
  for (EntityId entity : this->getEntites()) {
    auto transform =
        coordinator.getComponent<component::Transform>(entity).transformMat;
    auto model = coordinator.getComponent<component::Model>(entity);
    renderer.renderMesh(dt, transform, model.meshId, model.primIdToMatId);
  }

  // post process
  Texture frameTexture =
      Texture(framebuffer.getColorAttachmentId(), GL_TEXTURE_2D);
  framebuffer.useDefault();
  postProcessor.applyVisualPrep(frameTexture);
  frameTexture.release(); // this is a hack, reThink??

  return std::make_shared<Image>(FrameBuffer::readPixelsWindow());
}

} // namespace render_system
