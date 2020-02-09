#pragma once

#include <glm/glm.hpp>

enum class CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  STRAFE_RIGHT,
  STRAFE_LEFT
};

namespace render_system {
/**
 * @brief The Camera class
 *
 * Simple camera class to generate view materix for renderer.
 */
class Camera {
private:
  // default settings
  static constexpr float YAW = 0.0f;
  static constexpr float PITCH = 0.0f;
  static constexpr float ROLL = 0.0f;
  static constexpr float SPEED = 2.5f;
  static constexpr float SENSITIVITY = 0.2f;
  static constexpr float ZOOM = 45.0f;

  glm::mat4 viewMat;
  glm::vec3 front;
  glm::vec3 right;
  glm::vec3 up;

public:
  glm::vec3 position;
  glm::vec3 rotation; // pitch, yaw, roll
  float movementSpeed;
  float mouseSensitivity;

  Camera(const glm::vec3 &position = glm::vec3(0.0f),
         const glm::vec3 &rotation = glm::vec3(PITCH, YAW, ROLL));

  void processMovement(CameraMovement movement, float dt);
  void processRotation(float xoffset, float yoffset);
  void update();

  const glm::mat4 &getViewMatrix() const { return viewMat; }
};
} // namespace render_system
