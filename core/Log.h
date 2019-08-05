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
