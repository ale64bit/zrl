#ifndef GLTF_BINDERS_H_
#define GLTF_BINDERS_H_

namespace zrl {
namespace support {
namespace gltf {

template <int ComponentType> constexpr const char *ComponentTypeString() {
  switch (ComponentType) {
  case TINYGLTF_COMPONENT_TYPE_BYTE:
    return "BYTE";
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
    return "UNSIGNED_BYTE";
  case TINYGLTF_COMPONENT_TYPE_SHORT:
    return "SHORT";
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
    return "UNSIGNED_SHORT";
  case TINYGLTF_COMPONENT_TYPE_INT:
    return "INT";
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
    return "UNSIGNED_INT";
  case TINYGLTF_COMPONENT_TYPE_FLOAT:
    return "FLOAT";
  case TINYGLTF_COMPONENT_TYPE_DOUBLE:
    return "DOUBLE";
  default:
    return "UNKNOWN";
  }
}

template <int Type> constexpr const char *TypeString() {
  switch (Type) {
  case TINYGLTF_TYPE_VEC2:
    return "VEC2";
  case TINYGLTF_TYPE_VEC3:
    return "VEC3";
  case TINYGLTF_TYPE_VEC4:
    return "VEC4";
  case TINYGLTF_TYPE_MAT2:
    return "MAT2";
  case TINYGLTF_TYPE_MAT3:
    return "MAT3";
  case TINYGLTF_TYPE_MAT4:
    return "MAT4";
  case TINYGLTF_TYPE_SCALAR:
    return "SCALAR";
  case TINYGLTF_TYPE_VECTOR:
    return "VECTOR";
  case TINYGLTF_TYPE_MATRIX:
    return "MATRIX";
  default:
    return "UNKNOWN";
  }
}

template <int ComponentType, int Type>
void Attribute(const tinygltf::Model &model, int mesh, int primitive,
               const std::string &attribute, VkDeviceSize &size,
               void const **src) {
  const tinygltf::Primitive &p = model.meshes[mesh].primitives[primitive];
  CHECK_PC(p.attributes.count(attribute),
           "attribute '" + attribute + "' is required");
  const tinygltf::Accessor &accessor =
      model.accessors[p.attributes.at(attribute)];
  const tinygltf::BufferView &bufferView =
      model.bufferViews[accessor.bufferView];
  const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
  const size_t tightStride =
      tinygltf::GetComponentSizeInBytes(accessor.componentType) *
      tinygltf::GetTypeSizeInBytes(accessor.type);
  CHECK_PC(bufferView.byteStride == 0 || bufferView.byteStride == tightStride,
           "buffer views that are not tighly packed are not supported");
  CHECK_PC(!accessor.sparse.isSparse, "sparse accessors are not supported");
  CHECK_PC(accessor.componentType == ComponentType,
           "position accessor component type must be " +
               std::string(ComponentTypeString<ComponentType>()));
  CHECK_PC(accessor.type == Type,
           "position accessor type must be " + std::string(TypeString<Type>()));
  size = accessor.count * tinygltf::GetComponentSizeInBytes(ComponentType) *
         tinygltf::GetTypeSizeInBytes(Type);
  if (src != nullptr) {
    *src = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
  }
}

template <int ComponentType, int Type>
bool OptionalAttribute(const tinygltf::Model &model, int mesh, int primitive,
                       const std::string &attribute, VkDeviceSize &size,
                       void const **src) {
  size = 0;
  *src = nullptr;
  const tinygltf::Primitive &p = model.meshes[mesh].primitives[primitive];
  if (p.attributes.count(attribute) == 0) {
    return false;
  }
  Attribute<ComponentType, Type>(model, mesh, primitive, attribute, size, src);
  return true;
}

void LoadTexture(const tinygltf::Model &model, const std::string &base_dir,
                 const tinygltf::Texture &tex,
                 SampledImage2DReference &ref) noexcept {
  const tinygltf::Sampler sampler =
      (tex.sampler == -1) ? tinygltf::Sampler() : model.samplers[tex.sampler];
  const tinygltf::Image image = model.images[tex.source];
  // Sampler
  ref.sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  ref.sampler_create_info.pNext = nullptr;
  ref.sampler_create_info.flags = 0;
  switch (sampler.magFilter) {
  case TINYGLTF_TEXTURE_FILTER_NEAREST:
    ref.sampler_create_info.magFilter = VK_FILTER_NEAREST;
    break;
  case TINYGLTF_TEXTURE_FILTER_LINEAR:
    ref.sampler_create_info.magFilter = VK_FILTER_LINEAR;
    break;
  default:
    ref.sampler_create_info.magFilter = VK_FILTER_LINEAR;
    break;
  }
  ref.sampler_create_info.minLod = 0.0f;
  ref.sampler_create_info.maxLod = 0.0f;
  switch (sampler.minFilter) {
  case TINYGLTF_TEXTURE_FILTER_NEAREST:
    ref.sampler_create_info.minFilter = VK_FILTER_NEAREST;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    ref.sampler_create_info.maxLod = 0.25f;
    break;
  case TINYGLTF_TEXTURE_FILTER_LINEAR:
    ref.sampler_create_info.minFilter = VK_FILTER_LINEAR;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    ref.sampler_create_info.maxLod = 0.25f;
    break;
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
    ref.sampler_create_info.minFilter = VK_FILTER_NEAREST;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    break;
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
    ref.sampler_create_info.minFilter = VK_FILTER_LINEAR;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    break;
  case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
    ref.sampler_create_info.minFilter = VK_FILTER_NEAREST;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    break;
  case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    ref.sampler_create_info.minFilter = VK_FILTER_LINEAR;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    break;
  default:
    ref.sampler_create_info.minFilter = VK_FILTER_LINEAR;
    ref.sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    ref.sampler_create_info.maxLod = 0.25f;
    break;
  }
  switch (sampler.wrapS) {
  case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
    ref.sampler_create_info.addressModeU =
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    break;
  case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
    ref.sampler_create_info.addressModeU =
        VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    break;
  case TINYGLTF_TEXTURE_WRAP_REPEAT:
    ref.sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    break;
  }
  switch (sampler.wrapT) {
  case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
    ref.sampler_create_info.addressModeV =
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    break;
  case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
    ref.sampler_create_info.addressModeV =
        VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    break;
  case TINYGLTF_TEXTURE_WRAP_REPEAT:
    ref.sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    break;
  }
  switch (sampler.wrapR) {
  case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
    ref.sampler_create_info.addressModeW =
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    break;
  case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
    ref.sampler_create_info.addressModeW =
        VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    break;
  case TINYGLTF_TEXTURE_WRAP_REPEAT:
    ref.sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    break;
  }
  ref.sampler_create_info.anisotropyEnable = VK_TRUE;
  ref.sampler_create_info.maxAnisotropy = 8.0f;
  ref.sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  ref.sampler_create_info.unnormalizedCoordinates = VK_FALSE;
  ref.sampler_create_info.compareEnable = VK_FALSE;
  ref.sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  ref.sampler_create_info.mipLodBias = 0.0f;
  // Image
  ref.format = VK_FORMAT_R8G8B8A8_UNORM;
  int width, height, channels;
  const std::string filepath = base_dir + "/" + image.uri;
  ref.image_data.resize(1);
  ref.image_data[0].push_back(
      stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha));
  CHECK_PC(ref.image_data[0][0] != nullptr,
           "failed to load image: " + filepath);
  ref.size = width * height * 4;
  ref.width = static_cast<uint32_t>(width);
  ref.height = static_cast<uint32_t>(height);
  ref.channels = static_cast<uint32_t>(channels);
  ref.build_mipmaps = true;

  LOG(INFO) << "loaded texture '" << filepath << "': width=" << ref.width
            << " height=" << ref.height << " channels=" << ref.channels << "\n";
}

} // namespace gltf
} // namespace support
} // namespace zrl

#endif // GLTF_BINDERS_H_
