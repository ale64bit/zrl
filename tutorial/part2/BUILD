load("//core:builddefs.bzl", "COPTS", "DEFINES", "LINKOPTS")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    defines = DEFINES,
    deps = [
        "//core",
        "@vulkan//:sdk",
    ],
)
