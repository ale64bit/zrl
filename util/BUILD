load("//core:builddefs.bzl", "COPTS", "DEFINES")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["camera.h"],
    copts = COPTS,
    defines = DEFINES,
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
    deps = [
        "@glfw_repo//:glfw",
    ],
)

cc_library(
    name = "cubedata",
    hdrs = ["cubedata.h"],
    copts = COPTS,
    defines = DEFINES,
    deps = ["@glm"],
)
