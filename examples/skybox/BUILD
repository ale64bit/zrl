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
    data = ["//assets"],
    defines = DEFINES + [
        "STB_IMAGE_IMPLEMENTATION",
        "ZRL_FRONT_FACE_COUNTER_CLOCKWISE",
    ],
    deps = [
        ":skybox",
        "//core",
        "//util:camera",
        "//util:cubedata",
        "@glfw_repo//:glfw",
        "@glm",
        "@stb//:stb_image",
        "@vulkan_repo//:sdk",
    ],
)
