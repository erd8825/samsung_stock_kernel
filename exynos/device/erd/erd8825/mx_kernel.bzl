# SPDX-License-Identifier: GPL-2.0
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")
load("@rules_pkg//pkg:mappings.bzl", "pkg_files")

def kernel_output(board, page_size="4k"):
    if page_size == "4k":
        suffix = ""
    elif page_size == "16k":
        suffix = "_16k"

    pkg_files(
        name = "{}{}_dist_debug_files".format(board, suffix),
        srcs = [
            "//common:kernel_aarch64{}/System.map".format(suffix),
            "//common:kernel_aarch64{}/vmlinux".format(suffix),
            "//common:kernel_aarch64{}_config".format(suffix),
            "//common:kernel_aarch64{}_gki_artifacts".format(suffix),
            ":{}{}".format(board, suffix),
            ":{}{}_ext_modules".format(board, suffix),
            ":{}_module_symbols".format(board, suffix),
            ":{}{}_images".format(board, suffix),
        ],
        strip_prefix = "",
    )

    pkg_tar(
        name = "out_{}{}_debug".format(board, suffix),
        srcs = [":{}{}_dist_debug_files".format(board, suffix)],
    )

def mx_kernel_rules(board, platform_version):
    native.filegroup(
        name = "{}.dt-bindings".format(board),
        srcs = native.glob([
            "include/dt-bindings/**",
            "include/dtc/**",
            ],
            allow_empty = True,
        ),
        visibility = [
            "//exynos/device:__subpackages__",
        ],
    )

    kernel_output(board)
    kernel_output(board, "16k")

