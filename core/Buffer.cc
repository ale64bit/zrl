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

#include "core/Buffer.h"

#include "core/Log.h"

namespace zrl {

Buffer::Buffer(const Core &core, VkDeviceSize size,
               VkMemoryPropertyFlags mem_props, VkBufferUsageFlags usage)
    : device_(core.GetLogicalDevice().GetHandle()), size_(size),
      buffer_(VK_NULL_HANDLE), memory_(VK_NULL_HANDLE) {
  VkBufferCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.pNext = nullptr;
  create_info.flags = 0;
  create_info.size = size_;
  create_info.usage = usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  create_info.queueFamilyIndexCount = 0;
  create_info.pQueueFamilyIndices = nullptr;

  CHECK_VK(vkCreateBuffer(device_, &create_info, nullptr, &buffer_));

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(device_, buffer_, &mem_reqs);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = nullptr;
  alloc_info.allocationSize = mem_reqs.size;
  alloc_info.memoryTypeIndex =
      core.FindMemoryType(mem_reqs.memoryTypeBits, mem_props);
  CHECK_VK(vkAllocateMemory(device_, &alloc_info, nullptr, &memory_));
  CHECK_VK(vkBindBufferMemory(device_, buffer_, memory_, 0));
}

Buffer::~Buffer() {
  vkDestroyBuffer(device_, buffer_, nullptr);
  vkFreeMemory(device_, memory_, nullptr);
}

} // namespace zrl
