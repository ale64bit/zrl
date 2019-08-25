load("//core:builddefs.bzl", "COPTS", "DEFINES")
load("//core:zrl_library.bzl", "zrl_library")

zrl_library(
    name = "texturedcube",
    src = "texturedcube.zrl",
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    data = ["//assets"],
    defines = DEFINES + [
        "STB_IMAGE_IMPLEMENTATION",
    ],
    deps = [
        ":texturedcube",
        "//core",
        "@glfw_repo//:glfw",
        "@glm",
        "@stb//:stb_image",
        "@vulkan_repo//:sdk",
    ],
)
