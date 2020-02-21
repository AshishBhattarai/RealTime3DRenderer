#pragma once

#include "common.h"
#include "shaders/config.h"
#include <cassert>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace render_system {

struct BaseMaterial {
  ShaderType shaderType;
};

/**
 * Flat materials contain color values only no textures
 */
struct FlatMaterial : BaseMaterial {
  glm::vec4 albedo;
  glm::vec3 emission;
  float metallic;
  float roughtness;
  float ao;
};

/**
 * Normal texture based materials
 */
struct Material : BaseMaterial {
  GLuint albedo;
  GLuint metallicRoughness; // R - Metallic, G - Roughtnessj
  GLuint ao;
  GLuint normal;
  GLuint emission;
  //  std::unique_ptr<FlatMaterial> flatMaterial;
};

/**
 * Primitive of a mesh(sub-mesh) It has a material and is rendered
 *
 * User constructor to create it validates
 */
struct Primitive {
  std::unique_ptr<BaseMaterial> material;
  GLuint vao;
  GLenum mode; // Render mode
  GLenum indexType;
  GLsizei indexCount;
  const GLvoid *indexOffset; // Index buffer offset;

  Primitive(std::unique_ptr<BaseMaterial> material, GLuint vao, GLenum mode,
            GLenum indexType, GLsizei indexCount, const GLvoid *indexOffset)
      : material(std::move(material)), vao(vao), mode(mode),
        indexType(indexType), indexCount(indexCount), indexOffset(indexOffset) {
    assert(this->material != nullptr && "Material must be valid.");
    assert(vao && "Invalid index buffer.");
    assert(mode >= GL_POINTS && mode <= GL_TRIANGLE_FAN &&
           "Invalid primitive mode.");
    assert(indexType >= GL_SHORT && indexType <= GL_FLOAT);
    assert(indexCount > 0 && "Count must be greater than 0.");
  }
};

struct Mesh {
  std::vector<Primitive> primitives;

  bool isValid() { return !primitives.empty(); }
  MeshId getId() {
    if (!isValid())
      return INVALID_MESH_ID;
    else
      return primitives.front().vao + MESH_ID_OFFSET;
  }

  Mesh() = default;
  Mesh(Mesh &&mesh) noexcept = default;
  Mesh &operator=(Mesh &&) noexcept = default;
  Mesh(const Mesh &) = delete;
  const Mesh &operator=(const Mesh &) = delete;
};
} // namespace render_system
