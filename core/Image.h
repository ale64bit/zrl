#ifndef ZRL_CORE_IMAGE_H_
#define ZRL_CORE_IMAGE_H_

#include "vulkan/vulkan.h"

#include "core/Core.h"

namespace zrl {

class Image {
public:
  Image(const Core &core, VkExtent3D extent, uint32_t mip_levels,
        uint32_t layers, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags mem_prop_flags,
        VkImageAspectFlags aspect);
  ~Image();

  VkFormat GetFormat() const { return format_; }
  VkDeviceSize GetSize() const { return size_; }
  VkImage GetHandle() const { return image_; }
  VkImageView GetViewHandle() const { return view_; }
  VkExtent3D GetExtent() const { return extent_; }
  uint32_t GetMipLevels() const { return mip_levels_; }

private:
  const VkDevice device_;
  const VkExtent3D extent_;
  const VkFormat format_;
  uint32_t mip_levels_;
  VkDeviceSize size_;
  VkImage image_;
  VkDeviceMemory memory_;
  VkImageView view_;
};

} // namespace zrl

#endif // ZRL_CORE_IMAGE_H_
