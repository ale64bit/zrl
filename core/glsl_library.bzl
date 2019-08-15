def _glsl_library_internal_impl(ctx):
    out_files = []
    single_hdr_lines = ["#pragma once"]
    for src in ctx.files.srcs:
        parts = src.basename.split(".")
        if len(parts) != 3:
            fail("shader file names must be in the form <name>.<stage>.glsl")
        name = "%s%s" % (parts[0], parts[1].capitalize())
        fname = name + ".h"
        outf = ctx.actions.declare_file(fname)
        out_files.append(outf)
        single_hdr_lines.append('#include "%s" ' % outf.short_path)
        ctx.actions.run(
            inputs = [src],
            outputs = [outf],
            executable = ctx.executable.compiler,
            progress_message = "compiling " + src.basename,
            arguments = [
                "-V",
                "--vn",
                "k" + name,
                src.path,
                "-o",
                outf.path,
            ],
        )
    single_hdr_name = ctx.attr.name + ".h"
    single_hdr = ctx.actions.declare_file(single_hdr_name)
    ctx.actions.write(single_hdr, "\n".join(single_hdr_lines))
    out_files.append(single_hdr)
    compilation_context = cc_common.create_compilation_context(
        headers = depset(out_files),
    )
    return [
        DefaultInfo(files = depset(out_files)),
        CcInfo(compilation_context = compilation_context),
    ]

_glsl_library_internal = rule(
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
        ),
        "compiler": attr.label(
            cfg = "host",
            allow_single_file = True,
            executable = True,
        ),
    },
    implementation = _glsl_library_internal_impl,
)

def glsl_library(name, srcs):
    _glsl_library_internal(
        name = name,
        srcs = srcs,
        compiler = "@vulkan_repo//:glsl_compiler",
    )
