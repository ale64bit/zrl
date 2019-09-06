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

#include <array>

#include "core/Core.h"
#include "core/Log.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

#include "examples/hellotriangle/gen.cc/Main.h"

struct Triangle {
  const std::array<glm::fvec3, 3> position;
  const std::array<glm::fvec3, 3> color;
} triangle = {
    {
        glm::fvec3(0.0, -1.0, 0.0),
        glm::fvec3(1.0, 1.0, 0.0),
        glm::fvec3(-1.0, 1.0, 0.0),
    },
    {
        glm::fvec3(1.0, 0.0, 0.0),
        glm::fvec3(0.0, 1.0, 0.0),
        glm::fvec3(0.0, 0.0, 1.0),
    },
};

template <> struct Draw_indices_<Triangle> {
  void operator()(const Triangle &, uint32_t &uid, void const **, VkIndexType &,
                  uint32_t &index_count) const noexcept {
    uid = 0;
    index_count = 0;
  }
};

template <> struct Draw_pos<Triangle> {
  void operator()(const Triangle &t, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 0;
    size = 3 * sizeof(glm::fvec3);
    if (src != nullptr) {
      *src = t.position.data();
    }
  }
};

template <> struct Draw_col<Triangle> {
  void operator()(const Triangle &t, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 0;
    size = 3 * sizeof(glm::fvec3);
    if (src != nullptr) {
      *src = t.color.data();
    }
  }
};

int main() {
  const zrl::Config config{/* app_name */ "hellotriangle",
                           /* engine_name */ "zrl",
                           /* width */ 800,
                           /* height */ 600,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  Main renderer(core);

  while (!glfwWindowShouldClose(core.GetWindow())) {
    glfwPollEvents();
    renderer.Render(triangle);
  }

  return 0;
}
