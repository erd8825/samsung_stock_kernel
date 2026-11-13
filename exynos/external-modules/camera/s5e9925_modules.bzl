# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2024 The Android Open Source Project
load("//build/kernel/kleaf:kernel.bzl", "kernel_module")

def define_s5e9925_soc_module():
    module_outs = [
        "camera/fimc-is.ko",
        "camera/sensor/module_framework/cis/is-cis-gn3.ko",
        "camera/sensor/module_framework/cis/is-cis-2ld.ko",
        "camera/sensor/module_framework/cis/is-cis-3k1.ko",
        "camera/sensor/module_framework/cis/is-cis-2la.ko",
        "camera/sensor/module_framework/cis/is-cis-imx374.ko",
        "camera/sensor/module_framework/actuator/is-actuator-ak737x.ko",
        "camera/camera-pp/gdc/gdc.ko",
        "camera/camera-pp/votf/votf.ko",
        "camera/camera-pp/camerapp.ko",
    ]

    kernel_module(
        name = "s5e9925_camera_user",
        srcs = [
            ":camera_src",
            "//exynos/soc-series/common:exynos_soc_header",
        ],
        outs = module_outs,
        kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
        visibility = [ "//exynos/device:__subpackages__", ],
        deps = [ "//exynos/soc-series/common:exynos_soc_module" ],
    )

    kernel_module(
        name = "s5e9925_camera",
        srcs = [
            ":camera_src",
            "//exynos/soc-series/common:exynos_soc_header",
        ],
        outs = module_outs,
        kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
        visibility = [ "//exynos/device:__subpackages__", ],
        deps = [ "//exynos/soc-series/common:exynos_soc_module" ],
    )

def register_s5e9925_soc_module():
    define_s5e9925_soc_module()
