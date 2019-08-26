#ifndef ZRL_CORE_IMAGE_H_
#define ZRL_CORE_IMAGE_H_

#include "vulkan/vulkan.h"

#include "core/Core.h"

namespace zrl {

class Image {
public:
  Image(const Core &core, VkExtent3D extent, uint32_t levels, uint32_t layers,
        VkFormat format, VkImageType img_type, VkImageViewType view_type,
        VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags mem_prop_flags, VkImageAspectFlags aspect_mask);
  ~Image();

  // Constructors
  static std::unique_ptr<Image> Image1D(const Core &core, uint32_t extent,
                                        uint32_t levels, uint32_t layers,
                                        VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> Image2D(const Core &core, VkExtent2D extent,
                                        uint32_t levels, uint32_t layers,
                                        VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> Image3D(const Core &core, VkExtent3D extent,
                                        uint32_t levels, uint32_t layers,
                                        VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> CubeMap(const Core &core, VkExtent2D extent,
                                        uint32_t levels, VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> DepthBuffer(const Core &core, VkExtent2D extent,
                                            VkFormat format,
                                            VkImageUsageFlags usage);

  // Accessors
  VkExtent3D GetExtent() const { return extent_; }
  uint32_t GetLevelCount() const { return levels_; }
  uint32_t GetLayerCount() const { return layers_; }
  VkFormat GetFormat() const { return format_; }
  VkImageViewType GetViewType() const { return view_type_; }
  VkImageAspectFlags GetAspects() const { return aspect_mask_; }
  VkDeviceSize GetSize() const { return size_; }
  VkImage GetHandle() const { return image_; }
  VkImageView GetViewHandle() const { return view_; }

private:
  const VkDevice device_;
  const VkExtent3D extent_;
  const uint32_t levels_;
  const uint32_t layers_;
  const VkFormat format_;
  const VkImageViewType view_type_;
  const VkImageAspectFlags aspect_mask_;
  VkDeviceSize size_;
  VkImage image_;
  VkDeviceMemory memory_;
  VkImageView view_;
};

} // namespace zrl

#endif // ZRL_CORE_IMAGE_H_
