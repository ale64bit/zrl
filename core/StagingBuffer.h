#ifndef ZRL_CORE_STAGING_BUFFER_H_
#define ZRL_CORE_STAGING_BUFFER_H_

#include "vulkan/vulkan.h"

#include "core/Buffer.h"
#include "core/Core.h"

namespace zrl {

class StagingBuffer final : public Buffer {
public:
  StagingBuffer(const Core &core, VkDeviceSize size);
  ~StagingBuffer();

  VkDeviceSize PushData(VkDeviceSize size, const void *data);
  VkDeviceSize PushFile(VkDeviceSize size, const std::string &filename);
  void Flush();

private:
  const VkDeviceSize non_coherent_atom_size_;
  void *mapped_;
  VkDeviceSize offset_;
};

} // namespace zrl

#endif // ZRL_CORE_STAGING_BUFFER_H_
