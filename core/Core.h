#ifndef ZRL_CORE_H_
#define ZRL_CORE_H_

#include <memory>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "core/LogicalDevice.h"
#include "core/Swapchain.h"

namespace zrl {

struct Config {
  std::string app_name;
  std::string engine_name;
  uint32_t width;
  uint32_t height;
  bool fullscreen;
  bool debug;
};

class Core {
public:
  Core(Config);
  ~Core();

  const LogicalDevice &GetLogicalDevice() const { return *device_; }
  const Swapchain &GetSwapchain() const { return *swapchain_; }
  GLFWwindow *GetWindow() const { return window_; }
  int32_t FindMemoryType(uint32_t mem_type_requirements,
                         VkMemoryPropertyFlags required_props) const;
  void UpdateSwapchain();

private:
  const Config config_;
  GLFWwindow *window_;
  VkInstance instance_;
  VkDebugReportCallbackEXT debug_callback_;
  VkSurfaceKHR surface_;
  std::unique_ptr<LogicalDevice> device_;
  std::unique_ptr<Swapchain> swapchain_;

  void CreateWindow();
  void CreateInstance();
  void CreateSurface();
  void DestroySurface();
  void SetupDebugCallback();
  void DestroyDebugCallback();
  void CreateLogicalDevice();
  void CreateSwapchain();
  void ListSupportedInstanceExtensions() const;
  void ListSupportedDeviceExtensions() const;
  void ListSupportedLayers() const;
  void CheckInstanceExtensionSupport() const;
  void CheckLayerSupport() const;
};

} // namespace zrl

#endif // ZRL_CORE_H_
