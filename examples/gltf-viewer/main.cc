#include <string>
#include <vector>

#include "core/Core.h"
#include "core/Log.h"
#include "util/camera.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "tiny_gltf.h"
#include "vulkan/vulkan.h"

#include "examples/gltf-viewer/gen.cc/PBR.h"
#include "examples/gltf-viewer/gen.cc/Types.h"

#include "gltf_binders.h"

// TODO: modify the UIDs.

std::string base_dir;

struct Node {
  const tinygltf::Model &m;
  const glm::fmat4 model;
  const int mesh = 0;
  const int primitive = 0;
};

template <> struct ForwardPass_g<Global> {
  void operator()(const Global &g, uint32_t &uid, Global *data) const noexcept {
    uid = 0;
    if (data != nullptr) {
      *data = g;
    }
  }
};

template <> struct ForwardPass_env<std::string> {

  void SetSampler(VkSamplerCreateInfo &info) const {
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.minLod = 0.0f;
    info.maxLod = 0.0f;
    info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.anisotropyEnable = VK_TRUE;
    info.maxAnisotropy = 8.0f;
    info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;
    info.compareEnable = VK_FALSE;
    info.compareOp = VK_COMPARE_OP_ALWAYS;
    info.mipLodBias = 0.0f;
  }

  void LoadTexture(const std::string &filepath,
                   SampledImage2DReference &ref) const {
    int width, height, channels;
    ref.format = VK_FORMAT_R8G8B8A8_UNORM;
    ref.image_data.resize(1);
    ref.image_data[0].push_back(stbi_load(filepath.c_str(), &width, &height,
                                          &channels, STBI_rgb_alpha));
    CHECK_PC(ref.image_data[0][0] != nullptr,
             "failed to load image: " + filepath);
    ref.size = width * height * 4;
    ref.width = static_cast<uint32_t>(width);
    ref.height = static_cast<uint32_t>(height);
    ref.channels = static_cast<uint32_t>(channels);
    ref.build_mipmaps = false;

    LOG(INFO) << "main: loaded texture '" << filepath
              << "': width=" << ref.width << " height=" << ref.height
              << " channels=" << ref.channels << "\n";
  }

  bool LevelExist(const std::string &name, const std::string &kind,
                  int level) const {
    const std::string filename = "assets/textures/" + name + "/" + kind + "/" +
                                 kind + "_front_" + std::to_string(level) +
                                 ".hdr";
    if (FILE *f = fopen(filename.c_str(), "r")) {
      fclose(f);
      return true;
    }
    return false;
  }

  void LoadEnvCubeMaps(const std::string &name,
                       SampledCubeMapReference &diffuse,
                       SampledCubeMapReference &specular) const {
    constexpr std::array<const char *, 6> kFaceNames = {
        "front", "back", "top", "bottom", "left", "right",
    };

    // Diffuse environment map.
    diffuse.format = VK_FORMAT_R8G8B8A8_UNORM;
    int mip_levels = 1;
    while (LevelExist(name, "diffuse", mip_levels)) {
      ++mip_levels;
    }
    for (int layer = 0; layer < 6; ++layer) {
      for (int level = 0; level < mip_levels; ++level) {
        int width, height, channels;
        const std::string filename = "assets/textures/" + name +
                                     "/diffuse/diffuse_" + kFaceNames[layer] +
                                     "_" + std::to_string(level) + ".hdr";
        diffuse.image_data[layer].push_back(stbi_load(
            filename.c_str(), &width, &height, &channels, STBI_rgb_alpha));
        CHECK_PC(diffuse.image_data[layer][level] != nullptr,
                 "failed to load image: " + filename);
        if (layer == 0 && level == 0) {
          diffuse.width = static_cast<uint32_t>(width);
          diffuse.height = static_cast<uint32_t>(height);
          diffuse.channels = static_cast<uint32_t>(channels);
        }
      }
    }
    diffuse.size = diffuse.width * diffuse.height * 4;
    LOG(INFO) << "main: loaded diffuse env cubemap '" << name
              << "': width=" << diffuse.width << " height=" << diffuse.height
              << " channels=" << diffuse.channels << " levels=" << mip_levels
              << "\n";

    // Specular environment map.
    specular.format = VK_FORMAT_R8G8B8A8_UNORM;
    mip_levels = 1;
    while (LevelExist(name, "specular", mip_levels)) {
      ++mip_levels;
    }
    for (int layer = 0; layer < 6; ++layer) {
      for (int level = 0; level < mip_levels; ++level) {
        int width, height, channels;
        const std::string filename = "assets/textures/" + name +
                                     "/specular/specular_" + kFaceNames[layer] +
                                     "_" + std::to_string(level) + ".hdr";
        specular.image_data[layer].push_back(stbi_load(
            filename.c_str(), &width, &height, &channels, STBI_rgb_alpha));
        CHECK_PC(specular.image_data[layer][level] != nullptr,
                 "failed to load image: " + filename);
        if (layer == 0 && level == 0) {
          specular.width = static_cast<uint32_t>(width);
          specular.height = static_cast<uint32_t>(height);
          specular.channels = static_cast<uint32_t>(channels);
        }
      }
    }
    specular.size = specular.width * specular.height * 4;
    LOG(INFO) << "main: loaded specular env cubemap '" << name
              << "': width=" << specular.width << " height=" << specular.height
              << " channels=" << specular.channels << " levels=" << mip_levels
              << "\n";
  }

  void operator()(const std::string &name, uint32_t &uid,
                  Environment *data) const noexcept {
    uid = 1 << 30;
    if (data != nullptr) {
      SetSampler(data->diffuse.sampler_create_info);
      SetSampler(data->specular.sampler_create_info);
      LoadEnvCubeMaps(name, data->diffuse, data->specular);
      SetSampler(data->brdfLUT.sampler_create_info);
      LoadTexture("assets/textures/brdfLUT.png", data->brdfLUT);
    }
  }
};

template <> struct ForwardPass_mat<Node> {
  void operator()(const Node &node, uint32_t &uid, Material *m) const noexcept {
    const tinygltf::Primitive &p =
        node.m.meshes[node.mesh].primitives[node.primitive];
    if (p.material == -1) {
      return;
    }
    uid = (1 << 20) + p.material;
    if (m == nullptr) {
      return;
    }
    const tinygltf::Material &mat = node.m.materials[p.material];
    m->baseColorFactor =
        glm::make_vec4(mat.pbrMetallicRoughness.baseColorFactor.data());
    if (mat.pbrMetallicRoughness.baseColorTexture.index != -1) {
      CHECK_PC(mat.pbrMetallicRoughness.baseColorTexture.texCoord == 0,
               "Only one texture coordinate set is supported");
      m->hasBaseColor = true;
      zrl::support::gltf::LoadTexture(
          node.m, base_dir,
          node.m.textures[mat.pbrMetallicRoughness.baseColorTexture.index],
          m->baseColor);
    }
    m->metallicFactor = mat.pbrMetallicRoughness.metallicFactor;
    m->roughnessFactor = mat.pbrMetallicRoughness.roughnessFactor;
    if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
      CHECK_PC(mat.pbrMetallicRoughness.metallicRoughnessTexture.texCoord == 0,
               "Only one texture coordinate set is supported");
      m->hasMetallicRoughness = true;
      zrl::support::gltf::LoadTexture(
          node.m, base_dir,
          node.m.textures[mat.pbrMetallicRoughness.metallicRoughnessTexture
                              .index],
          m->metallicRoughness);
    }
    if (mat.normalTexture.index != -1) {
      CHECK_PC(mat.normalTexture.texCoord == 0,
               "Only one texture coordinate set is supported");
      m->hasNormal = true;
      m->normalScale = mat.normalTexture.scale;
      zrl::support::gltf::LoadTexture(node.m, base_dir,
                                      node.m.textures[mat.normalTexture.index],
                                      m->normal);
    }
    if (mat.occlusionTexture.index != -1) {
      CHECK_PC(mat.occlusionTexture.texCoord == 0,
               "Only one texture coordinate set is supported");
      m->hasOcclusion = true;
      m->occlusionStrength = mat.occlusionTexture.strength;
      zrl::support::gltf::LoadTexture(
          node.m, base_dir, node.m.textures[mat.occlusionTexture.index],
          m->occlusion);
    }
    if (mat.emissiveTexture.index != -1) {
      CHECK_PC(mat.emissiveTexture.texCoord == 0,
               "Only one texture coordinate set is supported");
      m->hasEmissive = true;
      m->emissiveFactor = glm::make_vec3(mat.emissiveFactor.data());
      zrl::support::gltf::LoadTexture(
          node.m, base_dir, node.m.textures[mat.emissiveTexture.index],
          m->emissive);
    }
    if (mat.alphaMode == "OPAQUE") {
      m->alphaMode = 0;
    } else if (mat.alphaMode == "MASK") {
      m->alphaMode = 1;
    } else if (mat.alphaMode == "BLEND") {
      m->alphaMode = 2;
    }
    m->alphaCutoff = mat.alphaCutoff;
  }
};

template <> struct ForwardPass_obj<Node> {
  void operator()(const Node &node, uint32_t &uid, PerObject *data) const
      noexcept {
    uid = 0;
    if (data != nullptr) {
      const tinygltf::Primitive &p =
          node.m.meshes[node.mesh].primitives[node.primitive];
      data->model = node.model;
      data->normalMat = glm::transpose(glm::affineInverse(node.model));
      data->hasColor = p.attributes.count("COLOR") != 0;
      data->hasNormal = p.attributes.count("NORMAL") != 0;
      data->hasTangent = p.attributes.count("TANGENT") != 0;
    }
  }
};

template <> struct ForwardPass_indices_<Node> {
  void operator()(const Node &node, uint32_t &uid, void const **src,
                  VkIndexType &index_type, uint32_t &index_count) const
      noexcept {
    const tinygltf::Primitive &p =
        node.m.meshes[node.mesh].primitives[node.primitive];
    uid = (node.mesh << 20) + (node.primitive << 10) + 1;
    if (src != nullptr) {
      const tinygltf::Accessor &accessor = node.m.accessors[p.indices];
      const tinygltf::BufferView &bufferView =
          node.m.bufferViews[accessor.bufferView];
      const tinygltf::Buffer &buffer = node.m.buffers[bufferView.buffer];
      CHECK_PC(bufferView.byteStride == 0, "strided buffers are not supported");
      switch (accessor.componentType) {
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        index_type = VK_INDEX_TYPE_UINT16;
        break;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        index_type = VK_INDEX_TYPE_UINT32;
        break;
      }
      index_count = accessor.count;
      *src = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
    }
  }
};

template <> struct ForwardPass_position<Node> {
  void operator()(const Node &node, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = (node.mesh << 20) + (node.primitive << 10) + 2;
    zrl::support::gltf::Attribute<TINYGLTF_COMPONENT_TYPE_FLOAT,
                                  TINYGLTF_TYPE_VEC3>(
        node.m, node.mesh, node.primitive, "POSITION", size, src);
  }
};

template <> struct ForwardPass_normal<Node> {
  void operator()(const Node &node, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 0;
    if (zrl::support::gltf::OptionalAttribute<TINYGLTF_COMPONENT_TYPE_FLOAT,
                                              TINYGLTF_TYPE_VEC3>(
            node.m, node.mesh, node.primitive, "NORMAL", size, src)) {
      uid = (node.mesh << 20) + (node.primitive << 10) + 3;
    }
  }
};

template <> struct ForwardPass_color<Node> {
  void operator()(const Node &node, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 0;
    if (zrl::support::gltf::OptionalAttribute<TINYGLTF_COMPONENT_TYPE_FLOAT,
                                              TINYGLTF_TYPE_VEC3>(
            node.m, node.mesh, node.primitive, "COLOR", size, src)) {
      uid = (node.mesh << 20) + (node.primitive << 10) + 4;
    }
  }
};

template <> struct ForwardPass_tangent<Node> {
  void operator()(const Node &node, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = 0;
    if (zrl::support::gltf::OptionalAttribute<TINYGLTF_COMPONENT_TYPE_FLOAT,
                                              TINYGLTF_TYPE_VEC4>(
            node.m, node.mesh, node.primitive, "TANGENT", size, src)) {
      uid = (node.mesh << 20) + (node.primitive << 10) + 5;
    }
  }
};

template <> struct ForwardPass_uv<Node> {
  void operator()(const Node &node, uint32_t &uid, void const **src,
                  VkDeviceSize &size) const noexcept {
    uid = (node.mesh << 20) + (node.primitive << 10) + 6;
    zrl::support::gltf::Attribute<TINYGLTF_COMPONENT_TYPE_FLOAT,
                                  TINYGLTF_TYPE_VEC2>(
        node.m, node.mesh, node.primitive, "TEXCOORD_0", size, src);
  }
};

void AddNodes(const tinygltf::Model &model, const tinygltf::Node &node,
              glm::fmat4 transform, std::vector<Node> &nodes) {
  if (node.matrix.size() == 16) {
    transform *= glm::fmat4(glm::make_mat4(node.matrix.data()));
  } else {
    if (node.scale.size() == 3) {
      transform = glm::scale(transform,
                             glm::fvec3(glm::make_vec3((node.scale.data()))));
    }
    if (node.rotation.size() == 4) {
      transform =
          glm::mat4_cast(glm::fquat(glm::make_quat(node.rotation.data()))) *
          transform;
    }
    if (node.translation.size() == 3) {
      transform = glm::translate(
          transform, glm::fvec3(glm::make_vec3((node.translation.data()))));
    }
  }
  if (node.mesh != -1) {
    for (size_t j = 0; j < model.meshes[node.mesh].primitives.size(); ++j) {
      nodes.push_back({
          model,
          transform,
          node.mesh,
          static_cast<int>(j),
      });
    }
  }
  for (int child : node.children) {
    AddNodes(model, model.nodes[child], transform, nodes);
  }
}

std::tuple<double, double, double> HandleInput(GLFWwindow *window, int &mode) {
  static double last_x = 0, last_y = 0;

  glfwPollEvents();
  double cur_x, cur_y;
  glfwGetCursorPos(window, &cur_x, &cur_y);
  const double dx = (cur_x - last_x) * 0.1f;
  const double dy = (cur_y - last_y) * 0.1f;
  last_x = cur_x;
  last_y = cur_y;

  if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
    mode = 0;
  } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    mode = 1;
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    mode = 2;
  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
    mode = 3;
  } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
    mode = 4;
  } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
    mode = 5;
  } else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
    mode = 6;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    return std::make_tuple(.0, .0, dy);
  }
  return std::make_tuple(dx, dy, .0);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    LOG(ERROR) << "usage: gltf_viewer <gltf_file>\n";
    return -1;
  }
  LOG(INFO) << "main: loading input model\n";
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, argv[1]);
  if (!warn.empty()) {
    LOG(WARNING) << warn << "\n";
  }
  if (!err.empty()) {
    LOG(ERROR) << err << "\n";
  }
  if (!ret) {
    LOG(ERROR) << "failed to parse glTF\n";
    return -1;
  }
  base_dir = tinygltf::GetBaseDir(argv[1]);

  LOG(INFO) << "main: creating renderer\n";
  const zrl::Config config{/* app_name */ "gltf-viewer",
                           /* engine_name */ "zrl",
                           /* width */ 1920,
                           /* height */ 1080,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  PBR renderer(core);
  Global global;
  Camera camera(30.0f);
  std::vector<Node> nodes;

  // global.lights[0].direction = glm::fvec3(0.0, -1.0, 0.1);
  // global.lights[0].range = -1;
  // global.lights[0].color = glm::fvec3(1, 1, 1);
  // global.lights[0].intensity = 0.8;
  // global.lights[0].position = glm::fvec3(4, 0, 4);
  // global.lights[0].innerConeCos = 0;
  // global.lights[0].outerConeCos = M_PI / 4;
  // global.lights[0].lightType = 0; // directional

  // global.lights[1].direction = glm::fvec3(0.0, 1.0, -1.1);
  // global.lights[1].range = -1;
  // global.lights[1].color = glm::fvec3(1, 1, 1);
  // global.lights[1].intensity = 0.8;
  // global.lights[1].position = glm::fvec3(4, 0, 4);
  // global.lights[1].innerConeCos = 0;
  // global.lights[1].outerConeCos = M_PI / 4;
  // global.lights[1].lightType = 0; // directional

  // global.lights[2].direction = glm::fvec3(1.0, -1.0, 0.1);
  // global.lights[2].range = -1;
  // global.lights[2].color = glm::fvec3(1, 1, 1);
  // global.lights[2].intensity = 0.8;
  // global.lights[2].position = glm::fvec3(4, 4, 0);
  // global.lights[2].innerConeCos = 0;
  // global.lights[2].outerConeCos = M_PI / 4;
  // global.lights[2].lightType = 0; // directional

  // global.lights[3].direction = glm::fvec3(1.0, 1.0, 1.1);
  // global.lights[3].range = -1;
  // global.lights[3].color = glm::fvec3(1, 1, 1);
  // global.lights[3].intensity = 0.8;
  // global.lights[3].position = glm::fvec3(0, 4, 4);
  // global.lights[3].innerConeCos = 0;
  // global.lights[3].outerConeCos = M_PI / 4;
  // global.lights[3].lightType = 0; // directional

  for (int i : model.scenes[model.defaultScene].nodes) {
    AddNodes(model, model.nodes[i], glm::fmat4(1.0), nodes);
  }

  const std::string env_name = "studio_grey";

  glfwSetInputMode(core.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  while (!glfwWindowShouldClose(core.GetWindow())) {
    double dx, dy, dz;
    std::tie(dx, dy, dz) = HandleInput(core.GetWindow(), global.debugMode);
    camera.Update(dx, dy, dz);

    glm::fmat4 proj = glm::perspective(
        glm::radians(63.0f), core.GetSwapchain().GetAspect(), 0.1f, 100.0f);
    glm::fmat4 view = camera.View();
    global.projView = proj * view;
    global.cameraPosition = glm::fvec4(camera.Eye(), 1.0);

    renderer.Render(global, env_name, nodes);
  }
  return 0;
}
