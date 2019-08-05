#ifndef ZRL_PHYSICAL_DEVICE_H_
#define ZRL_PHYSICAL_DEVICE_H_

#include <iostream>
#include <vector>

#include "vulkan/vulkan.h"

namespace zrl {

class PhysicalDevice {
public:
  PhysicalDevice(VkPhysicalDevice);
  ~PhysicalDevice();

  VkPhysicalDevice GetHandle() const { return physical_device_; }
  std::string GetName() const;
  VkPhysicalDeviceProperties GetProperties() const;
  VkPhysicalDeviceMemoryProperties GetMemoryProperties() const;
  std::vector<VkQueueFamilyProperties> GetQueueFamilies() const;
  VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkSurfaceKHR) const;
  std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkSurfaceKHR) const;
  std::vector<VkPresentModeKHR> GetPresentModes(VkSurfaceKHR) const;
  VkFormatProperties GetFormatProperties(VkFormat) const;

  bool SupportsExtensions(const std::vector<const char *> &) const;
  bool SupportsPresent(uint32_t queue_family, VkSurfaceKHR surface) const;

  static std::vector<PhysicalDevice> GetDevices(VkInstance);

  friend std::ostream &operator<<(std::ostream &, const PhysicalDevice &);

private:
  VkPhysicalDevice physical_device_;
};

} // namespace zrl

#endif // ZRL_PHYSICAL_DEVICE_H_
