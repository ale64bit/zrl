_BUILD_CONTENTS_LINUX = """
filegroup(
  name = "glsl_compiler",
  srcs = ["vulkan/bin/glslangValidator"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "sdk",
  srcs = ["vulkan/lib/libvulkan.so"],
  hdrs = glob(["vulkan/include/**/*.h"]),
  strip_include_prefix = "vulkan/include",
  visibility = ["//visibility:public"],
)
"""

_BUILD_CONTENTS_WINDOWS = """
filegroup(
  name = "glsl_compiler",
  srcs = ["vulkan/Bin/glslangValidator.exe"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "sdk",
  srcs = ["vulkan/Lib/vulkan-1.lib"],
  hdrs = glob(["vulkan/Include/**/*.h"]),
  strip_include_prefix = "vulkan/Include",
  visibility = ["//visibility:public"],
)
"""

def _vulkan_repo_impl(repository_ctx):
    os_name = repository_ctx.os.name.lower()
    env_cmd = []
    build_contents = ""
    if os_name.find("linux") != -1:
        env_cmd = ["printenv", "VULKAN_SDK_PATH"]
        build_contents = _BUILD_CONTENTS_LINUX
    elif os_name.find("windows") != -1:
        env_cmd = ["echo", "$Env:VULKAN_SDK_PATH"]
        build_contents = _BUILD_CONTENTS_WINDOWS
    else:
        fail("Unsupported operating system: " + os_name)

    result = repository_ctx.execute(env_cmd)
    if result.return_code:
        fail("VULKAN_SDK_PATH environment variable must be set")
    vulkan_path = result.stdout.splitlines()[0]
    if vulkan_path == "":
        fail("VULKAN_SDK_PATH environment variable must be set")
    repository_ctx.symlink(vulkan_path, "vulkan")
    repository_ctx.file("BUILD", build_contents)

vulkan_repo = repository_rule(
    environ = ["VULKAN_SDK_PATH"],
    local = True,
    implementation = _vulkan_repo_impl,
)
