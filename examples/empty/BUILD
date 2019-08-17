load("//core:builddefs.bzl", "COPTS", "DEFINES")
load("//core:zrl_library.bzl", "zrl_library")

zrl_library(
    name = "empty",
    src = "empty.zrl",
)

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    defines = DEFINES,
    deps = [
        ":empty",
        "//core",
        "@vulkan_repo//:sdk",
    ],
)
