def _zrlc_repo_impl(repository_ctx):
    zrlc_path = repository_ctx.os.environ["ZRLC_PATH"]
    if zrlc_path == "":
        fail("ZRLC_PATH environment variable must be set")
    repository_ctx.symlink(zrlc_path, "zrlc")
    repository_ctx.file("BUILD", """
filegroup(
    name = "compiler",
    srcs = ["zrlc/zrlc.exe"],
    visibility = ["//visibility:public"],
)""")

zrlc_repo = repository_rule(
    environ = ["ZRLC_PATH"],
    local = True,
    implementation = _zrlc_repo_impl,
)
