workspace(name = "zrl")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_VULKAN_VERSION = "1.1.114.0"

http_archive(
    name = "vulkan",
    build_file = "@//external:vulkan.BUILD",
    sha256 = "796d3eedea9d2f5fd0720e5ebd9cc6072c95d5e958abea6d07b121db3973e968",
    strip_prefix = "%s/x86_64" % _VULKAN_VERSION,
    urls = ["https://sdk.lunarg.com/sdk/download/%s/linux/vulkansdk-linux-x86_64-%s.tar.gz?Human=true" % (_VULKAN_VERSION, _VULKAN_VERSION)],
)
