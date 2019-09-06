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

#ifndef ZRL_CORE_LOG_H
#define ZRL_CORE_LOG_H

#include <iostream>

#include "vulkan/vulkan.h"

#include "core/Constants.h"

namespace zrl {

#define DEBUG "\033[0;32mdebug: "
#define INFO "\033[0minfo: "
#define WARNING "\033[0;33mwarn: "
#define ERROR "\033[0;31merror: "

#define LOG(l) std::clog << (l)

#ifdef NDEBUG
#define DLOG                                                                   \
  if (false)                                                                   \
  LOG(DEBUG)
#else
#define DLOG LOG(DEBUG)
#endif

#define CHECK_GENERAL(condition, msg)                                          \
  if (!(condition)) {                                                          \
    LOG(ERROR) << __FILE__ << ":" << __LINE__ << ": " << (#condition) << ": "  \
               << msg << "\033[0m\n";                                          \
    std::abort();                                                              \
  }

#define CHECK(condition, msg) CHECK_GENERAL(condition, msg)
#define CHECK_ARG(condition, msg)                                              \
  CHECK_GENERAL(condition, "invalid argument: " msg)
#define CHECK_PC(condition, msg)                                               \
  CHECK_GENERAL(condition, "precondition failed: " msg)
#define CHECK_VK(val)                                                          \
  CHECK_GENERAL((val) == VK_SUCCESS,                                           \
                "vulkan operation failed: " + zrl::VkResultStr.at(val))

} // namespace zrl

#endif // ZRL_CORE_LOG_H
