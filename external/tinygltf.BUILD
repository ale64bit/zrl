cc_library(
    name = "tinygltf",
    hdrs = [
        "tiny_gltf.h",
        "stb_image.h",
        "stb_image_write.h",
    ],
    srcs = [
        "json.hpp",
        "stb_image.h",
        "stb_image_write.h",
        "tiny_gltf.h",
    ],
    # TODO(ale64bit): replace with local_defines when available.
    copts = [
        "-DTINYGLTF_NO_EXTERNAL_IMAGE",
    ],
    visibility = ["//visibility:public"],
)
