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

#ifndef ZRL_CORE_BUFFER_POOL_H_
#define ZRL_CORE_BUFFER_POOL_H_

#include "vulkan/vulkan.h"

#include <set>
#include <unordered_map>

#include "core/Buffer.h"
#include "core/Core.h"

namespace zrl {

using Block = std::pair<VkDeviceSize, VkDeviceSize>; // size, offset

constexpr Block kEmptyBlock = Block(0, 0);

class BufferPool final : public Buffer {
public:
  BufferPool(const Core &core, const char *id, VkDeviceSize size,
             VkMemoryPropertyFlags mem_props, VkBufferUsageFlags usage,
             VkDeviceSize min_block_size, bool mapped);
  ~BufferPool();

  Block Alloc(VkDeviceSize);
  void Free(Block);
  void Write(VkDeviceSize offset, VkDeviceSize size, const void *src) const;
  VkDeviceSize LargestBlock() const;

private:
  const std::string id_;
  const VkDeviceSize min_block_size_;
  VkDeviceSize free_size_ = 0;
  std::set<Block> blocks_;
  void *mapped_;
};

} // namespace zrl

#endif // ZRL_CORE_BUFFER_POOL_H_
