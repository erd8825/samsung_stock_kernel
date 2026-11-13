#
# Copyright (C) 2011 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


include $(LOCAL_PATH)/BoardConfig.mk

# dmd
$(call inherit-product-if-exists, vendor/samsung_slsi/telephony/samsungmobile/modem_logging.mk)

ALLOW_MISSING_DEPENDENCIES := true
PRODUCT_ENFORCE_VINTF_MANIFEST := false
PRODUCT_SHIPPING_API_LEVEL := 31
BOARD_BOOT_HEADER_VERSION := 4
BOARD_GKI_NONAB_COMPAT := true
PRODUCT_USE_DYNAMIC_PARTITIONS := true
BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT := true
BOARD_VENDOR_BOOTIMAGE_PARTITION_SIZE := 0x02000000
CONF_PATH_BASE := device/samsung/erd8825/conf
BOARD_USES_SW_GATEKEEPER := false
USE_SWIFTSHADER := false
BOARD_USES_GENERIC_KERNEL_IMAGE := true
BOARD_MOVE_GSI_AVB_KEYS_TO_VENDOR_BOOT := true

BOARD_PREBUILTS := device/samsung/$(TARGET_SOC)-prebuilts
INSTALLED_KERNEL_TARGET := ${BOARD_PREBUILTS}/kernel.prebuilt
BOARD_PREBUILT_DTBOIMAGE := $(BOARD_PREBUILTS)/dtbo.img
BOARD_PREBUILT_DTB := $(BOARD_PREBUILTS)/dtb.img
#BOARD_PREBUILT_BOOTLOADER_IMG := $(BOARD_PREBUILTS)/bootloader.img
INSTALLED_DTBIMAGE_TARGET := $(BOARD_PREBUILT_DTB)

BOARD_PREBUILT_KERNEL_MODULES := $(BOARD_PREBUILTS)/modules
BOARD_PREBUILT_KERNEL_MODULES_SYMBOLS := $(BOARD_PREBUILTS)/module_symbols
BOARD_PREBUILT_KUNIT_MODULES := $(BOARD_PREBUILTS)/kunit_modules
BOARD_VENDOR_RAMDISK_FRAGMENTS := dlkm
BOARD_VENDOR_RAMDISK_FRAGMENT.dlkm.KERNEL_MODULE_DIRS := top

# Reordered modules list
BOARD_VENDOR_RAMDISK_KERNEL_MODULES += $(foreach module, $(notdir $(shell cat $(BOARD_PREBUILTS)/modules/modules.order | sed 's/^.*\///')), $(BOARD_PREBUILT_KERNEL_MODULES)/$(module))
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_OPTIONS_FILE := $(BOARD_PREBUILT_KERNEL_MODULES)/vendor_boot.modules.options

# set system dlkm source directory and remove it in copy file list
BOARD_SYSTEM_DLKM_SRC := $(BOARD_PREBUILTS)/system_dlkm_staging
BOARD_SYSTEM_KERNEL_MODULES := $(wildcard $(BOARD_SYSTEM_DLKM_SRC)/*.ko)
BOARD_SYSTEM_KERNEL_MODULES_LOAD := $(shell cat $(BOARD_SYSTEM_DLKM_SRC)/system_dlkm.modules.load)
BOARD_SYSTEM_KERNEL_MODULES_BLOCKLIST_FILE := $(BOARD_SYSTEM_DLKM_SRC)/system_dlkm.modules.blocklist

BOARD_VENDOR_DLKM_SRC := $(BOARD_PREBUILTS)/vendor_dlkm_staging
BOARD_VENDOR_KERNEL_MODULES := $(wildcard $(BOARD_VENDOR_DLKM_SRC)/*.ko)
BOARD_VENDOR_KERNEL_MODULES_LOAD := $(shell cat $(BOARD_VENDOR_DLKM_SRC)/vendor_dlkm.modules.load)
BOARD_VENDOR_KERNEL_MODULES_BLOCKLIST_FILE := $(BOARD_VENDOR_DLKM_SRC)/vendor_dlkm.modules.blocklist
PRODUCT_COPY_FILES += $(BOARD_PREBUILTS)/init.insmod.vendor_dlkm.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod.vendor_dlkm.cfg

BOARD_RECOVERY_KERNEL_MODULES += $(BOARD_VENDOR_RAMDISK_KERNEL_MODULES)

PRODUCT_COPY_FILES += $(foreach image,\
	$(filter-out $(BOARD_SYSTEM_DLKM_SRC) \
		     $(BOARD_VENDOR_DLKM_SRC) \
		     $(BOARD_PREBUILT_DTBOIMAGE) \
		     $(BOARD_PREBUILT_DTB) \
		     $(BOARD_PREBUILT_KERNEL_MODULES) \
		     $(BOARD_PREBUILT_KERNEL_MODULES_SYMBOLS) \
		     $(BOARD_PREBUILT_KUNIT_MODULES), \
		     $(wildcard $(BOARD_PREBUILTS)/*)),\
	$(image):$(notdir $(image)))

PRODUCT_COPY_FILES += device/samsung/erd8825/conf/init.insmod.sh:/vendor/bin/init.insmod.sh
PRODUCT_COPY_FILES += $(BOARD_PREBUILTS)/init.insmod.vendor_dlkm.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod.vendor_dlkm.cfg
PRODUCT_COPY_FILES += $(foreach modules, $(wildcard $(BOARD_PREBUILT_KERNEL_MODULES_SYMBOLS)/*), $(modules):$(PRODUCT_OUT)/modules_symbols/$(notdir $(modules)))


#EXYNOS_USE_VIRTUAL_AB=true
ifeq ($(EXYNOS_USE_VIRTUAL_AB),true)
AB_OTA_UPDATER := true
EXYNOS_VIRTUAL_AB_COMPRESSION := true
EXYNOS_KERNEL_DM_SNAPSHOT_AS_MODULE := false		# Current Kernel uses CONFIG_DM_SNAPSHOT=y
AB_OTA_PARTITIONS := \
	vendor \
	vbmeta \
        vbmeta_system \
        dtbo \
	boot \
	vendor_boot \

ifneq ($(WITH_ESSI), true)
AB_OTA_PARTITIONS += \
	system \

endif

# Boot Contorl HAL to support AB system
EXYNOS_BOOT_CONTROL_HAL_INTERFACE_TYPE := AIDL
ifeq ($(EXYNOS_BOOT_CONTROL_HAL_INTERFACE_TYPE), HIDL)
EXYNOS_BOOT_CONTROL_HAL_NAME := android.hardware.boot@1.2
PRODUCT_PACKAGES += \
	$(EXYNOS_BOOT_CONTROL_HAL_NAME)-impl-exynos \
	$(EXYNOS_BOOT_CONTROL_HAL_NAME)-impl-exynos.recovery \
	$(EXYNOS_BOOT_CONTROL_HAL_NAME)-service \
	$(EXYNOS_BOOT_CONTROL_HAL_NAME)-service.recovery \

else
PRODUCT_PACKAGES += \
	android.hardware.boot-service.exynos \
	android.hardware.boot-service.exynos_recovery \

endif


PRODUCT_PACKAGES += \
	update_engine \
	update_verifier \
	update_engine_client \

# Recovery Configuration for Virtual AB
# Ignore case for applying AB update with Recovery image
BOARD_EXCLUDE_KERNEL_FROM_RECOVERY_IMAGE :=
BOARD_USES_FULL_RECOVERY_IMAGE :=
BOARD_MOVE_RECOVERY_RESOURCES_TO_VENDOR_BOOT := true
NORMAL_TARGET_COPY_OUT_VENDOR_RAMDISK := $(TARGET_COPY_OUT_VENDOR_RAMDISK)/first_stage_ramdisk
RECOVERY_TARGET_COPY_OUT_VENDOR_RAMDISK := $(TARGET_COPY_OUT_VENDOR_RAMDISK)
TARGET_NO_RECOVERY := true

else
# If Non-AB, then recovery must be exist
BOARD_EXCLUDE_KERNEL_FROM_RECOVERY_IMAGE :=
BOARD_USES_FULL_RECOVERY_IMAGE := true
BOARD_MOVE_RECOVERY_RESOURCES_TO_VENDOR_BOOT :=
NORMAL_TARGET_COPY_OUT_VENDOR_RAMDISK := $(TARGET_COPY_OUT_VENDOR_RAMDISK)
RECOVERY_TARGET_COPY_OUT_VENDOR_RADISK := $(TARGET_COPY_OUT_VENDOR_RAMDISK)
TARGET_NO_RECOVERY :=
endif



PRODUCT_VENDOR_PROPERTIES += \
	ro.soc.manufacturer=$(TARGET_SOC_MANUFACTURE) \
	ro.soc.model=$(TARGET_SOC_MODEL)

BOARD_RAMDISK_OFFSET := 0
BOARD_KERNEL_TAGS_OFFSET := 0
ifneq ($(call math_gt_or_eq,$(BOARD_BOOT_HEADER_VERSION),3),)
BOARD_INCLUDE_DTB_IN_BOOTIMG := true
BOARD_DTB_OFFSET := 0

# recovery mode
ifeq ($(EXYNOS_USE_DEDICATED_RECOVERY_IMAGE), true)
BOARD_INCLUDE_RECOVERY_DTBO := true
BOARD_RECOVERY_CMDLINE := $(BOARD_BOOTCONFIG)
BOARD_RECOVERY_CMDLINE += $(BOARD_KERNEL_CMDLINE)
BOARD_RECOVERY_MKBOOTIMG_ARGS := \
  --ramdisk_offset $(BOARD_RAMDISK_OFFSET) \
  --tags_offset $(BOARD_KERNEL_TAGS_OFFSET) \
  --header_version 2 \
  --dtb $(INSTALLED_DTBIMAGE_TARGET) \
  --dtb_offset $(BOARD_DTB_OFFSET) \
  --pagesize $(BOARD_FLASH_BLOCK_SIZE) \
  --recovery_dtbo $(BOARD_PREBUILT_DTBOIMAGE) \
  --cmdline "$(BOARD_RECOVERY_CMDLINE)"
endif

BOARD_MKBOOTIMG_ARGS := \
  --ramdisk_offset $(BOARD_RAMDISK_OFFSET) \
  --tags_offset $(BOARD_KERNEL_TAGS_OFFSET) \
  --header_version $(BOARD_BOOT_HEADER_VERSION) \
  --dtb $(INSTALLED_DTBIMAGE_TARGET) \
  --dtb_offset 0 \
  --pagesize $(BOARD_FLASH_BLOCK_SIZE)
else

BOARD_MKBOOTIMG_ARGS := \
  --ramdisk_offset $(BOARD_RAMDISK_OFFSET) \
  --tags_offset $(BOARD_KERNEL_TAGS_OFFSET) \
  --header_version $(BOARD_BOOT_HEADER_VERSION)
endif

PRODUCT_COPY_FILES += \
		$(INSTALLED_KERNEL_TARGET):kernel


# Partitions options
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x04000000
BOARD_DTBOIMG_PARTITION_SIZE := 0x00800000

BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
#BOARD_VENDORIMAGE_PARTITION_SIZE := 943718400

BOARD_EXT4_SHARE_DUP_BLOCKS := true

TARGET_USERIMAGES_USE_EXT4 := true
#BOARD_SYSTEMIMAGE_PARTITION_SIZE := 3145728000
BOARD_USERDATAIMAGE_PARTITION_SIZE := 11796480000
BOARD_MOUNT_SDCARD_RW := true
# Recovery is optional for AB
ifeq ($(EXYNOS_USE_DEDICATED_RECOVERY_IMAGE), true)
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x6000000
endif
# If AB system is applied, cache partition does not used
ifneq ($(AB_OTA_UPDATER), true)
BOARD_CACHEIMAGE_PARTITION_SIZE := 69206016
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
else
# @RECOVERY TG : set 40MB for cache.img in VAB model
BOARD_CACHEIMAGE_PARTITION_SIZE := 41943040
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
endif

BOARD_SUPER_PARTITION_GROUPS := group_basic

# Dynamic Partitions options
#ifeq ($(EXYNOS_USE_VIRTUAL_AB), true)
#BOARD_SUPER_PARTITION_SIZE := 8589934592
#BOARD_GROUP_BASIC_SIZE := 4919918592
#else
BOARD_SUPER_PARTITION_SIZE := 4924112896
BOARD_GROUP_BASIC_SIZE := 4919918592
#endif

# Configuration for dynamic partitions
ifeq ($(WITH_ESSI),true)
BOARD_GROUP_BASIC_PARTITION_LIST := vendor vendor_dlkm system_dlkm
else
BOARD_GROUP_BASIC_PARTITION_LIST := system vendor vendor_dlkm system_dlkm
endif

ifeq ($(TARGET_BUILD_VARIANT),eng)
PRODUCT_SET_DEBUGFS_RESTRICTIONS := false
endif

# control_privapp_permissions
PRODUCT_PROPERTY_OVERRIDES += \
	ro.control_privapp_permissions?=log

# Device Manifest, Device Compatibility Matrix for Treble
ifeq ($(AB_OTA_UPDATER), true)
DEVICE_MANIFEST_FILE := device/samsung/erd8825/manifest_ab.xml
else
DEVICE_MANIFEST_FILE := device/samsung/erd8825/manifest.xml
endif
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE := device/samsung/erd8825/framework_compatibility_matrix.xml

DEVICE_MATRIX_FILE := \
	device/samsung/erd8825/compatibility_matrix.xml

DEVICE_PACKAGE_OVERLAYS += device/samsung/erd8825/overlay

# WifiOverlay for WLAN
PRODUCT_PACKAGES += \
    WifiOverlayErd8825

ifeq ($(SEC_BUILD_OPTION_KNOX_CSB),true)
ifeq ($(TARGET_USE_EVT0),true)
UNSIGN_FW_DSP := device/samsung/erd8825/firmware/dsp/unsigned_npu_master.bin
SIGNED_FW_DSP := device/samsung/erd8825/firmware/dsp/npu_master.bin
else
UNSIGN_FW_DSP := device/samsung/erd8825/firmware/unsigend_NPU.bin
SIGNED_FW_DSP := device/samsung/erd8825/firmware/NPU.bin
endif

type=${VBMETA_TYPE}
project=${SEC_BUILD_CONF_MODEL_SIGNING_NAME}
SECURE_SCRIPT=../buildscript/tools/signclient.jar

DSP_RENAMING := $(shell mv $(SIGNED_FW_DSP) $(UNSIGN_FW_DSP))
DSP_SIGNNING := $(shell java -jar $(SECURE_SCRIPT) -runtype ${type} -model ${project} -input $(UNSIGN_FW_DSP) -output $(SIGNED_FW_DSP))
endif

# Init files
PRODUCT_COPY_FILES += \
	$(CONF_PATH_BASE)/init.s5e8825.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.$(TARGET_SOC).rc \
	$(CONF_PATH_BASE)/init.s5e8825.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.$(TARGET_SOC).usb.rc \
	device/samsung/erd8825/conf/ueventd.s5e8825.rc:$(TARGET_COPY_OUT_VENDOR)/etc/ueventd.rc \
	device/samsung/erd8825/conf/init.recovery.s5e8825.rc:root/init.recovery.$(TARGET_SOC).rc \
	device/samsung/erd8825/task_profiles.json:$(TARGET_COPY_OUT_VENDOR)/etc/task_profiles.json
#fstab file selection:
BOOTUP_STORAGE := $(BOARD_USES_BOOT_STORAGE)
ifeq ($(AB_OTA_UPDATER), true)
FSTAB_FILE := fstab.s5e8825.$(BOOTUP_STORAGE).ab
else
FSTAB_FILE := fstab.s5e8825.$(BOOTUP_STORAGE)
endif
PRODUCT_COPY_FILES += \
	$(CONF_PATH_BASE)/$(FSTAB_FILE):$(TARGET_COPY_OUT_VENDOR)/etc/fstab.$(TARGET_SOC)
PRODUCT_COPY_FILES += \
	$(CONF_PATH_BASE)/$(FSTAB_FILE):$(NORMAL_TARGET_COPY_OUT_VENDOR_RAMDISK)/fstab.$(TARGET_SOC) \

TARGET_RECOVERY_FSTAB := $(CONF_PATH_BASE)/$(FSTAB_FILE)

#PRODUCT_COPY_FILES += \
	device/samsung/universal9925/conf/init.recovery.s5e9925.rc:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/init.recovery.$(TARGET_SOC).rc \
	$(CONF_PATH_BASE)/init.s5e9925.usb.rc:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/etc/init/init.$(TARGET_SOC).usb.rc \
	$(FSTAB_PATH_BASE)/$(FSTAB_FILE):$(TARGET_COPY_OUT_VENDOR_RAMDISK)/etc/recovery.fstab

# Support devtools
ifeq ($(TARGET_BUILD_VARIANT),eng)
PRODUCT_PACKAGES += \
	Development
endif

#Health 1.0 HAL
PRODUCT_PACKAGES += \
       android.hardware.health-service.example

# SKPM HAL
$(call inherit-product-if-exists, vendor/samsung/hal-server/skpm/skpmHal.mk)

# Filesystem management tools
PRODUCT_PACKAGES += \
	e2fsck

PRODUCT_CHARACTERISTICS := phone

PRODUCT_AAPT_CONFIG := normal hdpi xhdpi xxhdpi
PRODUCT_AAPT_PREF_CONFIG := xxhdpi

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PACKAGES += \
    hwservicemanager \
    vndservicemanager

ifeq ($(shell expr $(shell secgetspf SEC_PRODUCT_FEATURE_BUILD_MAINLINE_API_LEVEL) \> 35), 1)
PRODUCT_PROPERTY_OVERRIDES += \
	ro.frp.pst=/dev/block/platform/13500000.ufs/by-name/frp
endif

#VNDK
PRODUCT_PACKAGES += \
	vndk-libs

PRODUCT_ENFORCE_RRO_TARGETS := \
	framework-res

# Use FUSE passthrough
PRODUCT_PRODUCT_PROPERTIES += \
	persist.sys.fuse.passthrough.enable=true

# fuse perf
PRODUCT_PROPERTY_OVERRIDES += persist.sys.sec.fuse.perf=true

ifeq ($(BOARD_USES_GENERIC_AIDL_AUDIO), true)
PRODUCT_PACKAGES += \
    com.android.hardware.audio

PRODUCT_COPY_FILES += \
    device/generic/goldfish/audio/policy/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    device/generic/goldfish/audio/policy/primary_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/primary_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/bluetooth_audio_policy_configuration_7_0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/bluetooth_audio_policy_configuration_7_0.xml \
    hardware/interfaces/audio/aidl/default/audio_effects_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects_config.xml
endif

PRODUCT_PACKAGES += libstagefright_wfd \
PRODUCT_PACKAGES += \
	android.hardware.thermal-service.exynos \
	android.hardware.sensors-service.example \

ifneq ($(BOARD_DISPLAYCONFIG_VENDOR_XML_PATH),)
PRODUCT_COPY_FILES += $(foreach xml,\
        $(wildcard $(BOARD_DISPLAYCONFIG_VENDOR_XML_PATH)/*.xml),\
        $(xml):$(TARGET_COPY_OUT_VENDOR)/etc/displayconfig/$(notdir $(xml)))
endif

# Fastboot HAL
PRODUCT_PACKAGES += \
       fastbootd \
       android.hardware.fastboot@1.1\
       android.hardware.fastboot@1.1-impl-mock.s5e8825

# USB HAL
PRODUCT_PACKAGES += \
        android.hardware.usb-service.exynos \
        com.android.future.usb.accessory

PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
        frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml

PRODUCT_PROPERTY_OVERRIDES += \
        dev.usbsetting.embedded=on

PRODUCT_COPY_FILES += \
        device/samsung/erd8825/handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml \

PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
		frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \

# Include additional device.mk
ifeq ($(BOARD_USES_GENERIC_AIDL_AUDIO), false)
include $(LOCAL_PATH)/include/audio.mk
#include $(LOCAL_PATH)/include/bluetooth.mk
endif
include $(LOCAL_PATH)/include/camera.mk
include $(LOCAL_PATH)/include/security.mk
include $(LOCAL_PATH)/include/video.mk
include $(LOCAL_PATH)/include/npu.mk
include $(LOCAL_PATH)/include/hwc.mk
include $(LOCAL_PATH)/include/gpu.mk
include $(LOCAL_PATH)/include/color.mk
include $(LOCAL_PATH)/include/m2m.mk
include $(LOCAL_PATH)/include/wfd.mk
include $(LOCAL_PATH)/include/sf.mk
#include $(LOCAL_PATH)/include/sensor.mk
#include $(LOCAL_PATH)/include/bluetooth.mk
include $(LOCAL_PATH)/include/memtrack.mk
#include $(LOCAL_PATH)/include/m2m.mk
#include $(LOCAL_PATH)/include/wifi.mk

# Bluetooth module init (insmod)
#PRODUCT_COPY_FILES += \
#    device/samsung/erd8825/conf/init.insmod.bluetooth.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod.bluetooth.cfg

# Bluetooth configuration
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    hardware/samsung_slsi/libbt/conf/bt_did.conf:$(TARGET_COPY_OUT_VENDOR)/etc/bluetooth/bt_did.conf \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml

PRODUCT_PACKAGES += \
    android.hardware.bluetooth-service.default \
    android.hardware.bluetooth.audio-impl

# Use 64-bit dex2oat for better dexopt time.
PRODUCT_PROPERTY_OVERRIDES += \
	dalvik.vm.dex2oat64.enabled=true


# enable dexpreopt in eng
# dexpreopt is disabled by default in eng
ifneq (,$(filter eng,$(TARGET_BUILD_VARIANT)))
OVERRIDE_DISABLE_DEXOPT_ALL := false
endif

$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/updatable_apex.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/emulated_storage.mk)
$(call inherit-product, frameworks/native/build/phone-xhdpi-6144-dalvik-heap.mk)
$(call inherit-product-if-exists, vendor/samsung_slsi/common/exynos-vendor.mk)
ifeq ($(EXYNOS_USE_VIRTUAL_AB),true)
# Previous AOSP guide is to use lunch default make file, it changed from android S
$(call inherit-product, $(SRC_TARGET_DIR)/product/virtual_ab_ota/vabc_features.mk)
endif

# PowerHAL
#PRODUCT_SOONG_NAMESPACES += device/samsung/erd8825/power
#PRODUCT_PACKAGES += \
#       android.hardware.power-service.exynos8825 \
#       android.hardware.power.stats-service.exynos8825 \
#       com.android.hardware.power.exynos8825

ifneq ($(BOARD_DISPLAYCONFIG_VENDOR_XML_PATH),)
PRODUCT_COPY_FILES += $(foreach xml,\
        $(wildcard $(BOARD_DISPLAYCONFIG_VENDOR_XML_PATH)/*.xml),\
        $(xml):$(TARGET_COPY_OUT_VENDOR)/etc/displayconfig/$(notdir $(xml)))
endif

# EPIC
PRODUCT_SOONG_NAMESPACES += device/samsung/erd8825/epic
PRODUCT_PACKAGES += \
       epic libems_service libexynos_migov libgmc libepic_helper

PRODUCT_COPY_FILES += \
       device/samsung/erd8825/epic/epic.json:$(TARGET_COPY_OUT_VENDOR)/etc/epic.json \
       device/samsung/erd8825/epic/ems.json:$(TARGET_COPY_OUT_VENDOR)/etc/ems.json \
       device/samsung/erd8825/epic/ems_parameter.json:$(TARGET_COPY_OUT_VENDOR)/etc/ems_parameter.json

# Enabled only in ERD and not in SEP
ifneq (,$(findstring erd8825, $(TARGET_PRODUCT)))
PRODUCT_VENDOR_PROPERTIES += \
	vendor.power.touch=1 \
	vendor.power.applaunch=1 \
	vendor.power.applaunchrelease=1 \
	vendor.epic.feature=1
endif

#Epic AIDL
SOONG_CONFIG_NAMESPACES += epic
SOONG_CONFIG_epic := vendor_hint
SOONG_CONFIG_epic_vendor_hint := true

PRODUCT_PACKAGES += \
	vendor.samsung_slsi.hardware.epic \
	vendor.samsung_slsi.hardware.epic-service

# Gralloc libs
PRODUCT_PACKAGES += libexynosgraphicbuffer_public

#$(call inherit-product-if-exists, vendor/samsung_slsi/telephony/$(BOARD_USES_SHARED_VENDOR_TELEPHONY)/common/device-vendor.mk)

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
	frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \

# WLAN configuration
# device specific wpa_supplicant.conf
#PRODUCT_COPY_FILES += \
#    device/samsung/erd8825/wpa_supplicant.conf:vendor/etc/wifi/wpa_supplicant.conf \
#    device/samsung/erd8825/wifi/p2p_supplicant.conf:vendor/etc/wifi/p2p_supplicant.conf \
#
#PRODUCT_PROPERTY_OVERRIDES += \
#        wifi.interface=wlan0
#PRODUCT_PROPERTY_OVERRIDES += \
#        wifi.aware.interface=wifi-aware0

# Packages needed for WLAN
# Note android.hardware.wifi@1.0-service is used by HAL interface 1.2
#PRODUCT_PACKAGES += \
#    android.hardware.wifi-service \
#    android.hardware.wifi.supplicant@1.1-service \
#    android.hardware.wifi.offload@1.0-service \
#    dhcpcd.conf \
#    hostapd \
#    wificond \
#    wifilog \
#    wpa_supplicant.conf \
#    wpa_supplicant \
#    wpa_cli \
#    hostapd_cli

#PRODUCT_PACKAGES += \
#    libwifi-hal \
#    libwifi-system \
#    libwifikeystorehal \
#    android.hardware.wifi.supplicant@1.2 \
#    android.hardware.wifi@1.3

## WLBT Logging ##
PRODUCT_PROPERTY_OVERRIDES += \
        persist.vendor.wlbtlog.maxfilesize=50 \
        persist.vendor.wlbtlog.maxfiles=5

$(call inherit-product, device/samsung/erd8825/scsc_wlbt.mk)
#$(call inherit-product, vendor/samsung_slsi/scsc_tools/wlbt/device-vendor.mk)

PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
        frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \

#Defined in /erd8825/include/wifi.mk
#supplicant configs
#ifneq ($(BOARD_WPA_SUPPLICANT_DRIVER),)
#CONFIG_DRIVER_$(BOARD_WPA_SUPPLICANT_DRIVER) := y
#endif
#WPA_SUPPL_DIR = external/wpa_supplicant_8
#include $(WPA_SUPPL_DIR)/wpa_supplicant/wpa_supplicant_conf.mk
#include $(WPA_SUPPL_DIR)/wpa_supplicant/android.config
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/wifi/init.insmod.wifi.cfg:/vendor/etc/wifi/init.insmod.wifi.cfg \
    device/samsung/erd8825/wifi/init.exynos.wifi.rc:/vendor/etc/init/init.exynos.wifi.rc

# Enable eden
$(call inherit-product-if-exists, vendor/samsung_slsi/exynos/eden/eden.mk)

PRODUCT_PACKAGES += \
                vendor.samsung_slsi.hardware.eden_runtime_aidl-impl \
                vendor.samsung_slsi.hardware.eden_runtime_aidl-service \
                vendor.samsung_slsi.hardware.eden_runtime_aidl-V1-ndk \
                libeden_rt_stub.edensdk.samsung \
                libeden_nn_on_system \
                public.libraries-edensdk.samsung.txt \
                vendor.samsung_slsi.hardware.enn_aux_aidl-impl \
                vendor.samsung_slsi.hardware.enn_aux_aidl-service\
                vendor.samsung_slsi.hardware.enn_aux_aidl-V1-ndk \
                libeden_gpu_boost_stub.edensdk.samsung \
                libeden_gb_on_system

PRODUCT_PROPERTY_OVERRIDES += \
                log.tag.EDEN=INFO \
                ro.vendor.eden.devices=CPU1_GPU1_NPU1 \
                ro.vendor.eden.npu.version.path=/sys/devices/platform/npu_exynos/version \
                ro.vendor.eden.core_mask=112 \
                ro.vendor.eden.mrs=16 \
                ro.vendor.eden.mnonm=32

PRODUCT_COPY_FILES += \
   device/samsung/erd8825/eden/eden_kernel_64.bin:$(TARGET_COPY_OUT_VENDOR)/etc/eden/gpu/eden_kernel_64.bin \
   device/samsung/erd8825/eden/enn_preset.json:$(TARGET_COPY_OUT_VENDOR)/etc/eden/enn_preset.json

# Exynos RIL and telephony
$(call inherit-product-if-exists, vendor/samsung_slsi/telephony/common/device-vendor.mk)
$(call inherit-product-if-exists, packages/apps/ShannonIms/device-vendor.mk)

# GNSS
BOARD_USES_EXYNOS_GNSS_HAL:=false
ifeq ($(BOARD_USES_EXYNOS_GNSS_HAL), true)
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml
# Exynos GNSS
$(call inherit-product, vendor/samsung_slsi/gnss/s5e8825/device-gnss.mk)
endif

TARGET_REQUIRES_HIDL_CAS_HAL := false

# BSP Property for Netflix HD
PRODUCT_PROPERTY_OVERRIDES += \
        ro.netflix.bsp_rev=EXYNOS1280-34993-1

# performance property
PRODUCT_PROPERTY_OVERRIDES += sys.perf.hmp=6:2

# RAM Plus
ifeq ($(shell secgetspf SEC_PRODUCT_FEATURE_SYSTEM_SUPPORT_RAMPLUS_MODEL_FSTAB), FALSE)
PRODUCT_COPY_FILES += \
	device/samsung/common/fstab.ramplus:vendor/etc/fstab.ramplus
endif

PRODUCT_COPY_FILES += \
	device/samsung/common/init.ramplus.rc:vendor/etc/init/init.ramplus.rc
	
# verified boot
ifeq ($(SEC_DMVERITY),true)
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.software.verified_boot.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.verified_boot.xml
endif
