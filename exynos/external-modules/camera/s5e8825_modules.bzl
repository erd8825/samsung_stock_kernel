# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2024 The Android Open Source Project
load("//build/kernel/kleaf:kernel.bzl", "kernel_module")
load(":model_variant_modules.bzl", "erd8825_camera_module_outs")

def define_s5e8825_soc_module():
    module_outs = [
        "camera/fimc-is.ko",
        "camera/camera-pp/gdc/gdc.ko",
        "camera/camera-pp/votf/votf.ko",
        "camera/camera-pp/camerapp.ko",
        "camera/pablo-smc.ko",
    ]

    kernel_module(
        name = "s5e8825_camera_user",
        srcs = [
            ":camera_src",
            "//exynos/soc-series/common:exynos_soc_header",
        ],
        outs = module_outs + erd8825_camera_module_outs,
        kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
        visibility = [ "//exynos/device:__subpackages__", ],
        deps = [ "//exynos/soc-series/common:exynos_soc_module" ],
    )

    kernel_module(
        name = "s5e8825_camera",
        srcs = [
            ":camera_src",
            "//exynos/soc-series/common:exynos_soc_header",
        ],
        outs = module_outs + erd8825_camera_module_outs,
        kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
        visibility = [ "//exynos/device:__subpackages__", ],
        deps = [ "//exynos/soc-series/common:exynos_soc_module" ],
    )

def register_s5e8825_soc_module():
    define_s5e8825_soc_module()
