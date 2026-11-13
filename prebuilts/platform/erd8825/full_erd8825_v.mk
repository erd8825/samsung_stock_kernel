# Copyright (C) 2011 The Android Open Source Project
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
# This file is the build configuration for a full Android
# build for erd8825 hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps). Except for a few implementation
# details, it only fundamentally contains two inherit-product
# lines, full and erd8825, hence its name.
#

# Check System uses Virtual AB system
EXYNOS_USE_VIRTUAL_AB := false

# Check Exynos Product support dedicated recovery image
# For Non-AB  -> Recovery Image is mandatory
# For AB      -> Recovery Image is optional
ifneq ($(EXYNOS_USE_VIRTUAL_AB), true)
EXYNOS_USE_DEDICATED_RECOVERY_IMAGE := true
endif

ENABLE_V_FEATURE := true

# cbd
#PRODUCT_COPY_FILES += \
	device/samsung/erd8825/conf/init.erd8825.cbd.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.erd8825.cbd.rc

# Live Wallpapers
PRODUCT_PACKAGES += \
        LiveWallpapers \
        LiveWallpapersPicker \
        MagicSmokeWallpapers \
        VisualizationWallpapers \
        librs_jni

PRODUCT_PROPERTY_OVERRIDES := \
        net.dns1=8.8.8.8 \
        net.dns2=8.8.4.4

# Do not build system image if WITH_ESSI specified as true
ifeq ($(WITH_ESSI),true)
PRODUCT_BUILD_SYSTEM_IMAGE := false
PRODUCT_BUILD_SYSTEM_OTHER_IMAGE := false
PRODUCT_BUILD_VENDOR_IMAGE := true
PRODUCT_BUILD_PRODUCT_IMAGE := false
PRODUCT_BUILD_PRODUCT_SERVICES_IMAGE := false
PRODUCT_BUILD_ODM_IMAGE := false
PRODUCT_BUILD_CACHE_IMAGE := false
PRODUCT_BUILD_RAMDISK_IMAGE := true
PRODUCT_BUILD_USERDATA_IMAGE := false
# If Target Support dedicated reocvery image, set property to build recovery image
ifeq ($(EXYNOS_USE_DEDICATED_RECOVERY_IMAGE), true)
PRODUCT_BUILD_RECOVERY_IMAGE := true
endif
PRODUCT_BUILD_BOOT_IMAGE := true
PRODUCT_BUILD_VBMETA_IMAGE := false

# Also, since we're going to skip building the system image, we also skip
# building the OTA package. We'll build this at a later step. We also don't
# need to build the OTA tools package (we'll use the one from the system build).
TARGET_SKIP_OTA_PACKAGE := true
TARGET_SKIP_OTATOOLS_PACKAGE := true
endif

#include vendor/samsung/hardware/wifi/base_wifi.mk
#include vendor/samsung/hardware/wifi/broadcom/bcm4375/wifi.mk

# Inherit from those products. Most specific first.
$(call inherit-product, device/samsung/erd8825/device.mk)
# $(call inherit-product, build/target/product/aosp_arm64.mk)

# DEVICE_PACKAGE_OVERLAYS += device/samsung/erd8825/overlay_notch

PRODUCT_BOARD := erd8825
TARGET_DEVICE_NAME := erd8825
TARGET_SOC := s5e8825
TARGET_SOC_BASE := s5e8825
TARGET_BOOTLOADER_BOARD_NAME := s5e8825
TARGET_BOARD_PLATFORM := universal8825

PRODUCT_NAME := full_erd8825_v
PRODUCT_DEVICE := erd8825
PRODUCT_BRAND := Exynos
PRODUCT_MODEL := Full Android on ERD8825
PRODUCT_MANUFACTURER := Samsung Electronics Co., Ltd.
TARGET_LINUX_KERNEL_VERSION := 5.10
SB_SIGN_TYPE := 3
SB_KEY_TYPE := 1
SB_RB_COUNT := 0
SB_RAMDISK_SIZE := 2097152
TARGET_BUILD_KERNEL_FROM_SOURCE := true

TARGET_BOARD_SUPPORT_FEATURE := V
