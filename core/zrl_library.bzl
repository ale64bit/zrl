load("//core:builddefs.bzl", "COPTS", "DEFINES")

def _zrl_library_internal_impl(ctx):
    include_tree = ctx.actions.declare_directory("include")
    src_tree = ctx.actions.declare_directory("src")
    shader_tree = ctx.actions.declare_directory("shader")
    gendir = shader_tree.path[:-7]
    for src in ctx.files.src:
        ctx.actions.run(
            inputs = [src],
            outputs = [
                include_tree,
                src_tree,
                shader_tree,
            ],
            executable = ctx.executable._zrl_compiler,
            progress_message = "compiling " + src.basename,
            arguments = [
                "-v",
                "-i",
                src.path,
                "-o",
                gendir,
            ],
        )

    shader_include_tree = ctx.actions.declare_directory("shader_include")

    # TODO(ale64bit): implement the equivalent Windows command.
    compile_shaders_cmd = 'for i in %s/*.glsl; do %s -V "$i" --vn "k$(basename $i | tr . _)_" -o "%s/$(basename $i).h"; done' % (shader_tree.path, ctx.executable._glsl_compiler.path, shader_include_tree.path)
    ctx.actions.run_shell(
        inputs = [shader_tree],
        outputs = [shader_include_tree],
        tools = [ctx.executable._glsl_compiler],
        command = compile_shaders_cmd,
    )

    gen_tree = ctx.actions.declare_directory("gen.cc")

    # TODO(ale64bit): implement the equivalent Windows command.
    collect_sources_cmd = 'for i in %s/*.cc %s/*.h %s/*.h; do cp "$i" "%s/$(basename $i)"; done' % (src_tree.path, include_tree.path, shader_include_tree.path, gen_tree.path)
    ctx.actions.run_shell(
        inputs = [shader_include_tree, include_tree, src_tree],
        outputs = [gen_tree],
        command = collect_sources_cmd,
    )

    return [DefaultInfo(files = depset([gen_tree]))]

_zrl_library_internal = rule(
    attrs = {
        "src": attr.label(
            allow_single_file = True,
            mandatory = True,
        ),
        "_zrl_compiler": attr.label(
            cfg = "host",
            allow_single_file = True,
            executable = True,
            default = "@zrlc_repo//:compiler",
        ),
        "_glsl_compiler": attr.label(
            cfg = "host",
            allow_single_file = True,
            executable = True,
            default = "@vulkan_repo//:glsl_compiler",
        ),
    },
    implementation = _zrl_library_internal_impl,
)

def zrl_library(name, src):
    gen_name = "_gen_" + name
    _zrl_library_internal(
        name = gen_name,
        src = src,
    )
    native.cc_library(
        name = name,
        srcs = [gen_name],
        hdrs = [gen_name],
        copts = COPTS,
        defines = DEFINES,
        deps = ["//core"],
    )
