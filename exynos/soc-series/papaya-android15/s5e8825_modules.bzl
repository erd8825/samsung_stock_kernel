# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2026 The Android Open Source Project
load("//exynos/device/erd/erd8825:model_variant_audio_modules.bzl","support_vts", "support_radio")
load("//build/kernel/kleaf:kernel.bzl", "kernel_module")

_VENDOR_EARLY_MODULE_LIST = [
   "drivers/clocksources/exynos_mct.ko",
   "drivers/soc/samsung/exynos/exynos-chipid_v2.ko",
   "drivers/power/reset/exynos/exynos-reboot.ko",
   "drivers/watchdog/s3c2410_wdt_ext.ko",
]

_VENDOR_MODULE_LIST = [
    # No need to sort, but should not be duplicated
    "drivers/pinctrl/samsung/pinctrl-samsung-ext.ko",
    "drivers/tty/serial/exynos_tty.ko",
    "drivers/i2c/busses/i2c-exynos5.ko",
    "drivers/gpu/arm/v_r49p1/mali_kbase.ko",
    "drivers/i3c/i3c-hci-exynos.ko",
    "drivers/spi/spi-s3c64xx-ext.ko",
    "drivers/pwm/pwm-exynos.ko",
    "drivers/soc/samsung/exynos/exynos-usi_v2.ko",
    "drivers/soc/samsung/exynos/exynos-cpuhp.ko",
    "drivers/soc/samsung/exynos/exynos-cpupm.ko",
    "kernel/sched/ems/ems.ko",
    "drivers/soc/samsung/xperf/xperf.ko",
    "drivers/iommu/samsung/samsung_iommu.ko",
    "drivers/iommu/samsung/samsung-iommu-group.ko",
    "drivers/iommu/samsung/samsung-secure-iova.ko",
    "drivers/dma-buf/heaps/samsung/samsung_dma_heap.ko",
    "drivers/clk/samsung/clk_exynos.ko",
    "drivers/soc/samsung/exynos/cal-if/cmupmucal.ko",
    "drivers/soc/samsung/exynos/acpm/plugin_dbg.ko",
    "drivers/soc/samsung/exynos/acpm/exynos_acpm.ko",
    "drivers/soc/samsung/exynos/acpm/exynos_acpm_mfd_bus.ko",
    "drivers/soc/samsung/exynos/acpm/flexpmu_dbg.ko",
    "drivers/soc/samsung/exynos/exynos-pmu-if.ko",
    "drivers/soc/samsung/exynos/exynos-pd/exynos-pd.ko",
    "drivers/soc/samsung/exynos/exynos-pd/exynos-pd_el3.ko",
    "drivers/soc/samsung/exynos/exynos-pd/exynos-pd-dbg.ko",
    "drivers/soc/samsung/exynos/ect_parser/ect_parser.ko",
    "drivers/soc/samsung/exynos/dm/exynos-dm.ko",
    "drivers/soc/samsung/exynos/pm_qos/exynos-pm-qos.ko",
    "drivers/soc/samsung/exynos/exynos-pm/exynos-pm.ko",
    "drivers/devfreq/exynos_devfreq.ko",
    "drivers/dma/samsung/samsung-pdma.ko",
    "drivers/bts/exynos-bts.ko",
    "drivers/bts/exynos-btsops8825.ko",
    "drivers/soc/samsung/exynos/bcmdbg/exynos-bcm.ko",
    "drivers/soc/samsung/exynos/security/exynos-seclog.ko",
    "drivers/soc/samsung/exynos/security/exynos-tzasc.ko",
    "drivers/soc/samsung/exynos/security/exynos-seh.ko",
    "drivers/soc/samsung/exynos/security/exynos-cm.ko",
    "drivers/soc/samsung/exynos/security/secmem.ko",
    "drivers/soc/samsung/exynos/security/exyswd-rng.ko",
    "drivers/soc/samsung/exynos/security/exynos-sdm.ko",
    "drivers/mfd/s2mpu13_mfd.ko",
    "drivers/mfd/s2mpu14_mfd.ko",
    "drivers/regulator/pmic_class.ko",
    "drivers/regulator/s2mpu13_regulator.ko",
    "drivers/regulator/s2mpu14_regulator.ko",
    "drivers/rtc/rtc-s2mpu13.ko",
    "drivers/input/keyboard/s2mpu13-key.ko",
    "drivers/iio/adc/s2mpu13_adc.ko",
    "drivers/pinctrl/pinctrl-samsung-s2mpu13.ko",
    "drivers/pinctrl/pinctrl-samsung-s2mpu14.ko",
    "drivers/mfd/s2mpb02_mfd.ko",
    "drivers/regulator/s2mpb02-regulator.ko",
    "drivers/regulator/s2mpb03.ko",
    "drivers/regulator/s2dos05-regulator.ko",
    "drivers/ufs/host/ufs-exynos-core.ko",
    "sound/soc/samsung/exynos/abox/snd-soc-samsung-abox-gic.ko",
    "sound/soc/samsung/exynos/abox/snd-soc-samsung-abox.ko",
    "sound/soc/samsung/exynos/sec_audio_exynos.ko",
    "sound/soc/samsung/exynos/snd_debug_proc.ko",
    # "sound/soc/samsung/exynos/codecs/snd-soc-s5m3700.ko",
    # "sound/soc/samsung/exynos/codecs/snd-soc-s5m3700-dump.ko",
    # "sound/soc/samsung/exynos/exynos8825_sound.ko",
    # "sound/soc/codecs/snd-soc-wm-adsp.ko",
    # "sound/soc/codecs/snd-soc-cs35l41-i2c.ko",
    "drivers/phy/samsung/phy-exynos-mipi.ko",
    "drivers/phy/samsung/phy-exynos-mipi-dsim.ko",
    "drivers/dpu/exynos-drm.ko",
    # "drivers/dpu/panel/panel-samsung-drv.ko",
    # "drivers/dpu/panel/panel-samsung-s6e3hab.ko",
    "drivers/thermal/exynos_thermal_v2.ko",
    "drivers/soc/samsung/exynos/exynos-mcinfo.ko",
    "drivers/thermal/gpu_cooling.ko",
    "drivers/soc/samsung/exynos/debug/exynos-itmon.ko",
    "drivers/soc/samsung/exynos/sysevent.ko",
    "drivers/soc/samsung/exynos/debug/hardlockup-watchdog.ko",
    "drivers/soc/samsung/exynos/debug/exynos-getcpustate.ko",
    "drivers/soc/samsung/exynos/debug/ehld.ko",
    "drivers/soc/samsung/exynos/debug/debug-snapshot-debug-kinfo.ko",
    "drivers/soc/samsung/exynos/sysevent_notif.ko",
    "drivers/soc/samsung/exynos/imgloader.ko",
    "drivers/soc/samsung/exynos/debug/exynos-adv-tracer-s2d.ko",
    "drivers/soc/samsung/exynos/debug/exynos-ecc-handler.ko",
    "drivers/soc/samsung/exynos/debug/dss.ko",
    "drivers/soc/samsung/exynos/debug/exynos-ssld.ko",
    "drivers/soc/samsung/exynos/debug/exynos-adv-tracer.ko",
    "drivers/soc/samsung/exynos/memlogger.ko",
    "drivers/soc/samsung/exynos/debug/exynos-coresight.ko",
    "drivers/soc/samsung/exynos/cpif/cpif.ko",
    "drivers/soc/samsung/exynos/cpif/cpif_page.ko",
    "drivers/soc/samsung/exynos/cpif/direct_dm.ko",
    "drivers/soc/samsung/exynos/cpif/cpif_memlogger.ko",
    "drivers/soc/samsung/exynos/cpif/hook.ko",
    "drivers/soc/samsung/exynos/cpif/mcu_ipc.ko",
    "drivers/soc/samsung/exynos/cpif/shm_ipc.ko",
    "drivers/soc/samsung/exynos/cpif/dit/exynos_dit.ko",
    "drivers/soc/samsung/exynos/gnssif/gnssif.ko",
    "drivers/soc/samsung/exynos/gnssif/gnss_mbox.ko",
    "drivers/mmc/host/dw_mmc.ko",
    "drivers/mmc/host/dw_mmc-pltfm.ko",
    "drivers/mmc/host/dw_mmc-exynos-samsung.ko",
    # "drivers/gud/gud-s5e8825/MobiCoreDriver/mcDrvModule.ko",
    # "drivers/gud/gud-s5e8825/TlcTui/t-base-tui.ko",
    "drivers/scsi/scsi_srpmb.ko",
    "drivers/soc/samsung/exynos/exynos-el2/exynos-el2.ko",
    "drivers/soc/samsung/exynos/exynos-el2/exynos-s2mpu.ko",
    "drivers/cpufreq/freq-qos-tracer.ko",
    "drivers/cpufreq/exynos-acme.ko",
    "drivers/cpufreq/exynos-dsufreq.ko",
    "drivers/soc/samsung/exynos/exynos-afm.ko",
    "drivers/soc/samsung/exynos/exynos-ufcc.ko",
    "drivers/phy/samsung/phy-exynos-usbdrd-super.ko",
    "drivers/usb/dwc3/dwc3-exynos-usb.ko",
    "drivers/usb/dwc3/exynos_usb_debug.ko",
    "drivers/usb/dwc3/sec_usb_cb.ko",
    "drivers/usb/gadget/function/usb_f_dm.ko",
    "drivers/usb/gadget/function/usb_f_rndis_mp.ko",
    "drivers/staging/nanohub/nanohub.ko",
    "drivers/mfc/exynos_mfc.ko",
    "drivers/scaler/scaler.ko",
    "drivers/smfc/smfc.ko",
    "sound/usb/exynos-usb-audio-offloading.ko"
]

_VENDOR_VTS_MODULE_LIST = [
      "sound/soc/samsung/exynos/vts/snd-soc-samsung-vts-mailbox.ko",
      "sound/soc/samsung/exynos/vts/snd-soc-samsung-vts.ko",
      "sound/soc/samsung/exynos/slif/snd-soc-samsung-slif.ko",
]

_VENDOR_DEV_MODULE_LIST = [
    # modules only for debug/developments.
]

_VENDOR_DEV_DLKM_MODULE_LIST = [
    # modules only for debug/developments.
    # Do NOT add kunit test modules here. Please use _VENDOR_KUNIT_TEST_LIST.
    "drivers/soc/samsung/exynos/debug/exynos-debug-test.ko",
    "drivers/pinctrl/samsung/secgpio_dvs.ko",
]

_VENDOR_DLKM_MODULE_LIST = [
    # For vendor_dlkm in order
    "drivers/vision/npu.ko",
    "drivers/net/wireless/scsc/scsc_wlan.ko",
    "drivers/misc/samsung/scsc/scsc_logring.ko",
    "drivers/misc/samsung/scsc/scsc_log_collection.ko",
    "drivers/misc/samsung/scsc/scsc_mx.ko",
    "drivers/net/wireless/scsc/scsc_wifilogger.ko",
    "drivers/misc/samsung/scsc/mx_client_test.ko",
    "drivers/misc/samsung/scsc/scsc_platform_mif.ko",
    "drivers/misc/samsung/scsc/scsc_mmap.ko",
    "drivers/misc/samsung/scsc/scsc_mx250_fm.ko",
    "drivers/misc/samsung/scsc_bt/scsc_bt.ko",
    "drivers/soc/samsung/exynos/exynos-ppmpu.ko",
    "drivers/block/zram/vendor_zram.ko",
    "mm/sec_mm/sec_mm.ko",
]

_VENDOR_DLKM_RADIO_MODULE_LIST = [
    "drivers/media/radio/s610/fm_s620.ko",
]

_VENDOR_DLKM_BOOT_COMPLETE_MODULE_LIST = [
    # For vendor_dlkm in order
]

_SYSTEM_DLKM_BLOCKLIST = [
    "kernel/drivers/ptp/ptp_kvm.ko",
    "drivers/block/zram/zram.ko",
]

_VENDOR_DLKM_BLOCKLIST = [
    # For vendor_dlkm blocklist
]

_VENDOR_KUNIT_TEST_LIST = [
    "sound/soc/samsung/exynos/abox/test/audio_exynos_test.ko",
    "drivers/dma-buf/heaps/samsung/samsung_dma_heap_test.ko",
]

_VENDOR_VTS_KUNIT_TEST_LIST = [
    "sound/soc/samsung/exynos/vts/test/vts_exynos_test.ko",
]

def get_soc_vendor_dlkm_blocklist():
    modules_list = []
    modules_list += _VENDOR_DLKM_BLOCKLIST
    return modules_list

def get_soc_system_dlkm_blocklist():
    modules_list = []
    modules_list += _SYSTEM_DLKM_BLOCKLIST
    return modules_list

def get_soc_vendor_early_boot_modules_list():
    return _VENDOR_EARLY_MODULE_LIST

def get_soc_vendor_kunit_test_list(variant = None):
    modules_list = []
    if variant != "user":
        modules_list += _VENDOR_KUNIT_TEST_LIST
        if support_vts:
            modules_list += _VENDOR_VTS_KUNIT_TEST_LIST
    return modules_list

def get_soc_vendor_boot_modules_list(variant = None):
    modules_list = []
    modules_list += get_soc_vendor_early_boot_modules_list()
    modules_list += _VENDOR_MODULE_LIST
    if support_vts:
        modules_list += _VENDOR_VTS_MODULE_LIST
    if variant != "user":
        modules_list += _VENDOR_DEV_MODULE_LIST
    return modules_list

def get_soc_vendor_dlkm_modules_list(variant = None):
    modules_list = []
    modules_list += _VENDOR_DLKM_MODULE_LIST
    if support_radio:
        modules_list += _VENDOR_DLKM_RADIO_MODULE_LIST
    modules_list += _VENDOR_DLKM_BOOT_COMPLETE_MODULE_LIST
    if variant != "user":
        modules_list += _VENDOR_DEV_DLKM_MODULE_LIST
    modules_list += get_soc_vendor_dlkm_blocklist()
    return modules_list

def get_soc_vendor_modules_list(variant = None):
    modules_list = []
    modules_list += get_soc_vendor_boot_modules_list(variant)
    modules_list += get_soc_vendor_dlkm_modules_list(variant)
    modules_list += get_soc_vendor_kunit_test_list(variant)
    return modules_list

def define_s5e8825_soc_module(variant):
    MODULE_OUTS = get_soc_vendor_modules_list(variant)
    kernel_module(
        name = "s5e8825_soc_module.{}".format(variant),
        srcs = [
            "//exynos/soc-series/papaya-android15:exynos.soc_sources",
            "//exynos/device/erd/erd8825:exynos.erd8825_sources",
        ],
        outs = MODULE_OUTS,
        kernel_build = "//exynos/soc-series/common:exynos_kernel_build",
        visibility = [
            "//exynos/device:__subpackages__",
            "//exynos/soc-series:__subpackages__",
            "//exynos/external-modules:__subpackages__",
        ],
    )

def register_s5e8825_soc_module():
    define_s5e8825_soc_module("eng")
    define_s5e8825_soc_module("userdebug")
    define_s5e8825_soc_module("user")

