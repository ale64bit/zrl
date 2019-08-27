#ifndef FPS_COUNTER_H_
#define FPS_COUNTER_H_

#include <chrono>

#include "GLFW/glfw3.h"

class FpsCounter {
public:
  FpsCounter(GLFWwindow *window, std::chrono::duration<double> refresh_period);
  void Update();

private:
  GLFWwindow *window_;
  const std::chrono::duration<double> refresh_period_;
  std::chrono::system_clock::time_point prev_;
  int frame_count_;
};

#endif // FPS_COUNTER_H_
