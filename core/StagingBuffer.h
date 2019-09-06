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
