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

#ifndef ZRL_LOGICAL_DEVICE_H_
#define ZRL_LOGICAL_DEVICE_H_

#include <memory>
#include <vector>

#include "vulkan/vulkan.h"

#include "core/PhysicalDevice.h"

namespace zrl {

constexpr uint32_t kInvalidQueueFamily = 0xFFFFFFFF;

class LogicalDevice {
public:
  LogicalDevice(PhysicalDevice, VkSurfaceKHR,
                const std::vector<const char *> &required_layers,
                const std::vector<const char *> &required_device_extensions);
  ~LogicalDevice();

  LogicalDevice(const LogicalDevice &) = delete;
  LogicalDevice(LogicalDevice &&) = delete;

  VkDevice GetHandle() const { return device_; }
  PhysicalDevice GetPhysicalDevice() const { return physical_device_; }
  std::vector<uint32_t> GetQueueFamilies() const { return queue_families_; }
  uint32_t GetGCTQueueFamily() const { return gct_queue_family_; }
  uint32_t GetPresentQueueFamily() const { return present_queue_family_; }
  VkQueue GetGCTQueue() const { return gct_queue_; }
  VkQueue GetPresentQueue() const { return present_queue_; }
  bool IsSingleQueue() const { return gct_queue_ == present_queue_; }

private:
  VkDevice device_;
  PhysicalDevice physical_device_;
  std::vector<uint32_t> queue_families_;
  uint32_t gct_queue_family_;
  uint32_t present_queue_family_;
  VkQueue gct_queue_;
  VkQueue present_queue_;
};

} // namespace zrl

#endif // ZRL_LOGICAL_DEVICE_H_
