def _zrlc_repo_impl(repository_ctx):
    os_name = repository_ctx.os.name.lower()
    env_cmd = []
    if os_name.find("linux") != -1:
        env_cmd = ["printenv", "ZRLC_PATH"]
    elif os_name.find("windows") != -1:
        env_cmd = ["echo", "$Env:ZRLC_PATH"]
    else:
        fail("Unsupported operating system: " + os_name)

    result = repository_ctx.execute(env_cmd)
    if result.return_code:
        fail("ZRLC_PATH environment variable must be set")
    zrlc_path = result.stdout.splitlines()[0]
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
