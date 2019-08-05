#include "core/PhysicalDevice.h"

#include <cstring>
#include <iomanip>
#include <sstream>

#include "core/Constants.h"
#include "core/Log.h"

namespace zrl {

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device)
    : physical_device_(physical_device) {}

PhysicalDevice::~PhysicalDevice() {}

std::string PhysicalDevice::GetName() const {
  return GetProperties().deviceName;
}

VkPhysicalDeviceProperties PhysicalDevice::GetProperties() const {
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(physical_device_, &props);
  return props;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::GetMemoryProperties() const {
  VkPhysicalDeviceMemoryProperties props;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &props);
  return props;
}

std::vector<VkQueueFamilyProperties> PhysicalDevice::GetQueueFamilies() const {
  uint32_t queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_,
                                           &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device_, &queue_family_count, queue_families.data());
  return queue_families;
}

VkSurfaceCapabilitiesKHR
PhysicalDevice::GetSurfaceCapabilities(VkSurfaceKHR surface) const {
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_, surface,
                                            &capabilities);
  return capabilities;
}

std::vector<VkSurfaceFormatKHR>
PhysicalDevice::GetSurfaceFormats(VkSurfaceKHR surface) const {
  uint32_t format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface, &format_count,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> formats(format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface, &format_count,
                                       formats.data());
  return formats;
}

std::vector<VkPresentModeKHR>
PhysicalDevice::GetPresentModes(VkSurfaceKHR surface) const {
  uint32_t mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface,
                                            &mode_count, nullptr);
  std::vector<VkPresentModeKHR> modes(mode_count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface,
                                            &mode_count, modes.data());
  return modes;
}

VkFormatProperties PhysicalDevice::GetFormatProperties(VkFormat format) const {
  VkFormatProperties properties = {};
  vkGetPhysicalDeviceFormatProperties(physical_device_, format, &properties);
  return properties;
}

bool PhysicalDevice::SupportsExtensions(
    const std::vector<const char *> &required_extensions) const {
  uint32_t extension_count = 0;
  vkEnumerateDeviceExtensionProperties(physical_device_, nullptr,
                                       &extension_count, nullptr);
  std::vector<VkExtensionProperties> supported_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(
      physical_device_, nullptr, &extension_count, supported_extensions.data());
  for (auto extension : required_extensions) {
    bool ok = false;
    for (const auto &prop : supported_extensions) {
      if (strcmp(extension, prop.extensionName) == 0) {
        ok = true;
        break;
      }
    }
    if (!ok) {
      return false;
    }
  }
  return true;
}

bool PhysicalDevice::SupportsPresent(uint32_t queue_family,
                                     VkSurfaceKHR surface) const {
  VkBool32 supported = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, queue_family, surface,
                                       &supported);
  return supported;
}

std::vector<PhysicalDevice> PhysicalDevice::GetDevices(VkInstance instance) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  std::vector<VkPhysicalDevice> handles(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, handles.data());
  std::vector<PhysicalDevice> devices;
  for (auto handle : handles) {
    devices.emplace_back(PhysicalDevice(handle));
  }
  return devices;
}

static std::string formatDeviceSize(VkDeviceSize size) {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(1);
  if (size >= (1 << 30)) {
    ss << (size / double(1 << 30)) << " GiB";
  } else if (size >= (1 << 20)) {
    ss << (size / double(1 << 20)) << " MiB";
  } else if (size >= (1 << 10)) {
    ss << (size / double(1 << 10)) << " KiB";
  } else {
    ss << size << " bytes";
  }
  return ss.str();
}

std::ostream &operator<<(std::ostream &out, const PhysicalDevice &device) {
  auto properties = device.GetProperties();
  out << "\tdevice name: " << properties.deviceName << "\n"
      << "\tdevice type: " << VkPhysicalDeviceTypeStr[properties.deviceType]
      << "\n"
      << "\tapi version: " << VK_VERSION_MAJOR(properties.apiVersion) << '.'
      << VK_VERSION_MINOR(properties.apiVersion) << '.'
      << VK_VERSION_PATCH(properties.apiVersion) << "\n"
      << "\tdriver version: " << properties.driverVersion << "\n";

  auto queue_families = device.GetQueueFamilies();
  int i = 0;
  for (const auto &prop : queue_families) {
    std::string capabilities;
    for (auto kv : VkQueueFlagBitsStr) {
      if (prop.queueFlags & kv.first) {
        if (capabilities != "") {
          capabilities += ", ";
        }
        capabilities += kv.second;
      }
    }
    out << "\tqueue family " << i << "\n"
        << "\t\tqueueFlags: {" << capabilities << "}\n"
        << "\t\tqueueCount: " << prop.queueCount << "\n";
  }

  auto memory_properties = device.GetMemoryProperties();
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
    std::string propertyFlags;
    for (auto kv : VkMemoryPropertyFlagBitsStr) {
      if (memory_properties.memoryTypes[i].propertyFlags & kv.first) {
        if (propertyFlags != "") {
          propertyFlags += ", ";
        }
        propertyFlags += kv.second;
      }
    }
    out << "\tmemory type: " << i << "\n"
        << "\t\tpropertyFlags: {" << propertyFlags << "}\n"
        << "\t\theapIndex: " << memory_properties.memoryTypes[i].heapIndex
        << "\n";
  }
  for (uint32_t i = 0; i < memory_properties.memoryHeapCount; ++i) {
    std::string heapFlags;
    for (auto kv : VkMemoryHeapFlagBitsStr) {
      if (memory_properties.memoryHeaps[i].flags & kv.first) {
        if (heapFlags != "") {
          heapFlags += ", ";
        }
        heapFlags += kv.second;
      }
    }
    out << "\tmemory heap: " << i << "\n"
        << "\t\tsize: "
        << formatDeviceSize(memory_properties.memoryHeaps[i].size) << "\n"
        << "\t\tflags: {" << heapFlags << "}\n";
  }

  return out;
}

} // namespace zrl
