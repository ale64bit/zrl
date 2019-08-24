#include "util/camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

#include <algorithm>

Camera::Camera(float dist) : dist_(dist), target_(glm::fvec3(0)) {}

void Camera::Update(float dx, float dy, float dz) {
  x_ -= dx;
  y_ = std::max(std::min(y_ - dy, 89.f), -89.f);
  dist_ = std::max(dist_ + dz, .0f);
}

glm::fvec3 Camera::Eye() const {
  glm::fmat3 rot =
      glm::toMat3(glm::quat(glm::fvec3(glm::radians(y_), glm::radians(x_), 0)));
  return rot * glm::fvec3(0, 0, dist_);
}

glm::fmat4 Camera::View() const {
  return glm::lookAt(Eye(), target_, glm::fvec3(0, -1, 0));
}
