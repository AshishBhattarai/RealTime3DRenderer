#pragma once

#include "shader_config.h"
#include <cassert>
#include <glad/glad.h>
#include <memory>
#include <string>
#include <vector>

namespace render_system {

struct BaseMaterial {
  ShaderType shaderType;
  GLuint normalMap;
  GLuint emissionMap;
};

/**
 * Normal intensity base material
 */
struct Material : BaseMaterial {
  GLuint diffuseMap;
  GLuint specularMap;
};

struct PbrMatrial : BaseMaterial {
  // TODO: Implement PbrMaterial
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
  const GLvoid *indexOffset; // Index buffer offset;

  Primitive(std::unique_ptr<BaseMaterial> material, GLenum mode, GLsizei count,
            const GLvoid *indexOffset)
      : material(std::move(material)), mode(mode), count(count),
        indexOffset(indexOffset) {
    assert(material != nullptr && "Material must be valid.");
    assert(mode >= GL_POINT && mode <= GL_TRIANGLE_FAN &&
           "Invalid primitive mode.");
    assert(count > 0 && "Count must be greater than 0.");
  }
};

struct Mesh {
  // name ust be unique
  std::string name;
  GLuint vao;
  std::vector<Primitive> primitives;

  bool isValid() { return vao && !primitives.empty(); }
};
} // namespace render_system
