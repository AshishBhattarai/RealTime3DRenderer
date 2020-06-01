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

/**
 * These structs shouldn't be accessable from outside the render system
 */

struct BaseMaterial {
  const MaterialId id;
  const ShaderType shaderType;
};

/**
 * Flat materials contain color values only no textures
 */
struct FlatMaterial : BaseMaterial {
  const glm::vec4 albedo;
  const glm::vec3 emission;
  const float metallic;
  const float roughtness;
  const float ao;
};

/**
 * Normal texture based materials
 */
struct Material : BaseMaterial {
  const GLuint albedo;
  const GLuint metallicRoughness; // R - Metallic, G - Roughtness
  const GLuint ao;
  const GLuint normal;
  const GLuint emission;
  //  std::unique_ptr<FlatMaterial> flatMaterial;
};

/**
 * Primitive of a mesh(sub-mesh) It has a material and is rendered
 *
 * User constructor to create it validates
 */
struct Primitive {
  const GLuint
      vao; // can be used as primitive id(local id not global primitive id)
  const GLenum mode; // Render mode
  const GLenum indexType;
  const GLsizei indexCount;
  const GLvoid *indexOffset; // Index buffer offset;
  /* primitive_id VAO + SOME OFFEST ?*/
};

struct Mesh {
  const MeshId id;
  std::vector<Primitive> primitives;
};
} // namespace render_system
