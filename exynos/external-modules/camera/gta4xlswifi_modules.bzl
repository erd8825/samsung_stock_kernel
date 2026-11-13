# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2026 The Android Open Source Project

erd8825_camera_module_outs = [
    # Please add to model variant drivers
    # Chipset common driver have to add to {PROJECT_NAME}_modules.bzl
    "camera/sensor/module_framework/cis/is-cis-4ha.ko",
    "camera/sensor/module_framework/cis/is-cis-gc5035.ko",
    "camera/sensor/module_framework/actuator/is-actuator-dw9808.ko",
    "camera/vendor/mcd_v2/is-device-otprom.ko",
    "camera/vendor/mcd_v2/is-device-eeprom.ko",
]