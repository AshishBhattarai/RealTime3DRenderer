#pragma once
#include "mesh.h"
#include <map>
#include <string>
#include <vector>

namespace tinygltf {
struct Model;
struct Mesh;
struct Image;
struct Material;
} // namespace tinygltf

namespace render_system {

/**
 * @brief The Model class
 *
 * This class calls gl methods.
 *
 * Represents a glft scene with rendereable meshes.
 * (i.e all the data loaded to gpu and ready to be rendered)
 *
 * Converts regular mesh data to renderable mesh.
 * This class is used as medium to load meshes into render_system.
 */
class Model {
private:
  uint currentIbo;
  friend class RenderSystem;

  std::string name;
  std::vector<Mesh> meshes;

  Mesh processMesh(const std::map<int, GLuint> &vbos,
                   const tinygltf::Mesh &meshData,
                   const tinygltf::Model &modelData);

  std::unique_ptr<BaseMaterial>
  processMaterial(const tinygltf::Material &materialData,
                  const tinygltf::Model &modelData, bool hasTextureCoords);
  GLuint processTexture(const tinygltf::Image &image);

public:
  Model() = default;
  Model(Model &&model) = default;
  Model(tinygltf::Model &modelData);
  void loadModel(tinygltf::Model &modelData);
  std::string getName() const { return name; }
  bool hasMeshes() const { return meshes.size(); }
};

} // namespace render_system
