#include <array>
#include <chrono>

#include "core/Core.h"
#include "core/Log.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "stb_image.h"
#include "vulkan/vulkan.h"

#include "examples/texturedcube/gen.cc/Main.h"

struct Cube {
  const std::array<glm::fvec3, 36> position;
  const std::array<glm::fvec2, 36> texCoord;
  glm::fmat4 model;
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
    {
        // left face
        glm::fvec2(1.f, 0.f), // lft-top-front
        glm::fvec2(0.f, 1.f), // lft-btm-back
        glm::fvec2(0.f, 0.f), // lft-top-back
        glm::fvec2(0.f, 1.f), // lft-btm-back
        glm::fvec2(1.f, 0.f), // lft-top-front
        glm::fvec2(1.f, 1.f), // lft-btm-front

        // front face
        glm::fvec2(0.f, 0.f), // lft-top-front
        glm::fvec2(1.f, 0.f), // rgt-top-front
        glm::fvec2(1.f, 1.f), // rgt-btm-front
        glm::fvec2(0.f, 0.f), // lft-top-front
        glm::fvec2(1.f, 1.f), // rgt-btm-front
        glm::fvec2(0.f, 1.f), // lft-btm-front

        // top face
        glm::fvec2(0.f, 1.f), // lft-top-front
        glm::fvec2(1.f, 0.f), // rgt-top-back
        glm::fvec2(1.f, 1.f), // rgt-top-front
        glm::fvec2(0.f, 1.f), // lft-top-front
        glm::fvec2(0.f, 0.f), // lft-top-back
        glm::fvec2(1.f, 0.f), // rgt-top-back

        // bottom face
        glm::fvec2(0.f, 0.f), // lft-btm-front
        glm::fvec2(1.f, 1.f), // rgt-btm-back
        glm::fvec2(0.f, 1.f), // lft-btm-back
        glm::fvec2(0.f, 0.f), // lft-btm-front
        glm::fvec2(1.f, 0.f), // rgt-btm-front
        glm::fvec2(1.f, 1.f), // rgt-btm-back

        // right face
        glm::fvec2(0.f, 1.f), // rgt-btm-front
        glm::fvec2(1.f, 0.f), // rgt-top-back
        glm::fvec2(1.f, 1.f), // rgt-btm-back
        glm::fvec2(1.f, 0.f), // rgt-top-back
        glm::fvec2(0.f, 1.f), // rgt-btm-front
        glm::fvec2(0.f, 0.f), // rgt-top-front

        // back face
        glm::fvec2(1.f, 1.f), // lft-btm-back
        glm::fvec2(0.f, 1.f), // rgt-btm-back
        glm::fvec2(1.f, 0.f), // lft-top-back
        glm::fvec2(1.f, 0.f), // lft-top-back
        glm::fvec2(0.f, 1.f), // rgt-btm-back
        glm::fvec2(0.f, 0.f), // rgt-top-back
    },
    glm::fmat4(1.0),
};

template <> struct Draw_mvp<Cube> {
  void operator()(const Cube &c, uint32_t &uid, glm::fmat4 *data) const
      noexcept {
    uid = 0;
    if (data != nullptr) {
      glm::fmat4 proj =
          glm::perspective(glm::radians(45.0f), 800 / 600.0f, 0.1f, 100.0f);
      glm::fmat4 view = glm::lookAt(glm::fvec3(4, -4, -4), glm::fvec3(0, 0, 0),
                                    glm::fvec3(0, 1, 0));
      *data = proj * view * c.model;
    }
  }
};

template <> struct Draw_tex<Cube> {
  void operator()(const Cube &c, uint32_t &uid,
                  SampledImageReference *ref) const noexcept {
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
    ref->sampler_create_info.anisotropyEnable = VK_FALSE;
    ref->sampler_create_info.maxAnisotropy = 0.0f;
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
    ref->image_data = stbi_load("assets/zrl-logo.png", &width, &height,
                                &channels, STBI_rgb_alpha);
    CHECK_PC(ref->image_data != nullptr, "failed to load image");
    ref->width = static_cast<uint32_t>(width);
    ref->height = static_cast<uint32_t>(height);
    ref->channels = static_cast<uint32_t>(channels);
    ref->size = width * height * 4;
  }
};

template <> struct Draw_indices_<Cube> {
  void operator()(const Cube &, uint32_t &uid, void const **, VkIndexType &,
                  uint32_t &index_count) const noexcept {
    uid = 0;
    index_count = 0;
  }
};

template <> struct Draw_pos<Cube> {
  void operator()(const Cube &c, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 1;
    size = c.position.size() * sizeof(glm::fvec3);
    if (src != nullptr) {
      *src = c.position.data();
    }
  }
};

template <> struct Draw_uv<Cube> {
  void operator()(const Cube &c, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 2;
    size = c.texCoord.size() * sizeof(glm::fvec2);
    if (src != nullptr) {
      *src = c.texCoord.data();
    }
  }
};

int main() {
  const zrl::Config config{/* app_name */ "texturedcube",
                           /* engine_name */ "zrl",
                           /* width */ 800,
                           /* height */ 600,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  Main renderer(core);

  auto prev_time = std::chrono::system_clock::now();
  while (!glfwWindowShouldClose(core.GetWindow())) {
    auto cur_time = std::chrono::system_clock::now();
    std::chrono::duration<float> delta = cur_time - prev_time;
    prev_time = cur_time;
    glfwPollEvents();
    cube.model = glm::rotate(cube.model, glm::radians(20 * delta.count()),
                             glm::fvec3(0, 1, 0));
    renderer.Render(cube);
  }

  return 0;
}
