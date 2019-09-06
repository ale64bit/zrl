// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
