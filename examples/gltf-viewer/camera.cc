#include "examples/gltf-viewer/camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

Camera::Camera(float dist) : dist_(dist), target_(glm::fvec3(0)) {}

void Camera::Update(float dx, float dy, float dz) {
  x_ -= dx * 0.01f;
  y_ -= dy * 0.01f;
  dist_ += dz * 0.1f;
}

glm::fvec3 Camera::Eye() const {
  glm::fmat3 rot = glm::toMat3(glm::quat(glm::fvec3(y_, x_, 0)));
  return rot * glm::fvec3(0, 0, dist_);
}

glm::fmat4 Camera::View() const {
  return glm::lookAt(Eye(), target_, glm::fvec3(0, -1, 0));
}
