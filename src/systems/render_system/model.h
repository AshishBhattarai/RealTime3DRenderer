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
 * Represents a glft scene with rendereable meshes.
 * (i.e all the data loaded to gpu and ready to be rendered)
 */
class Model {
public:
  Model(tinygltf::Model &modelData);

private:
  std::string name;
  std::vector<Mesh> meshes;

  Mesh processMesh(const std::map<int, GLuint> &vbos,
                   const tinygltf::Mesh &meshData,
                   const tinygltf::Model &modelData);

  std::unique_ptr<Material>
  processMaterial(const tinygltf::Material &materialData,
                  const tinygltf::Model &modelData);
  GLuint processTexture(const tinygltf::Image &image);
};

} // namespace render_system
