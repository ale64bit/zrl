#include "core/Image.h"

#include <string>
#include <vector>

#include "core/Log.h"

namespace zrl {

Image::Image(const Core &core, VkExtent3D extent, uint32_t levels,
             uint32_t layers, VkFormat format, VkImageType img_type,
             VkImageViewType view_type, VkImageTiling tiling,
             VkImageUsageFlags usage, VkMemoryPropertyFlags mem_prop_flags,
             VkImageAspectFlags aspect_mask)
    : device_(core.GetLogicalDevice().GetHandle()), extent_(extent),
      levels_(levels), layers_(layers), format_(format), view_type_(view_type),
      aspect_mask_(aspect_mask) {

  VkImageCreateInfo image_create_info = {};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.flags = 0;
  image_create_info.imageType = img_type;
  image_create_info.extent = extent_;
  image_create_info.mipLevels = levels_;
  image_create_info.arrayLayers = layers_;
  image_create_info.format = format_;
  image_create_info.tiling = tiling;
  image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_create_info.usage = usage;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (view_type_ == VK_IMAGE_VIEW_TYPE_CUBE ||
      view_type_ == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
    image_create_info.flags ^= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
  }
  CHECK_VK(vkCreateImage(device_, &image_create_info, nullptr, &image_));

  VkMemoryRequirements mem_reqs;
  vkGetImageMemoryRequirements(device_, image_, &mem_reqs);
  size_ = mem_reqs.size;

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_reqs.size;
  alloc_info.memoryTypeIndex =
      core.FindMemoryType(mem_reqs.memoryTypeBits, mem_prop_flags);
  CHECK_VK(vkAllocateMemory(device_, &alloc_info, nullptr, &memory_));
  CHECK_VK(vkBindImageMemory(device_, image_, memory_, 0));

  VkImageViewCreateInfo view_create_info = {};
  view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_create_info.image = image_;
  view_create_info.viewType = view_type_;
  view_create_info.format = format_;
  view_create_info.subresourceRange.aspectMask = aspect_mask_;
  view_create_info.subresourceRange.baseMipLevel = 0;
  view_create_info.subresourceRange.levelCount = levels_;
  view_create_info.subresourceRange.baseArrayLayer = 0;
  view_create_info.subresourceRange.layerCount = layers_;
  CHECK_VK(vkCreateImageView(device_, &view_create_info, nullptr, &view_));
}

Image::~Image() {
  vkDestroyImageView(device_, view_, nullptr);
  vkDestroyImage(device_, image_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

std::unique_ptr<Image> Image::Image1D(const Core &core, uint32_t extent,
                                      uint32_t levels, uint32_t layers,
                                      VkFormat format,
                                      VkImageUsageFlags usage) {
  VkExtent3D extent3D{extent, 1, 1};
  return std::unique_ptr<Image>(new Image(
      core, extent3D, levels, layers, format, VK_IMAGE_TYPE_1D,
      VK_IMAGE_VIEW_TYPE_1D, VK_IMAGE_TILING_OPTIMAL, usage,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
}

std::unique_ptr<Image> Image::Image2D(const Core &core, VkExtent2D extent,
                                      uint32_t levels, uint32_t layers,
                                      VkFormat format,
                                      VkImageUsageFlags usage) {
  VkExtent3D extent3D{extent.width, extent.height, 1};
  return std::unique_ptr<Image>(new Image(
      core, extent3D, levels, layers, format, VK_IMAGE_TYPE_2D,
      VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usage,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
}

std::unique_ptr<Image> Image::Image3D(const Core &core, VkExtent3D extent,
                                      uint32_t levels, uint32_t layers,
                                      VkFormat format,
                                      VkImageUsageFlags usage) {
  return std::unique_ptr<Image>(new Image(
      core, extent, levels, layers, format, VK_IMAGE_TYPE_3D,
      VK_IMAGE_VIEW_TYPE_3D, VK_IMAGE_TILING_OPTIMAL, usage,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
}

std::unique_ptr<Image> Image::CubeMap(const Core &core, VkExtent2D extent,
                                      uint32_t levels, VkFormat format,
                                      VkImageUsageFlags usage) {
  VkExtent3D extent3D{extent.width, extent.height, 1};
  return std::unique_ptr<Image>(new Image(
      core, extent3D, levels, 6, format, VK_IMAGE_TYPE_2D,
      VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_TILING_OPTIMAL, usage,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT));
}

std::unique_ptr<Image> Image::DepthBuffer(const Core &core, VkExtent2D extent,
                                          VkFormat format,
                                          VkImageUsageFlags usage) {
  VkExtent3D extent3D{extent.width, extent.height, 1};
  return std::unique_ptr<Image>(new Image(
      core, extent3D, 1, 1, format, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D,
      VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      VK_IMAGE_ASPECT_DEPTH_BIT));
}

} // namespace zrl
