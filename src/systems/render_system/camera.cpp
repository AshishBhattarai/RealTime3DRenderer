#include "camera.h"
#include <glm/gtc/quaternion.hpp>

Camera::Camera(const glm::vec3 &position, const glm::vec3 &rotation)
    : front(0.0f, 0.0f, 1.0f), right(1.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
      position(position), rotation(rotation) {}

void Camera::update() {
  // update view matrix and directional vectors
  glm::quat orientation =
      glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  orientation *=
      glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  orientation = glm::normalize(orientation);
  viewMat = glm::translate(glm::mat4_cast(orientation), -position);

  right = glm::vec3(viewMat[0][0], viewMat[1][0], viewMat[2][0]);
  up = glm::vec3(viewMat[0][1], viewMat[1][1], viewMat[2][1]);
  front = glm::vec3(viewMat[0][2], viewMat[1][2], viewMat[2][2]);
}

void Camera::processMovement(CameraMovement movement, float dt) {
  float speed = movementSpeed * dt;
  glm::vec3 direction = glm::vec3(0.0f);

  switch (movement) {
  case CameraMovement::FORWARD:
    direction = front;
    break;
  case CameraMovement::BACKWARD:
    direction = -front;
    break;
  case CameraMovement::RIGHT:
    direction = right;
    break;
  case CameraMovement::LEFT:
    direction = -right;
    break;
  case CameraMovement::STRAFE_RIGHT:
    direction = front + right;
    break;
  case CameraMovement::STRAFE_LEFT:
    direction = front - right;
    break;
  }
  position += glm::normalize(direction) * speed;
}

void Camera::processRotation(float xoffset, float yoffset) {
  rotation.y += xoffset * mouseSensitivity;
  rotation.x += yoffset * mouseSensitivity;

  // limit pitch to avoid unwanted rotation
  if (rotation.x > 89.0f)
    rotation.x = 89.0f;
  if (rotation.x < -89.0f)
    rotation.x = -89.0f;
}
