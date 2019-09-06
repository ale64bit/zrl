// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "core/Core.h"

#include <cstring>
#include <vector>

#include "core/Constants.h"
#include "core/Log.h"

namespace zrl {

// Instance extensions
static const std::vector<const char *> kRequiredInstanceExtensionsDebug = {
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
};
static const std::vector<const char *> kRequiredInstanceExtensions = {};

// Device extensions
static const std::vector<const char *> kRequiredDeviceExtensionsDebug = {};
static const std::vector<const char *> kRequiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

// Validation layers
static const std::vector<const char *> kRequiredLayersDebug = {
    "VK_LAYER_LUNARG_standard_validation",
};
static const std::vector<const char *> kRequiredLayers = {};

static std::vector<VkExtensionProperties> GetSupportedInstanceExtensions() {
  uint32_t extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> supported_extensions(extension_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &extension_count,
                                         supported_extensions.data());
  return supported_extensions;
}

static std::vector<const char *> GetRequiredInstanceExtensions(bool debug) {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  std::vector<const char *> extensions(glfw_extensions,
                                       glfw_extensions + glfw_extension_count);

  for (auto ext : kRequiredInstanceExtensions) {
    extensions.push_back(ext);
  }
  if (debug) {
    for (auto ext : kRequiredInstanceExtensionsDebug) {
      extensions.push_back(ext);
    }
  }
  return extensions;
}

static std::vector<VkLayerProperties> GetSupportedLayers() {
  uint32_t layer_count = 0;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::vector<VkLayerProperties> supported_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, supported_layers.data());
  return supported_layers;
}

static std::vector<const char *> GetRequiredLayers(bool debug) {
  std::vector<const char *> layers(kRequiredLayers);
  if (debug) {
    for (auto layer : kRequiredLayersDebug) {
      layers.push_back(layer);
    }
  }
  return layers;
}

static std::vector<VkExtensionProperties>
GetSupportedDeviceExtensions(VkPhysicalDevice device) {
  uint32_t extension_count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       nullptr);
  std::vector<VkExtensionProperties> supported_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count,
                                       supported_extensions.data());
  return supported_extensions;
}

static std::vector<const char *> GetRequiredDeviceExtensions(bool debug) {
  std::vector<const char *> extensions(kRequiredDeviceExtensions);
  if (debug) {
    for (auto ext : kRequiredDeviceExtensionsDebug) {
      extensions.push_back(ext);
    }
  }
  return extensions;
}

PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;

static VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type,
              uint64_t obj, size_t location, int32_t code,
              const char *layer_prefix, const char *msg, void *) {
  std::string flags_str;
  for (auto kv : VkDebugReportFlagBitsEXTStr) {
    if (flags & kv.first) {
      flags_str += kv.second + " ";
    }
  }
  const char *tag = DEBUG;
  if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
    tag = ERROR;
  } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
    tag = WARNING;
  } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
    tag = WARNING;
  } else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
    tag = INFO;
  }
  LOG(tag) << tag << "DebugCallback:\n"
           << "\tobj_type=" << VkDebugReportObjectTypeEXTStr.at(obj_type)
           << "\n"
           << "\tflags=" << flags_str << "\n"
           << "\tlayer_prefix=" << layer_prefix << "\n"
           << "\tmsg='" << msg << "'\n"
           << "\tobj=" << obj << "\n"
           << "\tlocation=" << location << "\n"
           << "\tcode=" << code << "\n";
  return VK_FALSE;
}

Core::Core(Config config) : config_(config) {
  DLOG << "Core: ctor\n";
  CreateWindow();
  CreateInstance();
  SetupDebugCallback();
  CreateSurface();
  CreateLogicalDevice();
  CreateSwapchain();
}

Core::~Core() {
  DLOG << "Core: dtor\n";
  swapchain_.reset();
  device_.reset();
  DestroyDebugCallback();
  DestroySurface();
  vkDestroyInstance(instance_, nullptr);
  glfwDestroyWindow(window_);
}

// See:
// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceMemoryProperties.html
int32_t Core::FindMemoryType(uint32_t mem_type_requirements,
                             VkMemoryPropertyFlags required_props) const {
  const VkPhysicalDeviceMemoryProperties mem_props =
      device_->GetPhysicalDevice().GetMemoryProperties();
  const uint32_t mem_count = mem_props.memoryTypeCount;
  for (uint32_t mem_index = 0; mem_index < mem_count; ++mem_index) {
    const uint32_t mem_type_bits = (1 << mem_index);
    const bool is_required_mem_type = mem_type_requirements & mem_type_bits;
    const VkMemoryPropertyFlags properties =
        mem_props.memoryTypes[mem_index].propertyFlags;
    const bool has_required_props =
        (properties & required_props) == required_props;

    if (is_required_mem_type && has_required_props) {
      return static_cast<int32_t>(mem_index);
    }
  }

  CHECK_PC(false, "failed to find suitable memory type");
}

void Core::UpdateSwapchain() {
  CHECK_VK(vkQueueWaitIdle(device_->GetGCTQueue()));
  CHECK_VK(vkQueueWaitIdle(device_->GetPresentQueue()));
  CHECK_VK(vkDeviceWaitIdle(device_->GetHandle()));
  swapchain_.reset();
  CreateSwapchain();
}

void Core::CreateSurface() {
  LOG(INFO) << "Core: creating window surface\n";
  CHECK_VK(glfwCreateWindowSurface(instance_, window_, nullptr, &surface_));
}

void Core::DestroySurface() {
  vkDestroySurfaceKHR(instance_, surface_, nullptr);
}

void Core::CreateWindow() {
  LOG(INFO) << "Core: creating GLFW window\n";
  glfwInit();
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  if (config_.fullscreen) {
    const GLFWvidmode *vmode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, vmode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vmode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vmode->blueBits);
    window_ = glfwCreateWindow(config_.width, config_.height,
                               config_.app_name.c_str(), monitor, nullptr);
  } else {
    window_ = glfwCreateWindow(config_.width, config_.height,
                               config_.app_name.c_str(), nullptr, nullptr);
  }
  CHECK_PC(window_ != nullptr, "window_ was not created successfully");
}

void Core::ListSupportedInstanceExtensions() const {
  auto supported_extensions = GetSupportedInstanceExtensions();
  DLOG << "Core: supported instance extensions:\n";
  for (const auto &ext : supported_extensions) {
    DLOG << "\t\t" << ext.extensionName << "\n";
  }
}

void Core::ListSupportedDeviceExtensions() const {
  auto supported_extensions =
      GetSupportedDeviceExtensions(device_->GetPhysicalDevice().GetHandle());
  DLOG << "Core: supported device extensions:"
       << "\n";
  for (const auto &ext : supported_extensions) {
    DLOG << "\t\t" << ext.extensionName << "\n";
  }
}

void Core::ListSupportedLayers() const {
  auto supported_layers = GetSupportedLayers();
  DLOG << "Core: supported layers:\n";
  for (const auto &layer : supported_layers) {
    DLOG << "\t\t" << layer.layerName << "\n";
  }
}

void Core::CheckLayerSupport() const {
  LOG(INFO) << "Core: checking validation layer support\n";
  auto required_layers = GetRequiredLayers(config_.debug);
  auto supported_layers = GetSupportedLayers();
  for (auto layer : required_layers) {
    bool ok = false;
    for (const auto &prop : supported_layers) {
      if (strcmp(layer, prop.layerName) == 0) {
        ok = true;
        break;
      }
    }
    CHECK_PC(ok, "required_layer not supported: " + std::string(layer));
  }
}

void Core::CheckInstanceExtensionSupport() const {
  LOG(INFO) << "Core: checking instance extension support\n";
  auto required_extensions = GetRequiredInstanceExtensions(config_.debug);
  auto supported_extensions = GetSupportedInstanceExtensions();
  for (auto extension : required_extensions) {
    bool ok = false;
    for (const auto &prop : supported_extensions) {
      if (strcmp(extension, prop.extensionName) == 0) {
        ok = true;
        break;
      }
    }
    CHECK_PC(ok, "required extension not supported: " + std::string(extension));
  }
}

void Core::CreateInstance() {
  LOG(INFO) << "Core: creating Vulkan instance\n";
  DLOG << "Core: vulkan header version: " << VK_HEADER_VERSION << "\n";
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = config_.app_name.c_str();
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = config_.engine_name.c_str();
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  if (config_.debug) {
    ListSupportedInstanceExtensions();
    ListSupportedLayers();
  }

  CheckLayerSupport();
  CheckInstanceExtensionSupport();

  auto required_extensions = GetRequiredInstanceExtensions(config_.debug);
  create_info.enabledExtensionCount =
      static_cast<uint32_t>(required_extensions.size());
  create_info.ppEnabledExtensionNames = required_extensions.data();

  auto required_layers = GetRequiredLayers(config_.debug);
  create_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
  create_info.ppEnabledLayerNames = required_layers.data();

  CHECK_VK(vkCreateInstance(&create_info, nullptr, &instance_));
}

void Core::SetupDebugCallback() {
  if (!config_.debug) {
    return;
  }
  DLOG << "Core: setting up debug callback\n";

  CreateDebugReportCallback =
      reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
          vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT"));
  DestroyDebugReportCallback =
      reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
          vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT"));

  VkDebugReportCallbackCreateInfoEXT create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  create_info.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
                      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                      VK_DEBUG_REPORT_ERROR_BIT_EXT |
                      VK_DEBUG_REPORT_DEBUG_BIT_EXT;
  create_info.pfnCallback = DebugCallback;

  CHECK_VK(CreateDebugReportCallback(instance_, &create_info, nullptr,
                                     &debug_callback_));
}

void Core::DestroyDebugCallback() {
  if (config_.debug) {
    DestroyDebugReportCallback(instance_, debug_callback_, nullptr);
  }
}

void Core::CreateLogicalDevice() {
  LOG(INFO) << "Core: creating logical device\n";
  auto physical_devices = PhysicalDevice::GetDevices(instance_);
  DLOG << "Core: enumerating physical devices\n";
  for (size_t i = 0; i < physical_devices.size(); ++i) {
    DLOG << "Core: physical device #" << i << "\n" << physical_devices[i];
  }
  auto required_device_extensions = GetRequiredDeviceExtensions(config_.debug);
  for (auto physical_device : physical_devices) {
    // TODO: score devices to pick the most suitable
    if (physical_device.SupportsExtensions(required_device_extensions)) {
      LOG(INFO) << "Core: selected device: " << physical_device.GetName()
                << "\n";
      device_ = std::make_unique<LogicalDevice>(
          physical_device, surface_, GetRequiredLayers(config_.debug),
          GetRequiredDeviceExtensions(config_.debug));
      break;
    }
  }
  CHECK_PC(device_, "Could not find suitable device");

  if (config_.debug) {
    ListSupportedDeviceExtensions();
  }

  DLOG << "Core: gct queue: " << device_->GetGCTQueue() << "\n";
  DLOG << "Core: present queue: " << device_->GetPresentQueue() << "\n";
}

void Core::CreateSwapchain() {
  LOG(INFO) << "Core: creating swapchain\n";
  swapchain_ = std::make_unique<Swapchain>(*device_, surface_, config_.width,
                                           config_.height);
  CHECK_PC(swapchain_, "Could not create swapchain");
  DLOG << "Core: swapchain created:\n"
       << "\tformat: " << VkFormatStr.at(swapchain_->GetSurfaceFormat()) << "\n"
       << "\tcolor_space: "
       << VkColorSpaceKHRStr.at(swapchain_->GetColorSpace()) << "\n"
       << "\tpresent_mode: "
       << VkPresentModeKHRStr.at(swapchain_->GetPresentMode()) << "\n"
       << "\timage_count: " << swapchain_->GetImageCount() << "\n"
       << "\twidth: " << swapchain_->GetExtent().width << "\n"
       << "\theight: " << swapchain_->GetExtent().height << "\n";
}

} // namespace zrl
