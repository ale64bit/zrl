load("//core:builddefs.bzl", "COPTS", "DEFINES")

cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["camera.h"],
    copts = COPTS,
    defines = DEFINES,
    visibility = ["//visibility:public"],
    deps = [
        "@glm",
    ],
)

cc_library(
    name = "fps_counter",
    srcs = ["fps_counter.cc"],
    hdrs = ["fps_counter.h"],
    copts = COPTS,
    defines = DEFINES,
    visibility = ["//visibility:public"],
    deps = [
        "@glfw_repo//:glfw",
    ],
)
