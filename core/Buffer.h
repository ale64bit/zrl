#ifndef ZRL_CORE_BUFFER_H_
#define ZRL_CORE_BUFFER_H_

#include "vulkan/vulkan.h"

#include "core/Core.h"

namespace zrl {

class Buffer {
public:
  Buffer(const Core &core, VkDeviceSize size, VkMemoryPropertyFlags mem_props,
         VkBufferUsageFlags usage);
  virtual ~Buffer();

  VkBuffer GetHandle() const { return buffer_; }
  VkDeviceSize GetSize() const { return size_; }

protected:
  const VkDevice device_;
  const VkDeviceSize size_;
  VkBuffer buffer_;
  VkDeviceMemory memory_;
};

} // namespace zrl

#endif // ZRL_CORE_BUFFER_H_
