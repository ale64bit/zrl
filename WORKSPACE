workspace(name = "zrl")

load("//repo_tools:zrlc.bzl", "zrlc_repo")

zrlc_repo(name = "zrlc_repo")

load("//repo_tools:glfw.bzl", "glfw_repo")

glfw_repo(name = "glfw_repo")

load("//repo_tools:vulkan.bzl", "vulkan_repo")

vulkan_repo(name = "vulkan_repo")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "glm",
    build_file = "glm.BUILD",
    sha256 = "5e33b6131cea6a904339734b015110d4342b7dc02d995164fdb86332d28a5aa4",
    strip_prefix = "glm-0.9.9.5",
    url = "https://github.com/g-truc/glm/archive/0.9.9.5.tar.gz",
)
