# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022 The Android Open Source Project
load(":model_variant_modules.bzl",
    "model_variant_common_kernel_module_list",
    "model_variant_common_kernel_dlkm_module_list",
    "model_variant_vendor_boot_module_list",
    "model_variant_vendor_dlkm_module_list",
    "model_variant_external_module_list",
    "model_variant_external_dlkm_module_list",
)
load("//build/kernel/kleaf:kernel.bzl", "kernel_module")
load("//exynos/soc-series/papaya-android15:s5e8825_modules.bzl",
    "get_soc_vendor_boot_modules_list",
    "get_soc_vendor_dlkm_modules_list",
    "get_soc_vendor_dlkm_blocklist",
    "get_soc_system_dlkm_blocklist",
    "get_soc_vendor_kunit_test_list",
)
load(":lego.bzl", "lego_module_list", "lego_module_list_first", "lego_module_list_second")

_FACTORY_BUILD_MODULE_LIST = [
]

# common_kernel's module. In vendor_boot.
_COMMON_KERNEL_MODULE_LIST = [
    "crypto/twofish_common.ko",
    "crypto/twofish_generic.ko",
    #"drivers/firmware/cirrus/cs_dsp.ko",
    "drivers/usb/host/xhci-sideband.ko",
]

_COMMON_KERNEL_VENDOR_DLKM_MODULE_LIST = [
    "net/wireless/cfg80211.ko",
]

# vendor_boot modules. In vendor_boot
_VENDOR_MODULE_LIST = [
    "drivers/tee/tzdev/tzdev.ko",
    "drivers/tee/tui/tuihw.ko",
    "drivers/tee/tui/tuihw-inf.ko",
    "drivers/usb/gadget/function/usb_f_conn_gadget.ko",
	"drivers/usb/gadget/function/usb_f_ss_mon_gadget.ko",
    "drivers/usb/dwc3/sec_usb_hook.ko",
    "drivers/knox/hdm/hdm.ko",
    "drivers/knox/ngksm/ngksm.ko",
    "drivers/ufs/host/ufs-sec-driver.ko",
    "drivers/mmc/host/mmc-sec-driver.ko",
]

# Only for debug/developement. In vendor_boot
_VENDOR_DEV_MODULE_LIST = [
]

# Only for debug/developement. In vendor_dlkm
_VENDOR_DEV_DLKM_MODULE_LIST = [
]

# In vendor_dlkm
_VENDOR_DLKM_MODULE_LIST = [
    "drivers/input/touchscreen/sec_ts/sec_ts.ko",
    "drivers/samsung/pm/sec_pm_debug.ko",
    "drivers/samsung/pm/sec_thermistor.ko",
    "drivers/camera/is-vendor-interface.ko",
    "block/blk-sec-common.ko",
    "block/blk-sec-stats.ko",
    "block/blk-sec-wb.ko",
    "block/ssg.ko",
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
    "sec_class.ko",
    "sec_reboot.ko",
    "sec_bootstat.ko",
    "sec_key_notifier.ko",
    "sec_crash_key_user.ko",
    "sec_hard_reset_hook.ko",
    "sec_debug_mode.ko",
    "sec_debug_base_early.ko",
    "sec_debug.ko",
    "sec_debug_extra_info.ko",
    "sec_debug_hardlockup_info.ko",
    "sec_debug_test.ko",
    "sec_debug_hw_param.ko",
    "softdog.ko",
    "sec_debug_sched_report.ko",
    "sec_debug_stacktrace.ko",
    "sec_debug_softdog.ko",
    "sec_debug_wdd_info.ko",
    "sec_debug_dprt.ko",
    "sec_debug_dtask.ko",
    "sec_debug_dprm.ko",
    "sec_debug_reset_reason.ko",
    "sec_debug_sched_info.ko",
]

_VENDOR_KUNIT_TEST_LIST = [
]

# external-modules. In vendor_dlkm
_EXTERNAL_MODULE_IN_DLKM_LIST = [
    "fimc-is.ko",
    "gdc.ko",
    "votf.ko",
    "camerapp.ko",
    "pablo-smc.ko",
]

# external-modules. In vendor_dlkm
_EXTERNAL_MODULE_BOOT_COMPLETE_IN_DLKM_LIST = [
]

def get_erd8825_kunit_test_list(variant = None):
    modules_list = []
    if variant != "user" and variant != "user.fac":
        modules_list += _VENDOR_KUNIT_TEST_LIST
    return modules_list

def get_erd8825_system_dlkm_blocklist():
    modules_list = []
    modules_list += _SYSTEM_DLKM_BLOCKLIST
    return modules_list

def get_erd8825_vendor_dlkm_blocklist():
    modules_list = []
    modules_list += _VENDOR_DLKM_BLOCKLIST
    return modules_list

def get_erd8825_vendor_dlkm_modules_list(variant = None):
    modules_list = []
    modules_list += _VENDOR_DLKM_MODULE_LIST
    modules_list += _VENDOR_DLKM_BOOT_COMPLETE_MODULE_LIST
    if variant != "user" and variant != "user.fac":
        modules_list += _VENDOR_DEV_DLKM_MODULE_LIST
    modules_list += get_erd8825_vendor_dlkm_blocklist()
    modules_list += model_variant_vendor_dlkm_module_list
    modules_list += lego_module_list_second
    return modules_list

# All modules built from common_kernel
def get_common_kernel_modules_list():
    modules_list = []
    modules_list += _COMMON_KERNEL_MODULE_LIST
    modules_list += _COMMON_KERNEL_VENDOR_DLKM_MODULE_LIST
    modules_list += model_variant_common_kernel_module_list
    modules_list += model_variant_common_kernel_dlkm_module_list
    return modules_list

def get_erd8825_vendor_boot_modules_list(variant = None):
    modules_list = []
    modules_list += _VENDOR_MODULE_LIST
    if variant != "user" and variant != "user.fac":
        modules_list += _VENDOR_DEV_MODULE_LIST
    if "fac" in variant:
        modules_list += _FACTORY_BUILD_MODULE_LIST
    modules_list += model_variant_vendor_boot_module_list
    modules_list += lego_module_list
    modules_list += lego_module_list_first
    return modules_list

# All modules included in vendor boot
def get_all_vendor_boot_modules_list(variant = None):
    modules_list = []
    modules_list += get_erd8825_vendor_boot_modules_list(variant)
    modules_list += get_soc_vendor_boot_modules_list(variant)
    modules_list += _COMMON_KERNEL_MODULE_LIST
    modules_list += _EXTERNAL_MODULE_LIST
    modules_list += model_variant_external_module_list
    modules_list += model_variant_common_kernel_module_list
    return modules_list

def get_all_system_dlkm_blocklist():
    modules_list = []
    modules_list += get_soc_system_dlkm_blocklist()
    modules_list += get_erd8825_system_dlkm_blocklist()
    return modules_list

# All vendor_dlkm blocklist included in vendor_boot
def get_all_vendor_dlkm_blocklist():
    modules_list = []
    modules_list += get_soc_vendor_dlkm_blocklist()
    modules_list += get_erd8825_vendor_dlkm_blocklist()
    return modules_list

def get_all_kunit_test_list(variant = None):
    modules_list = []
    modules_list += get_soc_vendor_kunit_test_list(variant)
    modules_list += get_erd8825_kunit_test_list(variant)
    return modules_list

# All modules included in soc_vendor_dlkm
def get_all_vendor_dlkm_modules_list(variant = None):
    modules_list = []
    modules_list += _COMMON_KERNEL_VENDOR_DLKM_MODULE_LIST
    modules_list += get_soc_vendor_dlkm_modules_list(variant)
    modules_list += get_erd8825_vendor_dlkm_modules_list(variant)
    modules_list += _EXTERNAL_MODULE_IN_DLKM_LIST
    modules_list += model_variant_external_dlkm_module_list
    modules_list += model_variant_common_kernel_dlkm_module_list
    return modules_list

# All modules built from this directory
def get_erd8825_vendor_modules_list(variant = None):
    modules_list = []
    modules_list += get_erd8825_vendor_boot_modules_list(variant)
    modules_list += get_erd8825_vendor_dlkm_modules_list(variant)
    modules_list += get_erd8825_kunit_test_list(variant)
    return modules_list

def define_erd8825_module(variant = None):
    MODULE_OUTS = get_erd8825_vendor_modules_list(variant)
    if MODULE_OUTS:
        kernel_module(
            name = "erd8825_module.{}".format(variant),
             srcs = [
                ":exynos.erd8825_sources",
                "//exynos/soc-series/common:exynos_soc_header",
                "//exynos/soc-series/papaya-android15:exynos.soc_sources",
                "//exynos/external-modules/bsp/class:sec_class_fvc_headers",
                "//exynos/external-modules/bsp/reboot/6.6:sec_reboot_fvc_headers",
                "//exynos/external-modules/bsp/debug/6.6_fvc:sec_debug_headers",
                "//exynos/external-modules/camera:camera_header",
            ],
            outs = MODULE_OUTS,
            deps = [
                "//exynos/soc-series/common:exynos_soc_module",
                "//exynos/external-modules/bsp/class:sec_class_fvc",
                "//exynos/external-modules/bsp/reboot/6.6:sec_reboot_fvc",
                "//exynos/external-modules/bsp/debug/6.6_fvc:sec_debug_modules",
                ":camera_modules",
            ],
            kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
            visibility = [
                "//exynos/device:__subpackages__",
                "//exynos/external-modules:__subpackages__",
            ],
        )

def register_erd8825_module():
    define_erd8825_module("eng")
    define_erd8825_module("userdebug")
    define_erd8825_module("user")
    define_erd8825_module("user.fac")
