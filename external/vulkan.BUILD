exports_files(
  ["Bin/glslangValidator.exe", "bin/glslangValidator"]
)

cc_library(
  name = "sdk",
  srcs = select({
    "@bazel_tools//src/conditions:windows": ["Lib/vulkan-1.lib"],
    "//conditions:default": ["lib/libvulkan.so"],
  }),
  hdrs = select({
    "@bazel_tools//src/conditions:windows": glob(["Include/**/*.h"]),
    "//conditions:default": glob(["include/**/*.h"]),
  }),
  strip_include_prefix = select({
    "@bazel_tools//src/conditions:windows": "Include",
    "//conditions:default": "include",
  }),
  visibility = ["//visibility:public"],
)

