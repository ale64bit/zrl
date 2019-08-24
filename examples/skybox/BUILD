load("//core:builddefs.bzl", "COPTS", "DEFINES")
load("//core:zrl_library.bzl", "zrl_library")

zrl_library(
    name = "skybox",
    src = "skybox.zrl",
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    data = ["//assets:images"],
    defines = DEFINES + [
        "STB_IMAGE_IMPLEMENTATION",
    ],
    deps = [
        ":skybox",
        "//core",
        "//util:camera",
        "@glfw_repo//:glfw",
        "@glm",
        "@stb//:stb_image",
        "@vulkan_repo//:sdk",
    ],
)
