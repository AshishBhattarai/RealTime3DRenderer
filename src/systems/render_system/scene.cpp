#include "scene.h"
#include "render_defaults.h"
#include "shaders/config.h"
#include "utils/slogger.h"
#include <iostream>
#include <third_party/tinygltf/tiny_gltf.h>

namespace render_system {

/**
 * GLTF Terms
 *
 * 1) Buffer -> Compact data which contains all the geometric data and textures
 * of a model
 *
 * 2) BufferView -> It represent a part of the buffer, which might contain data
 * about a texture, geometric data such and position, normal, texture coords.
 * Can be used create VBOS
 *
 * 3) Accessor -> It contains information about layout and type of data
 * represented by BufferView. (also
 * Can be used to create vertex attrib pointers of vbos
 *
 * 4) Primitives -> They represent the sub-mesh that is actually rendered.
 * They contain information about the
 * attributes [POSITION, NROMAL, TEXT_COORDs with index respective accessor],
 * render mode [GL_POINTS, GL_TRIANGLES etc],
 * material [index to material],
 * indices [index to accessor]
 *
 * more:
 * 	https://www.khronos.org/files/gltf20-reference-guide.pdf
 *
 */

uint SceneLoader::loadedMeshCount = 1;
uint SceneLoader::loadedMaterialCount = 2;

Scene SceneLoader::loadScene(tinygltf::Model &modelData) {
  const tinygltf::Scene &scene = modelData.scenes[modelData.defaultScene];
  // load all buffer view into vbos

  std::map<int, GLuint> vbos;

  // store all buffer view on vbos
  for (size_t i = 0; i < modelData.bufferViews.size(); i++) {
    const tinygltf::BufferView &bufferView = modelData.bufferViews[i];
    // invalid buffer view (NOT ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER)
    if (bufferView.target == 0)
      continue;

    const tinygltf::Buffer &buffer = modelData.buffers[bufferView.buffer];

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data[0] + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  // load meshes
  std::vector<Mesh> meshes;
  std::vector<std::string> names;
  std::vector<std::unique_ptr<BaseMaterial>> materials;
  std::vector<std::map<MaterialId, std::string>> matIdToNameList;
  std::vector<std::map<PrimitiveId, MaterialId>>
      primIdToMatIdList; // map<Mesh_NAME, map<PRIMITIVE_ID, MATERIAL_ID>>
  std::vector<bool> hasTexCoords;

  for (const tinygltf::Mesh &meshData : modelData.meshes) {
    auto ret = processMesh(vbos, meshData, modelData);
    if (!ret.success) {
      SLOG(ret.message);
      continue;
    }
    meshes.push_back(ret.mesh);
    hasTexCoords.push_back(ret.hasTexCoords);
    names.push_back(meshData.name);
    primIdToMatIdList.push_back(ret.primIdToMatId);
    matIdToNameList.push_back(ret.matIdToName);
    // move material unique pointers
    materials.insert(materials.end(),
                     std::make_move_iterator(ret.materials.begin()),
                     std::make_move_iterator(ret.materials.end()));
  }

  // cleanup vbos
  for (size_t i = 0; i < vbos.size(); ++i)
    glDeleteBuffers(1, &vbos[i]);

  return {scene.name,
          meshes,
          names,
          hasTexCoords,
          primIdToMatIdList,
          matIdToNameList,
          std::move(materials)};
}

SceneLoader::ProcessMeshRet
SceneLoader::processMesh(const std::map<int, GLuint> &vbos,
                         const tinygltf::Mesh &meshData,
                         const tinygltf::Model &modelData) {
  std::vector<Primitive> primitives;
  std::vector<std::unique_ptr<BaseMaterial>> materials;
  std::vector<std::string> materialNames;

  std::map<PrimitiveId, MaterialId> primIdToMatId;
  std::map<MaterialId, std::string> matIdToName;
  bool hasTexCoords = false;
  bool success = true;
  std::string message; // contains reason if sucess == false

  // loop through mesh primitives
  for (size_t i = 0; i < meshData.primitives.size(); ++i) {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const tinygltf::Primitive &primitive = meshData.primitives[i];
    // primitive attributes (Position, Normal, TexCoords)
    for (const auto &attrib : primitive.attributes) {
      // attibe pair<string, int> pair of attribute name and accessor index
      const tinygltf::Accessor &accessor = modelData.accessors[attrib.second];
      // bind respective accessor buffer view
      glBindBuffer(GL_ARRAY_BUFFER, vbos.at(accessor.bufferView));

      int size = 1;
      /*
       * Accessor type represents bufferView data type (VEC2, VEC3,...)
       * Except for SCALAR type id is same as type size
       */
      if (accessor.type != TINYGLTF_TYPE_SCALAR)
        size = accessor.type;

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0)
        vaa = shader::vertex::attribute::POSITION_LOC;
      if (attrib.first.compare("NORMAL") == 0)
        vaa = shader::vertex::attribute::NORMAL_LOC;
      if (attrib.first.compare("TEXCOORD_0") == 0) {
        vaa = shader::vertex::attribute::TEXCOORD0_LOC;
        hasTexCoords = true;
      } else if (hasTexCoords) {
        message = "Invalid mesh data, some contain texcoords and some don't.";
        success = false;
      }

      if (vaa > -1) {
        // calculate stride
        int byteStride =
            accessor.ByteStride(modelData.bufferViews[accessor.bufferView]);
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, (void *)accessor.byteOffset);
      }
    }

    // primitive materials
    int matIndex = primitive.material;
    if (matIndex != -1) {
      const tinygltf::Material &materialData =
          modelData.materials[primitive.material];
      // process material
      auto processMatRet = processMaterial(materialData, modelData);
      materials.emplace_back(
          std::unique_ptr<BaseMaterial>(processMatRet.material.release()));
      materialNames.emplace_back(processMatRet.name);
      // primitive to material map
      primIdToMatId[vao] = materials.back()->id;
    } else {
      // if material for a primitive doesn't exists set default mat
      if (hasTexCoords) {
        primIdToMatId[vao] = DEFAULT_MATERIAL_ID;
      } else {
        primIdToMatId[vao] = DEFAULT_FLAT_MATERIAL_ID;
      }
    }

    // primitive indices
    const tinygltf::Accessor &indexAccessor =
        modelData.accessors[primitive.indices];

    assert((indexAccessor.componentType == GL_UNSIGNED_INT ||
            indexAccessor.componentType == GL_UNSIGNED_SHORT) &&
           "Invalid mesh index type.");

    GLuint ibo = vbos.at(indexAccessor.bufferView);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // make sure we have valid data
    assert(vao && "Invalid index buffer.");
    assert(primitive.mode >= GL_POINTS && primitive.mode <= GL_TRIANGLE_FAN &&
           "Invalid primitive mode.");
    assert(indexAccessor.componentType >= GL_SHORT &&
           indexAccessor.componentType <= GL_FLOAT);
    assert(indexAccessor.count > 0 && "Count must be greater than 0.");

    // register primitives to our mesh
    primitives.push_back({vao, (const GLenum)primitive.mode,
                          (const GLenum)indexAccessor.componentType,
                          (const GLsizei)indexAccessor.count,
                          (void *)indexAccessor.byteOffset});
    glBindVertexArray(0);
  }
  if (loadedMeshCount == UINT_MAX) {
    success = false;
    message = "SceneLoader maximum mesh count reached.";
  }
  if (success)
    return {{loadedMeshCount++, primitives},
            std::move(materials),
            materialNames,
            primIdToMatId,
            matIdToName,
            hasTexCoords,
            success,
            message};
  else
    return {{0, {}}, {}, {}, {}, {}, false, false, message};
}

SceneLoader::ProcessMaterialRet
SceneLoader::processMaterial(const tinygltf::Material &materialData,
                             const tinygltf::Model &modelData) {
  assert(loadedMeshCount != UINT_MAX && "Out of model ids.");
  const RenderDefaults &renderDefault = RenderDefaults::getInstance();
  std::unique_ptr<FlatMaterial> flatMaterial = nullptr;
  const tinygltf::PbrMetallicRoughness &pbrInfo =
      materialData.pbrMetallicRoughness;

  // material texture indices
  int baseColorTextureIndex = pbrInfo.baseColorTexture.index;
  int normalTextureIndex = materialData.normalTexture.index;
  int emissionTextureIndex = materialData.emissiveTexture.index;
  int occlusionTextureIndex = materialData.occlusionTexture.index;
  int metallicRoughnessTextureIndex = pbrInfo.metallicRoughnessTexture.index;

  // material texture id
  GLuint albedo = 0;
  GLuint normal = 0;
  GLuint emission = 0;
  GLuint metallicRoughness = 0;
  GLuint ao = 0;

  /**
   * load material texture if they exists, otherwise fallback to color or
   * default texture
   */
  if (baseColorTextureIndex != -1) {
    const tinygltf::Image &baseColorImage =
        modelData.images[modelData.textures[baseColorTextureIndex].source];
    albedo = processTexture(baseColorImage);
  } else {
    const auto albedo =
        glm::vec4(pbrInfo.baseColorFactor[0], pbrInfo.baseColorFactor[1],
                  pbrInfo.baseColorFactor[2], pbrInfo.baseColorFactor[3]);
    const auto emission = glm::vec3(materialData.emissiveFactor[0],
                                    materialData.emissiveFactor[1],
                                    materialData.emissiveFactor[2]);
    const auto ao = 1.0f;
    const auto metallic = (float)pbrInfo.metallicFactor;
    const auto roughtness = (float)pbrInfo.roughnessFactor;
    flatMaterial.reset(new FlatMaterial(
        {{loadedMaterialCount++, ShaderType::FLAT_FORWARD_SHADER},
         albedo,
         emission,
         ao,
         metallic,
         roughtness}));
  }
  if (normalTextureIndex != -1) {
    const tinygltf::Image &normalImage =
        modelData.images[modelData.textures[normalTextureIndex].source];
    normal = processTexture(normalImage);
  } else
    normal = renderDefault.getBlackTexture();
  if (emissionTextureIndex != -1) {
    const tinygltf::Image &emissionImage =
        modelData.images[modelData.textures[emissionTextureIndex].source];
    emission = processTexture(emissionImage);
  } else
    emission = renderDefault.getBlackTexture();
  if (metallicRoughnessTextureIndex != -1) {
    const tinygltf::Image &metallicRoughnessImage =
        modelData
            .images[modelData.textures[metallicRoughnessTextureIndex].source];
    metallicRoughness = processTexture(metallicRoughnessImage);
  } else
    metallicRoughness = renderDefault.getBlackTexture();
  if (occlusionTextureIndex != -1) {
    const tinygltf::Image &occlusionImage =
        modelData.images[modelData.textures[occlusionTextureIndex].source];
    ao = processTexture(occlusionImage);
  } else
    ao = renderDefault.getBlackTexture();

  if (flatMaterial)
    return {materialData.name,
            std::unique_ptr<BaseMaterial>(flatMaterial.release())};
  else
    return {materialData.name,
            std::unique_ptr<Material>(new Material(
                {{loadedMaterialCount++, ShaderType::FORWARD_SHADER},
                 albedo,
                 metallicRoughness,
                 ao,
                 normal,
                 emission}))};
}

GLuint SceneLoader::processTexture(const tinygltf::Image &image) {
  GLuint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLenum format = GL_RGBA;
  if (image.component == 1)
    format = GL_RED;
  else if (image.component == 2)
    format = GL_RG;
  else if (image.component == 3)
    format = GL_RGB;

  GLenum type = GL_UNSIGNED_BYTE;
  if (image.bits == 9)
    ;
  else if (image.bits == 16)
    type = GL_UNSIGNED_SHORT;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, format,
               type, &image.image.at(0));
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texId;
}
} // namespace render_system
