_WINDOWS_COPTS = [
    "/std:c++14",
    "/DNOMIXMAX",
    "/Wall",
]

_LINUX_COPTS = [
    "--std=c++14",
    "-Wall",
    "-Wextra",
]

COPTS = select({
    "@bazel_tools//src/conditions:windows": _WINDOWS_COPTS,
    "//conditions:default": _LINUX_COPTS,
})

DEFINES = [
    "GLM_ENABLE_EXPERIMENTAL",
    "GLM_FORCE_DEPTH_ZERO_TO_ONE",
    "GLM_FORCE_LEFT_HANDED",
    "GLM_FORCE_RADIANS",
]
