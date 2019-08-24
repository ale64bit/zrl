load("//core:builddefs.bzl", "COPTS", "DEFINES")
load("//core:zrl_library.bzl", "zrl_library")

zrl_library(
    name = "pbr",
    src = "pbr.zrl",
)

cc_binary(
    name = "main",
    srcs = [
        "gltf_binders.h",
        "main.cc",
    ],
    copts = COPTS,
    defines = DEFINES + [
        "STB_IMAGE_IMPLEMENTATION",
        "STB_IMAGE_WRITE_IMPLEMENTATION",
        "TINYGLTF_IMPLEMENTATION",
        "ZRL_FRONT_FACE_COUNTER_CLOCKWISE",
    ],
    deps = [
        ":pbr",
        "//core",
        "//util:camera",
        "@glm",
        "@tinygltf",
        "@vulkan_repo//:sdk",
    ],
)
