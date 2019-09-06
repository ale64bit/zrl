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

#include "core/BufferPool.h"

#include <cstring>

#include "core/Log.h"

namespace zrl {

static inline Block Buddy(Block b) {
  return Block(b.first, b.first ^ b.second);
}

BufferPool::BufferPool(const Core &core, const char *id, VkDeviceSize size,
                       VkMemoryPropertyFlags mem_props,
                       VkBufferUsageFlags usage, VkDeviceSize min_block_size,
                       bool mapped)
    : Buffer(core, size, mem_props, usage), id_(id),
      min_block_size_(min_block_size), free_size_(size), mapped_(nullptr) {
  CHECK_PC((size & (size - 1)) == 0, "size must be a power of two");
  blocks_.insert(std::make_pair(size, 0));
  if (mapped) {
    CHECK_VK(vkMapMemory(device_, memory_, 0, size_, 0, &mapped_));
  }
}

BufferPool::~BufferPool() {
  if (mapped_ != nullptr) {
    vkUnmapMemory(device_, memory_);
  }
}

Block BufferPool::Alloc(VkDeviceSize size) {
  while (true) {
    auto it = blocks_.lower_bound(std::make_pair(size, 0));
    if (it == blocks_.end()) { // Out of memory
      LOG(ERROR) << "BufferPool(" << id_ << "): OOM\n";
      return kEmptyBlock;
    }
    if (it->first > min_block_size_ && size + size <= it->first) {
      const VkDeviceSize new_size = it->first >> 1;
      const auto lo = Block(new_size, it->second);
      const auto hi = Block(new_size, it->second + new_size);
      blocks_.erase(it);
      blocks_.insert(lo);
      blocks_.insert(hi);
    } else {
      Block b = *it;
      blocks_.erase(it);
      free_size_ -= b.first;
      return b;
    }
  }
}

void BufferPool::Free(Block b) {
  while (true) {
    Block bb = Buddy(b);
    auto it = blocks_.find(bb);
    if (it != blocks_.end()) {
      blocks_.erase(it);
      b = Block(b.first << 1, std::min(b.second, bb.second));
    } else {
      blocks_.insert(b);
      break;
    }
  }
  free_size_ += b.first;
}

void BufferPool::Write(VkDeviceSize offset, VkDeviceSize size,
                       const void *src) const {
  std::memcpy(reinterpret_cast<char *>(mapped_) + offset, src, size);
}

VkDeviceSize BufferPool::LargestBlock() const {
  return blocks_.rbegin()->first;
}

} // namespace zrl
