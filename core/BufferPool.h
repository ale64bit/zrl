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
