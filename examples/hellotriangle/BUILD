load("//core:builddefs.bzl", "COPTS", "DEFINES")
load("//core:zrl_library.bzl", "zrl_library")

zrl_library(
    name = "hellotriangle",
    src = "hellotriangle.zrl",
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    defines = DEFINES,
    deps = [
        ":hellotriangle",
        "//core",
        "@glfw_repo//:glfw",
        "@glm",
        "@vulkan_repo//:sdk",
    ],
)
