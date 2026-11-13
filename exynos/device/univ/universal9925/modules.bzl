# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022 The Android Open Source Project
load("//build/kernel/kleaf:kernel.bzl", "kernel_module")
load("//exynos/soc-series/pamir-android15:s5e9925_modules.bzl",
    "get_soc_vendor_boot_modules_list",
    "get_soc_vendor_dlkm_modules_list",
    "get_soc_vendor_dlkm_blocklist",
    "get_soc_system_dlkm_blocklist",
    "get_soc_vendor_kunit_test_list",
)

# common_kernel's module. In vendor_boot.
_COMMON_KERNEL_MODULE_LIST = [
    "drivers/gpu/drm/display/drm_display_helper.ko",
    "drivers/gpu/drm/drm_ttm_helper.ko",
    "drivers/gpu/drm/scheduler/gpu-sched.ko",
    "drivers/firmware/cirrus/cs_dsp.ko",
    "drivers/hwmon/hwmon.ko",
    "drivers/i2c/algos/i2c-algo-bit.ko",
    "drivers/usb/host/xhci-sideband.ko",
]

_COMMON_KERNEL_VENDOR_DLKM_MODULE_LIST = [
]

# vendor_boot modules. In vendor_boot
_VENDOR_MODULE_LIST = [
	"drivers/samsung/sec_mparam.ko",
	"drivers/mfd/max77705/mfd_max77705.ko",
	"drivers/staging/android/switch/switch_class.ko",
	"drivers/samsung/sec_class.ko",
	"drivers/samsung/sec_param.ko",
	"drivers/samsung/misc/vbus_notifier.ko",
	"drivers/battery/charger/max77705_charger.ko",
	"drivers/battery/charger/pca9468_charger.ko",
	"drivers/battery/wireless/mfc_charger.ko",
	"drivers/battery/common/sec_direct_charger.ko",
	"drivers/battery/common/sec_battery_m.ko",
	"drivers/battery/fuelgauge/max77705_fuelgauge.ko",
	"drivers/usb/typec/maxim/max77705/pdic_max77705.ko",
	"drivers/usb/typec/manager/usb_typec_manager_notifier.ko",
	"drivers/usb/notify/usb_notify_layer.ko",
	"drivers/usb/notify/usb_notifier.ko",
	"drivers/muic-mcd/common_muic.ko",
        "drivers/battery/common/sec_pd.ko",
        "drivers/usb/typec/common/pdic_notifier_module.ko",
        "drivers/usb/typec/manager/if_cb_manager.ko",
]

# Only for debug/developement. In vendor_boot
_VENDOR_DEV_MODULE_LIST = [
]

# Only for debug/developement. In vendor_dlkm
_VENDOR_DEV_DLKM_MODULE_LIST = [

]

# In vendor_dlkm
_VENDOR_DLKM_MODULE_LIST = [
    "drivers/input/touchscreen/sec_input/sec_common_fn.ko",
    "drivers/input/touchscreen/sec_input/sec_cmd.ko",
    "drivers/input/touchscreen/stm/stm_spi/stm_ts_spi.ko",
]

# In vendor_dlkm
_VENDOR_DLKM_BOOT_COMPLETE_MODULE_LIST = [
]

# In system_dlkm blocklist
_SYSTEM_DLKM_BLOCKLIST = [
    # For system_dlkm blocklist
]

# In vendor_dlkm blocklist
_VENDOR_DLKM_BLOCKLIST = [
    # For vendor_dlkm blocklist
]

# external-modules. In vendor_boot
_EXTERNAL_MODULE_LIST = [
    "sgpu.ko",
]

_VENDOR_KUNIT_TEST_LIST = [
]

# external-modules. In vendor_dlkm
_EXTERNAL_MODULE_IN_DLKM_LIST = [

]

# external-modules. In vendor_dlkm
_EXTERNAL_MODULE_BOOT_COMPLETE_IN_DLKM_LIST = [
]
def get_univ9925_kunit_test_list(variant = None):
    modules_list = []
    if variant != "user":
        modules_list += _VENDOR_KUNIT_TEST_LIST
    return modules_list

def get_univ9925_system_dlkm_blocklist():
    modules_list = []
    modules_list += _SYSTEM_DLKM_BLOCKLIST
    return modules_list

def get_univ9925_vendor_dlkm_blocklist():
    modules_list = []
    modules_list += _VENDOR_DLKM_BLOCKLIST
    return modules_list

def get_univ9925_vendor_dlkm_modules_list(variant = None):
    modules_list = []
    modules_list += _VENDOR_DLKM_MODULE_LIST
    modules_list += _VENDOR_DLKM_BOOT_COMPLETE_MODULE_LIST
    modules_list += get_univ9925_vendor_dlkm_blocklist()
    if variant != "user":
        modules_list += _VENDOR_DEV_DLKM_MODULE_LIST
    return modules_list

# All modules built from common_kernel
def get_common_kernel_modules_list():
    modules_list = []
    modules_list += _COMMON_KERNEL_MODULE_LIST
    modules_list += _COMMON_KERNEL_VENDOR_DLKM_MODULE_LIST
    return modules_list


def get_univ9925_vendor_boot_modules_list(variant = None):
    modules_list = []
    modules_list += _VENDOR_MODULE_LIST
    if variant != "user":
        modules_list += _VENDOR_DEV_MODULE_LIST
    return modules_list


# All modules included in vendor boot
def get_all_vendor_boot_modules_list(variant = None):
    modules_list = []
    modules_list += get_univ9925_vendor_boot_modules_list(variant)
    modules_list += get_soc_vendor_boot_modules_list(variant)
    modules_list += _COMMON_KERNEL_MODULE_LIST
    modules_list += _EXTERNAL_MODULE_LIST
    return modules_list

def get_all_system_dlkm_blocklist():
    modules_list = []
    modules_list += get_soc_system_dlkm_blocklist()
    modules_list += get_univ9925_system_dlkm_blocklist()
    return modules_list

# All vendor_dlkm blocklist included in vendor_boot
def get_all_vendor_dlkm_blocklist():
    modules_list = []
    modules_list += get_soc_vendor_dlkm_blocklist()
    modules_list += get_univ9925_vendor_dlkm_blocklist()
    return modules_list

def get_all_kunit_test_list(variant = None):
    modules_list = []
    modules_list += get_soc_vendor_kunit_test_list(variant)
    modules_list += get_univ9925_kunit_test_list(variant)
    return modules_list


# All modules included in soc_vendor_dlkm
def get_all_vendor_dlkm_modules_list(variant = None):
    modules_list = []
    modules_list += _COMMON_KERNEL_VENDOR_DLKM_MODULE_LIST
    modules_list += get_soc_vendor_dlkm_modules_list(variant)
    modules_list += get_univ9925_vendor_dlkm_modules_list(variant)
    modules_list += _EXTERNAL_MODULE_IN_DLKM_LIST
    return modules_list

# All modules built from this directory
def get_univ9925_vendor_modules_list(variant = None):
    modules_list = []
    modules_list += get_univ9925_vendor_boot_modules_list(variant)
    modules_list += get_univ9925_vendor_dlkm_modules_list(variant)
    modules_list += get_univ9925_kunit_test_list(variant)
    return modules_list

def define_univ9925_module(variant = None):
    MODULE_OUTS = get_univ9925_vendor_modules_list(variant)
    if MODULE_OUTS:
        kernel_module(
            name = "universal9925_module.{}".format(variant),
             srcs = [
                ":exynos.universal9925_sources",
		"//exynos/soc-series/common:exynos_soc_header",
            ],
            outs = MODULE_OUTS,
            kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
            visibility = [
                "//exynos/device:__subpackages__",
            ],
	    deps = ["//exynos/soc-series/common:exynos_soc_module"],
        )

def register_univ9925_module():
    define_univ9925_module("eng")
    define_univ9925_module("userdebug")
    define_univ9925_module("user")
