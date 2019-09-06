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

#ifndef ZRL_SWAPCHAIN_H_
#define ZRL_SWAPCHAIN_H_

#include "vulkan/vulkan.h"

#include "core/LogicalDevice.h"

namespace zrl {

class Swapchain {
public:
  Swapchain(LogicalDevice &device, VkSurfaceKHR surface, int width, int height);
  ~Swapchain();

  VkSwapchainKHR GetHandle() const { return swapchain_; }
  VkFormat GetSurfaceFormat() const { return surface_format_.format; }
  VkColorSpaceKHR GetColorSpace() const { return surface_format_.colorSpace; }
  VkExtent2D GetExtent() const { return extent_; }
  float GetAspect() const {
    return static_cast<float>(extent_.width) / extent_.height;
  }
  uint32_t GetImageCount() const { return image_count_; }
  VkPresentModeKHR GetPresentMode() const { return present_mode_; }
  const std::vector<VkImage> &GetImages() const { return images_; }

private:
  VkDevice device_;
  VkSwapchainKHR swapchain_;
  VkSurfaceFormatKHR surface_format_;
  VkExtent2D extent_;
  uint32_t image_count_;
  VkPresentModeKHR present_mode_;
  std::vector<VkImage> images_;
};

} // namespace zrl

#endif // ZRL_SWAPCHAIN_H_
