_WINDOWS_LINKOPTS = [
    # none required
]

_LINUX_LINKOPTS = [
    "-lrt",
    "-lm",
    "-ldl",
    "-lX11",
    "-pthread",
    "-lxcb",
    "-lXau",
    "-lXdmcp",
]

cc_import(
  name = "glfw_win",
  interface_library = "lib-vc2015/glfw3dll.lib",
  shared_library = "lib-vc2015/glfw3.dll",
  alwayslink = 1,
)

cc_import(
  name = "glfw_linux",
  static_library = "src/libglfw3.a",
  alwayslink = 1,
)

cc_library(
  name = "glfw_lib",
  hdrs = glob(["include/**/*.h"]),
  linkopts = select({
      "@bazel_tools//src/conditions:windows": _WINDOWS_LINKOPTS,
      "//conditions:default": _LINUX_LINKOPTS,
  }),
  deps = select({
      "@bazel_tools//src/conditions:windows": [":glfw_win"],
      "//conditions:default": [":glfw_linux"],
  }),
  strip_include_prefix = "include",
  visibility = ["//visibility:public"],
)

