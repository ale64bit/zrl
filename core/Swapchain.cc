#include "core/Swapchain.h"

#include <limits>

#include "core/Log.h"

namespace zrl {

constexpr VkFormat kPreferredColorFormat = VK_FORMAT_B8G8R8A8_UNORM;

static VkSurfaceFormatKHR
ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
  if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
    return {kPreferredColorFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }
  for (const auto &format : formats) {
    if (format.format == kPreferredColorFormat &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return formats[0];
}

static VkPresentModeKHR
ChoosePresentMode(const std::vector<VkPresentModeKHR> &modes) {
  VkPresentModeKHR best_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
  for (const auto &mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
    if (mode == VK_PRESENT_MODE_FIFO_KHR) {
      best_mode = mode;
    }
  }
  return best_mode;
}

static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                               int width, int height) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  VkExtent2D actual_extent = {static_cast<uint32_t>(width),
                              static_cast<uint32_t>(height)};
  actual_extent.width = std::max(
      capabilities.minImageExtent.width,
      std::min(capabilities.maxImageExtent.width, actual_extent.width));
  actual_extent.height = std::max(
      capabilities.minImageExtent.height,
      std::min(capabilities.maxImageExtent.height, actual_extent.height));
  return actual_extent;
}

Swapchain::Swapchain(LogicalDevice &device, VkSurfaceKHR surface, int width,
                     int height)
    : device_(device.GetHandle()) {
  auto physical_device = device.GetPhysicalDevice();
  auto queue_families = device.GetQueueFamilies();
  auto surface_capabilities = physical_device.GetSurfaceCapabilities(surface);

  surface_format_ =
      ChooseSurfaceFormat(physical_device.GetSurfaceFormats(surface));
  present_mode_ = ChoosePresentMode(physical_device.GetPresentModes(surface));
  extent_ = ChooseExtent(surface_capabilities, width, height);
  image_count_ = surface_capabilities.minImageCount + 1;
  if (surface_capabilities.maxImageCount > 0 &&
      image_count_ > surface_capabilities.maxImageCount) {
    image_count_ = surface_capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface;
  create_info.minImageCount = image_count_;
  create_info.imageFormat = surface_format_.format;
  create_info.imageColorSpace = surface_format_.colorSpace;
  create_info.imageExtent = extent_;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (queue_families.size() == 1) {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount =
        static_cast<uint32_t>(queue_families.size());
    create_info.pQueueFamilyIndices = queue_families.data();
  }
  create_info.preTransform = surface_capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode_;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  CHECK_VK(vkCreateSwapchainKHR(device.GetHandle(), &create_info, nullptr,
                                &swapchain_));

  uint32_t image_count = 0;
  vkGetSwapchainImagesKHR(device.GetHandle(), swapchain_, &image_count,
                          nullptr);
  images_.resize(image_count);
  vkGetSwapchainImagesKHR(device.GetHandle(), swapchain_, &image_count,
                          images_.data());
}

Swapchain::~Swapchain() { vkDestroySwapchainKHR(device_, swapchain_, nullptr); }

} // namespace zrl
