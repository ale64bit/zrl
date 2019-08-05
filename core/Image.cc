#include "core/Image.h"

#include <string>
#include <vector>

#include "core/Log.h"

namespace zrl {

Image::Image(const Core &core, VkExtent3D extent, uint32_t mip_levels,
             uint32_t layers, VkFormat format, VkImageTiling tiling,
             VkImageUsageFlags usage, VkMemoryPropertyFlags mem_prop_flags,
             VkImageAspectFlags aspect)
    : device_(core.GetLogicalDevice().GetHandle()), extent_(extent),
      format_(format), mip_levels_(mip_levels) {
  VkImageCreateInfo image_create_info = {};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.extent = extent;
  image_create_info.mipLevels = mip_levels_;
  image_create_info.arrayLayers = layers;
  image_create_info.format = format;
  image_create_info.tiling = tiling;
  image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_create_info.usage = usage;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
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
  view_create_info.viewType =
      // TODO: change this to support 3D images.
      (layers == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  view_create_info.format = format_;
  view_create_info.subresourceRange.aspectMask = aspect;
  view_create_info.subresourceRange.baseMipLevel = 0;
  view_create_info.subresourceRange.levelCount = mip_levels_;
  view_create_info.subresourceRange.baseArrayLayer = 0;
  view_create_info.subresourceRange.layerCount = layers;
  CHECK_VK(vkCreateImageView(device_, &view_create_info, nullptr, &view_));
}

Image::~Image() {
  vkDestroyImageView(device_, view_, nullptr);
  vkDestroyImage(device_, image_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

}; // namespace zrl
