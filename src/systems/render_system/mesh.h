#pragma once

#include "common.h"
#include "shaders/config.h"
#include "texture.h"
#include <cassert>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

/**
 * Data structure for material & meshs loaded on the GPU.
 * App(module) should have its own representation of material & mesh for editor.
 */
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
  const Texture albedo;
  const Texture metallicRoughness; // R - Metallic, G - Roughtness
  const Texture ao;
  const Texture normal;
  const Texture emission;
  //  std::unique_ptr<FlatMaterial> flatMaterial;
};

/**
 * Primitive of a mesh(sub-mesh) It has a material and is rendered
 *
 * User constructor to create it validates
 */
struct Primitive {
  // can be used as primitive id(local id not global primitive id)
  const GLuint vao;
  const GLenum mode;         // Render mode GL_TRIANGLES ...
  const GLenum indexType;    // indices type GL_UNSIGNED_INT generally
  const GLsizei indexCount;  // indices count
  const GLvoid *indexOffset; // Index buffer offset;
};

struct Mesh {
  const MeshId id;
  std::vector<Primitive> primitives;
};
} // namespace render_system
