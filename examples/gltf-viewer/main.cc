#include <string>
#include <vector>

#include "core/Core.h"
#include "core/Log.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "tiny_gltf.h"
#include "vulkan/vulkan.h"

#include "examples/gltf-viewer/gen.cc/PBR.h"
#include "examples/gltf-viewer/gen.cc/Types.h"

#include "gltf_binders.h"

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
    uid = (node.mesh << 20) + (node.primitive << 10) + 3;
    zrl::support::gltf::Attribute<TINYGLTF_COMPONENT_TYPE_FLOAT,
                                  TINYGLTF_TYPE_VEC3>(
        node.m, node.mesh, node.primitive, "NORMAL", size, src);
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
                           /* width */ 800,
                           /* height */ 600,
                           /* fullscreen*/ false,
                           /* debug*/ true};
  zrl::Core core(config);
  PBR renderer(core);
  Global global;
  std::vector<Node> nodes;

  global.light.direction = glm::fvec3(0.0, -1.0, 0.0);
  global.light.range = -1;
  global.light.color = glm::fvec3(1, 1, 1);
  global.light.intensity = 2;
  global.light.position = glm::fvec3(0, 0, 0);
  global.light.innerConeCos = 0;
  global.light.outerConeCos = M_PI / 4;
  global.light.lightType = 0; // directional

  for (int i : model.scenes[model.defaultScene].nodes) {
    AddNodes(model, model.nodes[i], glm::fmat4(1.0), nodes);
  }
  DLOG << "node count: " << nodes.size() << "\n";

  float K = 30.0f;
  while (!glfwWindowShouldClose(core.GetWindow())) {
    glfwPollEvents();
    if (glfwGetKey(core.GetWindow(), GLFW_KEY_A) == GLFW_PRESS) {
      K -= 0.05f;
    }
    if (glfwGetKey(core.GetWindow(), GLFW_KEY_S) == GLFW_PRESS) {
      K += 0.05f;
    }

    glm::fmat4 proj =
        glm::perspective(glm::radians(63.0f), 800 / 600.0f, 0.1f, 100.0f);
    glm::fmat4 view = glm::lookAt(glm::fvec3(K, -K, -K), glm::fvec3(0, 0, 0),
                                  glm::fvec3(0, -1, 0));
    global.projView = proj * view;
    global.cameraPosition = glm::fvec4(K, -K, -K, 1);
    renderer.Render(global, nodes);
  }
  return 0;
}
