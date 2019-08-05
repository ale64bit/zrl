load("//core:builddefs.bzl", "COPTS", "DEFINES")

cc_library(
    name = "core",
    srcs = [
        "Buffer.cc",
        "BufferPool.cc",
        "Core.cc",
        "Image.cc",
        "LogicalDevice.cc",
        "PhysicalDevice.cc",
        "StagingBuffer.cc",
        "Swapchain.cc",
    ],
    hdrs = [
        "Buffer.h",
        "BufferPool.h",
        "Constants.h",
        "Core.h",
        "Image.h",
        "LRU.h",
        "Log.h",
        "LogicalDevice.h",
        "PhysicalDevice.h",
        "StagingBuffer.h",
        "Swapchain.h",
    ],
    copts = COPTS,
    defines = DEFINES,
    visibility = ["//visibility:public"],
    deps = [
        "@glfw//:glfw_lib",
        "@vulkan//:sdk",
    ],
)
