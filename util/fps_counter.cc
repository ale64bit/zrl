#include "util/fps_counter.h"

#include <iomanip>
#include <sstream>

FpsCounter::FpsCounter(GLFWwindow *window,
                       std::chrono::duration<double> refresh_period)
    : window_(window), refresh_period_(refresh_period) {
  frame_count_ = 0;
  prev_ = std::chrono::system_clock::now();
}

void FpsCounter::Update() {
  ++frame_count_;
  auto cur = std::chrono::system_clock::now();
  std::chrono::duration<double> delta = cur - prev_;
  if (delta >= refresh_period_) {
    std::stringstream out;
    const double fps = frame_count_ / delta.count();
    const double ftime = 1000.0 * delta.count() / frame_count_;
    out << std::fixed << std::setprecision(2) << "FPS=" << fps
        << " frame_time=" << ftime << "ms";
    glfwSetWindowTitle(window_, out.str().c_str());
    frame_count_ = 0;
    prev_ = cur;
  }
}
