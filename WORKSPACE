workspace(name = "zrl")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_GLFW_VERSION = "3.2.1"

_VULKAN_VERSION = "1.1.114.0"

http_archive(
    name = "glfw",
    build_file = "@//external:glfw.BUILD",
    sha256 = "e10f0de1384d75e6fc210c53e91843f6110d6c4f3afbfb588130713c2f9d8fe8",
    strip_prefix = "glfw-%s" % _GLFW_VERSION,
    urls = ["https://github.com/glfw/glfw/archive/%s.tar.gz" % _GLFW_VERSION],
)

http_archive(
    name = "vulkan",
    build_file = "@//external:vulkan.BUILD",
    sha256 = "796d3eedea9d2f5fd0720e5ebd9cc6072c95d5e958abea6d07b121db3973e968",
    strip_prefix = "%s/x86_64" % _VULKAN_VERSION,
    urls = ["https://sdk.lunarg.com/sdk/download/%s/linux/vulkansdk-linux-x86_64-%s.tar.gz?Human=true" % (_VULKAN_VERSION, _VULKAN_VERSION)],
)
