#include "core/StagingBuffer.h"

#include <cstring>

#include "core/Log.h"

namespace zrl {

static inline VkDeviceSize Align(VkDeviceSize size, VkDeviceSize alignment) {
  return (size - 1) - ((size - 1) % alignment) + alignment;
}

StagingBuffer::StagingBuffer(const Core &core, VkDeviceSize size)
    : Buffer(core, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
             VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
      non_coherent_atom_size_(core.GetLogicalDevice()
                                  .GetPhysicalDevice()
                                  .GetProperties()
                                  .limits.nonCoherentAtomSize),
      mapped_(nullptr), offset_(0) {
  CHECK_VK(vkMapMemory(device_, memory_, 0, size_, 0, &mapped_));
}

StagingBuffer::~StagingBuffer() { vkUnmapMemory(device_, memory_); }

VkDeviceSize StagingBuffer::PushData(VkDeviceSize size, const void *data) {
  CHECK_PC(size > 0, "size must be positive");
  CHECK_PC(offset_ + size <= size_, "buffer overflow");
  CHECK_PC(data != nullptr, "data cannot be nullptr");
  VkDeviceSize cur_offset = offset_;
  std::memcpy(reinterpret_cast<char *>(mapped_) + offset_, data, size);
  offset_ = Align(offset_ + size, 16);
  return cur_offset;
}

VkDeviceSize StagingBuffer::PushFile(VkDeviceSize size,
                                     const std::string &filename) {
  CHECK_PC(size > 0, "size must be positive");
  CHECK_PC(offset_ + size <= size_, "buffer overflow");
  VkDeviceSize cur_offset = offset_;
  std::FILE *f = std::fopen(filename.c_str(), "rb");
  CHECK_PC(
      std::fread(reinterpret_cast<char *>(mapped_) + offset_, size, 1, f) == 1,
      "error reading from file");
  offset_ = Align(offset_ + size, 16);
  return cur_offset;
}

void StagingBuffer::Flush() {
  if (offset_ == 0) {
    return;
  }
  VkMappedMemoryRange range = {};
  range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range.pNext = nullptr;
  range.memory = memory_;
  range.offset = 0;
  range.size = Align(offset_, non_coherent_atom_size_);
  CHECK_VK(vkFlushMappedMemoryRanges(device_, 1, &range));
  offset_ = 0;
}

} // namespace zrl
