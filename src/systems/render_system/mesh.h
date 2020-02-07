#pragma once

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
  glm::vec4 emission;
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
  GLenum mode; // Render mode
  GLsizei count;
  const GLvoid *offset; // Index buffer offset;

  Primitive(std::unique_ptr<BaseMaterial> material, GLenum mode, GLsizei count,
            const GLvoid *offset)
      : material(std::move(material)), mode(mode), count(count),
        offset(offset) {
    assert(material != nullptr && "Material must be valid.");
    assert(mode >= GL_POINT && mode <= GL_TRIANGLE_FAN &&
           "Invalid primitive mode.");
    assert(count > 0 && "Count must be greater than 0.");
  }
};

struct Mesh {
  // name must be unique
  std::string name;
  GLuint vao;
  std::vector<Primitive> primitives;

  bool isValid() { return vao && !primitives.empty(); }

  Mesh() = default;
  Mesh(Mesh &&mesh) noexcept = default;
  Mesh &operator=(Mesh &&) noexcept = default;
  Mesh(const Mesh &) = delete;
  const Mesh &operator=(const Mesh &) = delete;
};
} // namespace render_system
