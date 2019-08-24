#include <array>
#include <chrono>

#include "core/Core.h"
#include "core/Log.h"
#include "util/camera.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "stb_image.h"
#include "vulkan/vulkan.h"

#include "examples/skybox/gen.cc/Main.h"

struct Cube {
  const std::array<glm::fvec3, 36> position;
} cube{
    // From:
    // https://github.com/LunarG/VulkanSamples/blob/master/API-Samples/data/cube_data.h
    {
        // left face
        glm::fvec3(-1, -1, -1), // lft-top-front
        glm::fvec3(-1, 1, 1),   // lft-btm-back
        glm::fvec3(-1, -1, 1),  // lft-top-back
        glm::fvec3(-1, 1, 1),   // lft-btm-back
        glm::fvec3(-1, -1, -1), // lft-top-front
        glm::fvec3(-1, 1, -1),  // lft-btm-front

        // front face
        glm::fvec3(-1, -1, -1), // lft-top-front
        glm::fvec3(1, -1, -1),  // rgt-top-front
        glm::fvec3(1, 1, -1),   // rgt-btm-front
        glm::fvec3(-1, -1, -1), // lft-top-front
        glm::fvec3(1, 1, -1),   // rgt-btm-front
        glm::fvec3(-1, 1, -1),  // lft-btm-front

        // top face
        glm::fvec3(-1, -1, -1), // lft-top-front
        glm::fvec3(1, -1, 1),   // rgt-top-back
        glm::fvec3(1, -1, -1),  // rgt-top-front
        glm::fvec3(-1, -1, -1), // lft-top-front
        glm::fvec3(-1, -1, 1),  // lft-top-back
        glm::fvec3(1, -1, 1),   // rgt-top-back

        // bottom face
        glm::fvec3(-1, 1, -1), // lft-btm-front
        glm::fvec3(1, 1, 1),   // rgt-btm-back
        glm::fvec3(-1, 1, 1),  // lft-btm-back
        glm::fvec3(-1, 1, -1), // lft-btm-front
        glm::fvec3(1, 1, -1),  // rgt-btm-front
        glm::fvec3(1, 1, 1),   // rgt-btm-back

        // right face
        glm::fvec3(1, 1, -1),  // rgt-btm-front
        glm::fvec3(1, -1, 1),  // rgt-top-back
        glm::fvec3(1, 1, 1),   // rgt-btm-back
        glm::fvec3(1, -1, 1),  // rgt-top-back
        glm::fvec3(1, 1, -1),  // rgt-btm-front
        glm::fvec3(1, -1, -1), // rgt-top-front

        // back face
        glm::fvec3(-1, 1, 1),  // lft-btm-back
        glm::fvec3(1, 1, 1),   // rgt-btm-back
        glm::fvec3(-1, -1, 1), // lft-top-back
        glm::fvec3(-1, -1, 1), // lft-top-back
        glm::fvec3(1, 1, 1),   // rgt-btm-back
        glm::fvec3(1, -1, 1),  // rgt-top-back
    },
};

template <> struct Skybox_cubeMap<Cube> {
  void operator()(const Cube &, uint32_t &uid,
                  SampledCubeMapReference *ref) const noexcept {
    uid = 1;
    if (ref == nullptr) {
      return;
    }
    // Sampler
    ref->sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ref->sampler_create_info.pNext = nullptr;
    ref->sampler_create_info.flags = 0;
    ref->sampler_create_info.magFilter = VK_FILTER_LINEAR;
    ref->sampler_create_info.minFilter = VK_FILTER_LINEAR;
    ref->sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ref->sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ref->sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ref->sampler_create_info.anisotropyEnable = VK_TRUE;
    ref->sampler_create_info.maxAnisotropy = 8.0f;
    ref->sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    ref->sampler_create_info.unnormalizedCoordinates = VK_FALSE;
    ref->sampler_create_info.compareEnable = VK_FALSE;
    ref->sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    ref->sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    ref->sampler_create_info.mipLodBias = 0.0f;
    ref->sampler_create_info.minLod = 0.0f;
    ref->sampler_create_info.maxLod = 0.0f;
    // Image
    ref->format = VK_FORMAT_R8G8B8A8_UNORM;
    int width, height, channels;

    ref->image_data[0] = stbi_load("assets/sb_iceflow/iceflow_rt.tga", &width,
                                   &height, &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data[0] != nullptr, "failed to load image");
    ref->image_data[1] = stbi_load("assets/sb_iceflow/iceflow_lf.tga", &width,
                                   &height, &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data[1] != nullptr, "failed to load image");
    ref->image_data[2] = stbi_load("assets/sb_iceflow/iceflow_up.tga", &width,
                                   &height, &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data[2] != nullptr, "failed to load image");
    ref->image_data[3] = stbi_load("assets/sb_iceflow/iceflow_dn.tga", &width,
                                   &height, &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data[3] != nullptr, "failed to load image");
    ref->image_data[4] = stbi_load("assets/sb_iceflow/iceflow_bk.tga", &width,
                                   &height, &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data[4] != nullptr, "failed to load image");
    ref->image_data[5] = stbi_load("assets/sb_iceflow/iceflow_ft.tga", &width,
                                   &height, &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data[5] != nullptr, "failed to load image");

    ref->size = width * height * 4;
    ref->width = static_cast<uint32_t>(width);
    ref->height = static_cast<uint32_t>(height);
    ref->channels = static_cast<uint32_t>(channels);
  }
};

template <> struct Skybox_indices_<Cube> {
  void operator()(const Cube &, uint32_t &uid, void const **, VkIndexType &,
                  uint32_t &index_count) const noexcept {
    uid = 0;
    index_count = 0;
  }
};

template <> struct Skybox_pos<Cube> {
  void operator()(const Cube &c, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 1;
    size = c.position.size() * sizeof(glm::fvec3);
    if (src != nullptr) {
      *src = c.position.data();
    }
  }
};

std::tuple<double, double, double> HandleInput(GLFWwindow *window) {
  static double last_x = 0, last_y = 0;

  glfwPollEvents();
  double cur_x, cur_y;
  glfwGetCursorPos(window, &cur_x, &cur_y);
  const double dx = (cur_x - last_x) * 0.1f;
  const double dy = (cur_y - last_y) * 0.1f;
  last_x = cur_x;
  last_y = cur_y;
  return std::make_tuple(dx, dy, .0);
}

int main() {
  const zrl::Config config{/* app_name */ "skybox",
                           /* engine_name */ "zrl",
                           /* width */ 800,
                           /* height */ 600,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  Main renderer(core);
  glm::fmat4 proj =
      glm::perspective(glm::radians(63.0f), 800 / 600.0f, 0.1f, 100.0f);
  glm::fmat4 model = glm::fmat4(1.0);
  Camera camera(0.01f);

  auto prev_time = std::chrono::system_clock::now();
  while (!glfwWindowShouldClose(core.GetWindow())) {
    auto cur_time = std::chrono::system_clock::now();
    std::chrono::duration<float> delta = cur_time - prev_time;
    prev_time = cur_time;

    double dx, dy, dz;
    std::tie(dx, dy, dz) = HandleInput(core.GetWindow());
    camera.Update(dx, dy, dz);

    renderer.Render(proj, camera.View(), model, cube, cube);
  }

  return 0;
}
