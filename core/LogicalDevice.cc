#include "core/LogicalDevice.h"

#include <set>

#include "core/Log.h"

namespace zrl {

LogicalDevice::LogicalDevice(
    PhysicalDevice physical_device, VkSurfaceKHR surface,
    const std::vector<const char *> &required_layers,
    const std::vector<const char *> &required_device_extensions)
    : physical_device_(physical_device), gct_queue_family_(kInvalidQueueFamily),
      present_queue_family_(kInvalidQueueFamily), gct_queue_(VK_NULL_HANDLE),
      present_queue_(VK_NULL_HANDLE) {
  auto queue_families = physical_device_.GetQueueFamilies();
  uint32_t queue_family = 0;
  for (const auto &prop : queue_families) {
    if (prop.queueCount == 0) {
      continue;
    }
    if ((prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
        (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
        physical_device_.SupportsPresent(queue_family, surface)) {
      gct_queue_family_ = queue_family;
      present_queue_family_ = queue_family;
      break;
    } else if (gct_queue_family_ == kInvalidQueueFamily &&
               (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
               (prop.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
      gct_queue_family_ = queue_family;
    } else if (present_queue_family_ == kInvalidQueueFamily &&
               physical_device_.SupportsPresent(queue_family, surface)) {
      present_queue_family_ = queue_family;
    }
    if (gct_queue_family_ != kInvalidQueueFamily &&
        present_queue_family_ != kInvalidQueueFamily) {
      break;
    }
    ++queue_family;
  }
  CHECK_PC(gct_queue_family_ != kInvalidQueueFamily,
           "no gct queue family found");
  CHECK_PC(present_queue_family_ != kInvalidQueueFamily,
           "no present queue family found");

  std::set<uint32_t> unique_families = {
      gct_queue_family_,
      present_queue_family_,
  };

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  float queue_priority = 1.0f;
  for (int family : unique_families) {
    queue_families_.push_back(family);
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features = {};
  device_features.fillModeNonSolid = true;
  device_features.samplerAnisotropy = true;
  // TODO: make this configurable.

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = queue_create_infos.data();
  create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queue_create_infos.size());
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount =
      static_cast<uint32_t>(required_device_extensions.size());
  create_info.ppEnabledExtensionNames = required_device_extensions.data();
  create_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
  create_info.ppEnabledLayerNames = required_layers.data();
  CHECK_VK(vkCreateDevice(physical_device_.GetHandle(), &create_info, nullptr,
                          &device_));

  vkGetDeviceQueue(device_, gct_queue_family_, 0, &gct_queue_);
  vkGetDeviceQueue(device_, present_queue_family_, 0, &present_queue_);
  CHECK_PC(gct_queue_ != VK_NULL_HANDLE, "no gct queue found");
  CHECK_PC(present_queue_ != VK_NULL_HANDLE, "no present queue found");
}

LogicalDevice::~LogicalDevice() { vkDestroyDevice(device_, nullptr); }

} // namespace zrl
