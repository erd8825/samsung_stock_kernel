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

# TODO(b/147336716): Remove BUILD_BROKEN_PREBUILT_ELF_FILES
BUILD_BROKEN_PREBUILT_ELF_FILES := true
BUILD_BROKEN_ELF_PREBUILT_PRODUCT_COPY_FILES := true

# Should be uncommented after fixing vndk-sp violation is fixed.
PRODUCT_FULL_TREBLE_OVERRIDE := true

TARGET_BOARD_INFO_FILE := device/samsung/erd8825/board-info.txt
TARGET_BOARD_KERNEL_HEADERS := hardware/samsung_slsi/exynos/kernel-6.6-headers/kernel-headers

TARGET_SOC_NAME := exynos

TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true

TARGET_CPU_VARIANT := cortex-a53
TARGET_2ND_CPU_VARIANT := cortex-a53

TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

#audio
BOARD_USES_GENERIC_AUDIO := true
BOARD_USES_GENERIC_AIDL_AUDIO := false

# SOC Model specific variable. For CDD12 Requirement
TARGET_SOC_MANUFACTURE := Samsung
TARGET_SOC_MODEL := s5e8825

# Storage options
BOARD_USES_BOOT_STORAGE := ufs
BOARD_USES_VENDORIMAGE := true
TARGET_COPY_OUT_VENDOR := vendor
TARGET_USERIMAGES_USE_F2FS := true
BOARD_FLASH_BLOCK_SIZE := 4096

include device/samsung/erd8825/BootConfig.mk


# SW/TEE Gatekeeper/Keymint
BOARD_TEE_GATEKEEPER_ENABLE := false
BOARD_TEE_KEYMINT_ENABLE := false

# Enable Mobicore
BOARD_MOBICORE_ENABLE := false

# CBD (CP booting daemon)
SOONG_CONFIG_NAMESPACES += cbd
SOONG_CONFIG_cbd := version protocol cptype
SOONG_CONFIG_cbd_version := v3
SOONG_CONFIG_cbd_protocol := sipc
SOONG_CONFIG_cbd_cptype := modap

# Modem interface 
ifeq ($(shell secgetspf SEC_PRODUCT_FEATURE_FACTORY_SUPPORT_INTERPOSER), FALSE)
PRODUCT_COPY_FILES += \
	vendor/samsung_slsi/common/cpboot_v3/init.ss310r.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.baseband.rc
endif

SOONG_CONFIG_cbd += buildTest
ifeq ($(filter user,$(TARGET_BUILD_VARIANT)),)
SOONG_CONFIG_cbd_buildTest := true
else
SOONG_CONFIG_cbd_buildTest := false
endif

BOARD_RAMDISK_USE_LZ4 := true

# SELinux Platform Vendor policy for exynos
VENDOR_SEPOLICY := device/samsung/sepolicy/common

ifeq (, $(findstring $(VENDOR_SEPOLICY), $(BOARD_VENDOR_SEPOLICY_DIRS)))
BOARD_VENDOR_SEPOLICY_DIRS += $(VENDOR_SEPOLICY)
endif

# SECCOMP Policy
BOARD_SECCOMP_POLICY = device/samsung/erd8825/seccomp_policy

# SELinux Platform Private policy for exynos
SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS := device/samsung/sepolicy/private

# SELinux Platform Public policy for exynos
SYSTEM_EXT_PUBLIC_SEPOLICY_DIRS := device/samsung/sepolicy/public

# SENSOR HUB
BOARD_USES_EXYNOS_SENSORS_DUMMY := false

# SYSTEM SDK
#BOARD_SYSTEMSDK_VERSIONS := 31
BOARD_SHIPPING_API_LEVEL := 31

#VNDK
BOARD_PROPERTY_OVERRIDES_SPLIT_ENABLED := true
BOARD_VNDK_VERSION := current

# Enable AVB2.0
BOARD_AVB_ENABLE := true
BOARD_AVB_ALGORITHM := SHA256_RSA4096
BOARD_AVB_KEY_PATH := device/samsung/erd8825/avbkey_rsa4096.pem
BOARD_AVB_ROLLBACK_INDEX := 0

# Chained partition
BOARD_AVB_BOOT_KEY_PATH := $(BOARD_AVB_KEY_PATH)
BOARD_AVB_BOOT_ALGORITHM := $(BOARD_AVB_ALGORITHM)
BOARD_AVB_BOOT_ROLLBACK_INDEX := 0
BOARD_AVB_BOOT_ROLLBACK_INDEX_LOCATION := 1

BOARD_AVB_VENDOR_BOOT_KEY_PATH := $(BOARD_AVB_KEY_PATH)
BOARD_AVB_VENDOR_BOOT_ALGORITHM := $(BOARD_AVB_ALGORITHM)
BOARD_AVB_VENDOR_BOOT_ROLLBACK_INDEX := 0
BOARD_AVB_VENDOR_BOOT_ROLLBACK_INDEX_LOCATION := 2

BOARD_AVB_DTBO_KEY_PATH := $(BOARD_AVB_KEY_PATH)
BOARD_AVB_DTBO_ALGORITHM := $(BOARD_AVB_ALGORITHM)
BOARD_AVB_DTBO_ROLLBACK_INDEX := 0
BOARD_AVB_DTBO_ROLLBACK_INDEX_LOCATION := 3

BOARD_AVB_RECOVERY_KEY_PATH := $(BOARD_AVB_KEY_PATH)
BOARD_AVB_RECOVERY_ALGORITHM := $(BOARD_AVB_ALGORITHM)
BOARD_AVB_RECOVERY_ROLLBACK_INDEX := 0
BOARD_AVB_RECOVERY_ROLLBACK_INDEX_LOCATION := 4

BOARD_AVB_VBMETA_SYSTEM := system vendor system_dlkm vendor_dlkm
BOARD_AVB_VBMETA_SYSTEM_KEY_PATH := $(BOARD_AVB_KEY_PATH)
BOARD_AVB_VBMETA_SYSTEM_ALGORITHM := $(BOARD_AVB_ALGORITHM)
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX := 0
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX_LOCATION := 10
BOARD_AVB_SYSTEM_ADD_HASHTREE_FOOTER_ARGS += --hash_algorithm sha256
BOARD_AVB_VENDOR_ADD_HASHTREE_FOOTER_ARGS += --hash_algorithm sha256
BOARD_AVB_SYSTEM_DLKM_ADD_HASHTREE_FOOTER_ARGS += --hash_algorithm sha256
BOARD_AVB_VENDOR_DLKM_ADD_HASHTREE_FOOTER_ARGS += --hash_algorithm sha256

# Device Tree
BOARD_USES_DT := true

# CAMERA
BOARD_BACK_CAMERA_ROTATION := 90
BOARD_FRONT_CAMERA_ROTATION := 270
BOARD_SECURE_CAMERA_ROTATION := 0
ifneq ($(filter full_erd8825_v ,$(TARGET_PRODUCT)),)
BOARD_BACK_CAMERA_SENSOR := SENSOR_NAME_IMX582
BOARD_FRONT_CAMERA_SENSOR := SENSOR_NAME_S5K3H1
else ifneq ($(filter a33x%, $(TARGET_PRODUCT)),)
BOARD_BACK_CAMERA_SENSOR := SENSOR_NAME_IMX582
BOARD_FRONT_CAMERA_SENSOR := SENSOR_NAME_IMX616
else
BOARD_BACK_CAMERA_SENSOR := SENSOR_NAME_IMX582
BOARD_FRONT_CAMERA_SENSOR := SENSOR_NAME_S5K3J1
endif
BOARD_SECURE_CAMERA_SENSOR := SENSOR_NAME_S5K5F1
BOARD_BACK_1_CAMERA_SENSOR := SENSOR_NAME_NOTHING
BOARD_FRONT_1_CAMERA_SENSOR := SENSOR_NAME_NOTHING
BOARD_BACK_2_CAMERA_SENSOR := SENSOR_NAME_NOTHING
BOARD_BACK_2_CAMERA_SENSOR_OPEN := false
BOARD_FRONT_2_CAMERA_SENSOR := SENSOR_NAME_NOTHING
BOARD_BACK_3_CAMERA_SENSOR := SENSOR_NAME_NOTHING
BOARD_FRONT_3_CAMERA_SENSOR := SENSOR_NAME_NOTHING

ifeq ($(filter gta4xls%, $(TARGET_PRODUCT)),)
BOARD_CAMERA_USES_DUAL_CAMERA := true
endif
BOARD_DUAL_CAMERA_REAR_ZOOM_MASTER := CAMERA_ID_BACK_0
#BOARD_DUAL_CAMERA_REAR_ZOOM_SLAVE := CAMERA_ID_BACK_1
#BOARD_DUAL_CAMERA_REAR_PORTRAIT_MASTER := CAMERA_ID_BACK_1
#BOARD_DUAL_CAMERA_REAR_PORTRAIT_SLAVE := CAMERA_ID_BACK_0
#BOARD_DUAL_CAMERA_FRONT_PORTRAIT_MASTER := CAMERA_ID_FRONT_0
#BOARD_DUAL_CAMERA_FRONT_PORTRAIT_SLAVE := CAMERA_ID_FRONT_1

BOARD_CAMERA_USES_EFD := false
BOARD_CAMERA_USES_3AA_DNG := true
BOARD_CAMERA_USES_SBWC := false
BOARD_CAMERA_USES_SLSI_VENDOR_TAGS := false
BOARD_CAMERA_USES_CLAHE := false
BOARD_CAMERA_USES_BAYER_GDC := false
BOARD_CAMERA_USES_YUVPP := false
BOARD_CAMERA_USES_EXYNOS_GDC := false
BOARD_CAMERA_USES_REMOSAIC_SENSOR := true
BOARD_CAMERA_USES_SW_REMOSAIC_SENSOR := true
ifneq ($(filter a33x% a53x% m33x% m34x% a25x% gta4xls% a26x%, $(TARGET_PRODUCT)),)
BOARD_CAMERA_USES_EXYNOS_MERTA := true
BOARD_CAMERA_USES_EXYNOS_LME := true
endif

#BOARD_CAMERA_USES_LLS_SOLUTION := true
#BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS := false
#BOARD_CAMERA_USES_EXYNOS_LSI_EIS := true
#BOARD_CAMERA_USES_SLSI_PLUGIN_V1 := true
BOARD_CAMERA_USES_SLSI_PLUGIN_O21 := true
BOARD_CAMERA_USES_EXYNOS_VPL := true
BOARD_CAMERA_USES_EFD := true
#BOARD_CAMERA_USES_EXYNOS_LEC := true
ifeq ($(filter gta4xls%, $(TARGET_PRODUCT)),)
BOARD_CAMERA_USES_EXYNOS_CNR := true
endif
#BOARD_CAMERA_BUILD_SOLUTION_SRC := true
#BOARD_CAMERA_USES_PIPE_HANDLER := true
#BOARD_CAMERA_USES_HIFI_LLS_CAPTURE := true
#BOARD_CAMERA_USES_HIFI_CAPTURE := true
#BOARD_CAMERA_USES_HWFC := false
BOARD_CAMERA_TARGET_CLASS := mass

# Modules
BOARD_MODULE_USES_GDC := false

BOARD_USES_METADATA_PARTITION := true
# Extra mount point
BOARD_ROOT_EXTRA_SYMLINKS += /mnt/vendor/efs:/efs
BOARD_ROOT_EXTRA_SYMLINKS += /mnt/vendor/persist:/persist

# Graphics
TARGET_RECOVERY_PIXEL_FORMAT := RGBX_8888

# Skia backend
TARGET_USES_VULKAN := true

# Gralloc implementation
BOARD_USES_EXYNOS_GRALLOC_VERSION := 5

BOARD_USES_GRALLOC_ION_SYNC := true

SOONG_CONFIG_NAMESPACES += arm_gralloc
SOONG_CONFIG_arm_gralloc := \
	gralloc_arm_no_external_afbc \
	mali_gpu_support_afbc_basic \
	gralloc_init_afbc \
	gralloc_ion_sync_on_lock \
	gralloc_product_vendor_version \
	dpu_support_1010102_afbc \
	mfc_chroma_valign \
	support_raw10_raw12_cam_wr \
	gralloc_supports_protected_buffer \
	gralloc_supports_rgba_fp16_afbc

SOONG_CONFIG_arm_gralloc_gralloc_arm_no_external_afbc := false
SOONG_CONFIG_arm_gralloc_mali_gpu_support_afbc_basic := true
SOONG_CONFIG_arm_gralloc_gralloc_init_afbc := true
SOONG_CONFIG_arm_gralloc_gralloc_ion_sync_on_lock := $(BOARD_USES_GRALLOC_ION_SYNC)
SOONG_CONFIG_arm_gralloc_dpu_support_1010102_afbc := true
SOONG_CONFIG_arm_gralloc_mfc_chroma_valign := true
SOONG_CONFIG_arm_gralloc_support_raw10_raw12_cam_wr := true
SOONG_CONFIG_arm_gralloc_gralloc_product_vendor_version := true
SOONG_CONFIG_arm_gralloc_gralloc_supports_protected_buffer := true
SOONG_CONFIG_arm_gralloc_gralloc_supports_rgba_fp16_afbc := false

# libExynosGraphicbuffer
SOONG_CONFIG_NAMESPACES += exynosgraphicbuffer
SOONG_CONFIG_exynosgraphicbuffer := \
	        gralloc_version

ifeq ($(BOARD_USES_EXYNOS_GRALLOC_VERSION),5)
	SOONG_CONFIG_exynosgraphicbuffer_gralloc_version := five
endif

# Add the system properties for Gralloc
TARGET_VENDOR_PROP += device/samsung/erd8825/arm.gralloc.usage.prop
TARGET_VENDOR_PROP += device/samsung/erd8825/arm.egl.config.prop

# Build a separate vendor_dlkm partition
BOARD_USES_VENDOR_DLKMIMAGE := true
BOARD_VENDOR_DLKMIMAGE_FILE_SYSTEM_TYPE := erofs
TARGET_COPY_OUT_VENDOR_DLKM := vendor_dlkm

# Build a separate system_dlkm partition
BOARD_USES_SYSTEM_DLKMIMAGE := true
BOARD_SYSTEM_DLKMIMAGE_FILE_SYSTEM_TYPE := erofs
TARGET_COPY_OUT_SYSTEM_DLKM := system_dlkm

# DisplayConfig xml
BOARD_DISPLAYCONFIG_VENDOR_XML_PATH := device/samsung/erd8825/displayconfig

########################
# Audio
########################
ifeq ($(BOARD_USES_GENERIC_AIDL_AUDIO), false)
include device/samsung/audio/erd8825_sec/BoardConfig-audio.mk
endif

# WLBT (wlbt firmware)
SOONG_CONFIG_NAMESPACES += wlbt
SOONG_CONFIG_wlbt := type
SOONG_CONFIG_wlbt_type := papaya_s620

# WiFi related defines
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
BOARD_HOSTAPD_DRIVER             := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_slsi
BOARD_HOSTAPD_PRIVATE_LIB        := lib_driver_cmd_slsi
BOARD_HAS_SAMSUNG_WLAN           := true
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WLAN_DEVICE                := slsi
WIFI_DRIVER_MODULE_ARG           := ""
WLAN_VENDOR                      := 8
SCSC_WLAN_DEVICE_NAME            := papaya_s620
WIFI_HIDL_FEATURE_DUAL_INTERFACE := true
WIFI_HIDL_FEATURE_AWARE          := true
SLSI_WIFI_HAL_NL_ATTR_CONFIG     := true
WIFI_FEATURE_HOSTAPD_11AX        := true

WIFI_HAL_INTERFACE_COMBINATIONS  := {{{STA}, 1}, {{P2P, NAN}, 1}}
WIFI_HAL_INTERFACE_COMBINATIONS  += , {{{STA}, 1}, {{AP}, 1}}

# Enable BT/WIFI related code changes in Android source files
CONFIG_SAMSUNG_SCSC_WIFIBT       := true
WIFI_HIDL_UNIFIED_SUPPLICANT_SERVICE_RC_ENTRY := true

# Bluetooth related defines
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_SLSI := true
SOONG_CONFIG_NAMESPACES += libbt_vendor
SOONG_CONFIG_libbt_vendor := slsi
SOONG_CONFIG_libbt_vendor_slsi := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := hardware/samsung_slsi/libbt/include
BLUEDROID_HCI_VENDOR_STATIC_LINKING := false

# Exynos RIL and telephony
# Multi SIM(DSDS)
SIM_COUNT := 2
SUPPORT_DS := true
# Support NR
SUPPORT_NR := true
SUPPORT_NR_DS := true
# Using IRadio 2.2
USE_RADIO_HAL_2_2 := true
# Support AIDL for SecureElement
SUPPORT_SE_AIDL := true

#Using OEM AIDL HAL
USE_RADIOEXTERNAL_HAL_AIDL := true
USE_OEMSERVICE_HAL_AIDL := true

USE_OEM_RIL_SERVICE := true
USE_TELEPHONY_INJECTION := true
USE_SYSTEMUI_SLSI := true

# BOARD_USES_EXYNOS_SENSORS_HAL := true

# # Options for CHUB Firmware build
# NANOHUB_SENSORHAL2 := true
# NANOHUB_TARGET := erd8825 all
# ifeq ($(NANOHUB_SENSORHAL2), true)
# NANOHUB_PATH := hardware/samsung_slsi/contexthub
# else
# NANOHUB_PATH := device/google/contexthub
# endif

########################
# Video Codec
########################
BOARD_NAME := erd8825
BOARD_MFC_FW_VERSION := v14.20

SOONG_CONFIG_NAMESPACES += vendor_c2_service
SOONG_CONFIG_vendor_c2_service := use_c2_api
SOONG_CONFIG_vendor_c2_service_use_c2_api := true

# since Android V, HAL must use AIDL
ifeq ($(ENABLE_V_FEATURE), true)
BOARD_USE_CODEC2_AIDL := V1
endif

ifeq ($(BOARD_USE_CODEC2_AIDL), )
BOARD_USE_CODEC2_HIDL_1_2 := true
endif

BOARD_USE_CSC_FILTER := true
BOARD_USE_DEC_SW_CSC := false
BOARD_USE_ENC_SW_CSC := false
BOARD_SUPPORT_MFC_ENC_RGB := false

BOARD_USE_PERFORMANCE := true
BOARD_USE_QUERY_HDR2SDR := false
BOARD_USE_COMPRESSED_COLOR := false
BOARD_USE_FRAMERATE_THRESH_HOLD := 5
BOARD_USE_GDC := false

#use MFC stat
BOARD_USE_HDR10PLUS_STAT_ENC := false

BOARD_UNSUPPORT_10BIT := true

# forcefully true
BOARD_USE_FULL_ST2094_40 := true

SOONG_CONFIG_NAMESPACES += exynos_headers_c2
SOONG_CONFIG_exynos_headers_c2 := full_st2094_40
SOONG_CONFIG_exynos_headers_c2_full_st2094_40 := true

ifeq ($(BOARD_USE_DEFAULT_SERVICE), false)
SOONG_CONFIG_NAMESPACES += vendor_c2_service
SOONG_CONFIG_vendor_c2_service := use_c2_api
SOONG_CONFIG_vendor_c2_service_use_c2_api := true
endif

# forcefully true
BOARD_USE_MFC_HEADER := true
SOONG_CONFIG_NAMESPACES += mfc
SOONG_CONFIG_mfc += version
SOONG_CONFIG_mfc_version := v14_20
BOARD_CAPACITY_BASELINE := 8294400

BOARD_USE_MAX_SECURE_RESOURCE := 2
BOARD_MFC_CHROMA_VALIGN := 32

BOARD_USE_SKYPE_CERTIFIED_VERSION := "3.2.2024.R29"

########################

# RECOVERY TG - VAB <
# https://source.android.com/docs/core/architecture/bootloader/partitions/generic-boot#option-1
# Devices launching with Android 12 must set BOARD_USES_RECOVERY_AS_BOOT to empty and use new configurations. If such devices:
# Don't use a dedicated recovery partition, the architecture is as shown in Figure 1 and the device setup option is Option 1.
# Option 1: No dedicated recovery partition
# BOARD CONFIG should be configs below for AB UPDATER
#BOARD_USES_RECOVERY_AS_BOOT :=
#BOARD_MOVE_RECOVERY_RESOURCES_TO_VENDOR_BOOT := true
#BOARD_USES_GENERIC_KERNEL_IMAGE := true
#BOARD_EXCLUDE_KERNEL_FROM_RECOVERY_IMAGE :=
#BOARD_MOVE_GSI_AVB_KEYS_TO_VENDOR_BOOT := true

ifeq ($(AB_OTA_UPDATER), true)
$(warning RECOVERY mode configs for AB_OTA_UPDATER)

# move recovery ramdisk to vendor_boot
BOARD_MOVE_RECOVERY_RESOURCES_TO_VENDOR_BOOT := true
#BOARD_INCLUDE_RECOVERY_RAMDISK_IN_VENDOR_BOOT := true

# remove /lib/modules in recovery ramdisk
BOARD_RECOVERY_KERNEL_MODULES :=
endif

PRODUCT_COPY_FILES += \
    device/samsung/common/recovery/init.recovery.samsung.rc:root/init.recovery.samsung.rc

ifeq ($(AB_OTA_UPDATER), true)
TARGET_RECOVERY_FSTAB := device/samsung/common/recovery/recovery.fstab.$(TARGET_SOC).ab
else
TARGET_RECOVERY_FSTAB := device/samsung/common/recovery/recovery.fstab
endif
# RECOVERY TG - VAB >
