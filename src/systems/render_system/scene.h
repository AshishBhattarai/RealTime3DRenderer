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

struct Scene {
  const std::string name;

  /**
   * meshes, meshNames, hasTexCoords, primIdToMatId, matIdToNameList [i:i:i]
   * belong to same mesh
   */
  const std::vector<Mesh> meshes;
  const std::vector<std::string> meshNames;
  const std::vector<bool> hasTexCoords;
  const std::vector<std::map<PrimitiveId, MaterialId>>
      primIdToMatId; // list of map<PRIMITIVE_ID, MATERIAL_ID>>
  const std::vector<std::map<MaterialId, std::string>> matIdToNameList;

  // can't be const coz we need to move unique_ptr from it on render_system
  std::vector<std::unique_ptr<BaseMaterial>> materials;
};

/**
 * @brief The Scene:Loader class
 *
 * This class calls gl methods.
 *
 * Loads a glft models with rendereable meshes.
 * (i.e all the data loaded to gpu and ready to be rendered)
 *
 * Converts regular mesh data to renderable mesh.
 * This class is used as medium to load meshes into render_system.
 */
class SceneLoader {
private:
  struct ProcessMaterialRet {
    const std::string name;
    std::unique_ptr<BaseMaterial> material;
  };

  struct ProcessMeshRet {
    const Mesh mesh;
    std::vector<std::unique_ptr<BaseMaterial>> materials;
    const std::vector<std::string> materialNames;
    const std::map<PrimitiveId, MaterialId> primIdToMatId;
    const std::map<MaterialId, std::string> matIdToName;
    const bool hasTexCoords;
    const bool success;
    const std::string message; // contains reason if sucess == false
  };

  //  uint currentIbo;
  static MeshId loadedMeshCount;
  static MaterialId loadedMaterialCount;

  ProcessMeshRet processMesh(const std::map<int, GLuint> &vbos,
                             const tinygltf::Mesh &meshData,
                             const tinygltf::Model &modelData);

  ProcessMaterialRet processMaterial(const tinygltf::Material &materialData,
                                     const tinygltf::Model &modelData, bool hasTexCoords);
  GLuint processTexture(const tinygltf::Image &image);

public:
  Scene loadScene(tinygltf::Model &modelData);
  static MeshId generateMeshId() { return loadedMeshCount++; }
  static MaterialId generateMaterialId() { return loadedMaterialCount++; }
};

} // namespace render_system
