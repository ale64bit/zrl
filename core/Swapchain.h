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
