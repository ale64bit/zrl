load("//core:builddefs.bzl", "COPTS", "DEFINES")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    defines = DEFINES,
    deps = [
        "//core",
        "@vulkan_repo//:sdk",
    ],
)
