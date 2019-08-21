#ifndef CAMERA_H_
#define CAMERA_H_

#include "glm/glm.hpp"

class Camera {
public:
  Camera(float dist);
  void Update(float dx, float dy, float dz);
  glm::fvec3 Eye() const;
  glm::fmat4 View() const;

private:
  float x_;
  float y_;
  float dist_;
  glm::fvec3 target_;
};

#endif // CAMERA_H_
