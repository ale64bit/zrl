/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
