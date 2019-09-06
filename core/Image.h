/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
        VkSampleCountFlagBits samples, VkMemoryPropertyFlags mem_prop_flags,
        VkImageAspectFlags aspect_mask);
  ~Image();

  // Constructors
  static std::unique_ptr<Image> Image1D(const Core &core, uint32_t extent,
                                        uint32_t levels, uint32_t layers,
                                        VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> Image2D(const Core &core, VkExtent2D extent,
                                        uint32_t levels, uint32_t layers,
                                        VkFormat format,
                                        VkImageUsageFlags usage,
                                        VkSampleCountFlagBits samples);
  static std::unique_ptr<Image> Image3D(const Core &core, VkExtent3D extent,
                                        uint32_t levels, uint32_t layers,
                                        VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> CubeMap(const Core &core, VkExtent2D extent,
                                        uint32_t levels, VkFormat format,
                                        VkImageUsageFlags usage);
  static std::unique_ptr<Image> DepthBuffer(const Core &core, VkExtent2D extent,
                                            VkFormat format,
                                            VkImageUsageFlags usage,
                                            VkSampleCountFlagBits samples);

  // Accessors
  VkExtent3D GetExtent() const { return extent_; }
  uint32_t GetLevelCount() const { return levels_; }
  uint32_t GetLayerCount() const { return layers_; }
  VkFormat GetFormat() const { return format_; }
  VkImageViewType GetViewType() const { return view_type_; }
  VkSampleCountFlagBits GetSampleCount() const { return samples_; }
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
  VkSampleCountFlagBits samples_;
  const VkImageAspectFlags aspect_mask_;
  VkDeviceSize size_;
  VkImage image_;
  VkDeviceMemory memory_;
  VkImageView view_;
};

} // namespace zrl

#endif // ZRL_CORE_IMAGE_H_
