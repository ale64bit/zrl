_BUILD_CONTENTS_LINUX = """
cc_import(
    name = "glfw_static_lib",
    static_library = "glfw/lib/libglfw3.a",
    alwayslink = 1,
)

cc_library(
    name = "glfw",
    hdrs = glob(["glfw/include/GLFW/**/*.h"]),
    deps = [":glfw_static_lib"],
    linkopts = _LINKOPTS,
    strip_include_prefix = "glfw/include",
    visibility = ["//visibility:public"],
)
"""

_BUILD_CONTENTS_WINDOWS = """
cc_import(
    name = "glfw_static_lib",
    interface_library = "glfw/lib-vc2015/glfw3dll.lib",
    shared_library = "glfw/lib-vc2015/glfw3.dll",
    alwayslink = 1,
)

cc_library(
    name = "glfw",
    hdrs = glob(["glfw/include/GLFW/**/*.h"]),
    deps = [":glfw_static_lib"],
    linkopts = _LINKOPTS,
    strip_include_prefix = "glfw/include",
    visibility = ["//visibility:public"],
)
"""

def _glfw_repo_linux(repository_ctx):
    glfw_path = repository_ctx.os.environ["GLFW_PATH"]
    if glfw_path == "":
        fail("GLFW_PATH environment variable must be set")
    repository_ctx.symlink(glfw_path, "glfw")

    result = repository_ctx.execute(["pkg-config", "--static", "--libs", "glfw3"])
    if result.return_code:
        fail("Failed to run pkg-config for glfw3: " + result.stderr)
    linkopts = result.stdout.splitlines()[0].split(" ")
    repository_ctx.file("BUILD", ("_LINKOPTS=%s\n" % str(linkopts)) + _BUILD_CONTENTS_LINUX)

def _glfw_repo_windows(repository_ctx):
    glfw_path = repository_ctx.os.environ["GLFW_PATH"]
    if glfw_path == "":
        fail("GLFW_PATH environment variable must be set")
    repository_ctx.symlink(glfw_path, "glfw")
    repository_ctx.file("BUILD", "_LINKOPTS=[]\n" + _BUILD_CONTENTS_WINDOWS)

def _glfw_repo_impl(repository_ctx):
    os_name = repository_ctx.os.name.lower()
    if os_name.find("linux") != -1:
        _glfw_repo_linux(repository_ctx)
    elif os_name.find("windows") != -1:
        _glfw_repo_windows(repository_ctx)
    else:
        fail("Unsupported operating system: " + os_name)

glfw_repo = repository_rule(
    environ = ["GLFW_PATH"],
    local = True,
    implementation = _glfw_repo_impl,
)
