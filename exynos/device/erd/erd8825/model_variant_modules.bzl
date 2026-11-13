# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2026 The Android Open Source Project

model_variant_common_kernel_module_list = [
    # Please add model variant common kernel drivers in vendor_boot
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
]

model_variant_common_kernel_dlkm_module_list = [
    # Please add model variant common kernel drivers in vendor_dlkm
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
]

model_variant_vendor_boot_module_list = [
    # Please add model variant drivers in vendor_boot
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
]

model_variant_vendor_dlkm_module_list = [
    # Please add model variant drivers in vendor_dlkm
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
    "sound/soc/codecs/snd-soc-fs1816.ko",
    "sound/soc/codecs/aw882xx/snd-soc-smartpa-aw882xx.ko",
    "sound/soc/samsung/exynos/exynos8825_audio.ko",
    "sound/soc/samsung/exynos/sec_audio_debug.ko",
    "sound/soc/samsung/exynos/sec_audio_sysfs.ko",
    "sound/soc/samsung/exynos/abox/snd-soc-samsung-abox-sync-aw.ko",
    "drivers/leds/leds-s2mf301.ko",
    "drivers/samsung/pm/sec_wakeup_cpu_allocator.ko"
]

model_variant_external_module_list = [
    # Please add model variant external drivers in vendor_boot
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
]

model_variant_external_dlkm_module_list = [
    # Please add model variant external drivers in vendor_dlkm
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
    "is-cis-jn1.ko",
    "is-cis-4ha.ko",
    "is-cis-gc02m1.ko",
    "is-cis-imx258.ko",
    "is-actuator-dw9818.ko",
    "is-device-eeprom.ko",
    "is-device-otprom.ko",
    "is-flash-s2mf301.ko",
]

