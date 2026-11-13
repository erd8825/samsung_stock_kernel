# AVB2.0, to tell PackageManager that the system supports Verified Boot(PackageManager.FEATURE_VERIFIED_BOOT)
ifeq ($(BOARD_AVB_ENABLE), true)
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.software.verified_boot.xml:system/etc/permissions/android.software.verified_boot.xml
endif

# WideVine modules
PRODUCT_PACKAGES += \
	android.hardware.drm@latest-service.clearkey

# WideVine modules
TARGET_BUILD_WIDEVINE := nonupdatable
TARGET_BUILD_WIDEVINE_USE_PREBUILT := false
-include vendor/widevine/libwvdrmengine/apex/device/device.mk

## SecureDRM modules
PRODUCT_PACKAGES += \
	liboemcrypto_modular


## tlwvdrm
#PRODUCT_COPY_FILES += \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/00060308060501020000000000000000.tabin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/00060308060501020000000000000000.tabin

# TUI
PRODUCT_COPY_FILES += \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/FFFF0000000000000000000000000014.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/FFFF0000000000000000000000000014.tlbin \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/070c0000000000000000000000000000.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/070c0000000000000000000000000000.tlbin

# MobiCore setup
ifeq ($(BOARD_MOBICORE_ENABLE),true)
PRODUCT_PACKAGES += \
	libMcClient \
	libMcRegistry \
	libgdmcprov \
	mcDriverDaemon \
	libTeeClient \
	tee_tlcm \
	tee_whitelist

	PRODUCT_SOONG_NAMESPACES += hardware/samsung_slsi/exynos/tee/kinibi520
	RODUCT_SOONG_NAMESPACES += vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp
else
# TEEgris
USE_BLOWFISH := true
TEEGRIS_VERSION := 4
PRODUCT_PACKAGES += \
	tzdaemon \
	tzts_daemon \
	libteecl \
	libuuid \
	libteecl.vendor \
	libuuid.vendor \
	libiwt.vendor \
	tzapp

PRODUCT_TRUSTZONE_ENABLED := true
PRODUCT_TRUSTZONE_TYPE := exynos9xxx
TEEGRIS_GATEKEEPER_ENABLE := true
PRODUCT_TEEGRIS_TUI_ENABLED := true
endif

##Gatekeeper
#ifeq ($(BOARD_TEE_GATEKEEPER_ENABLE), false)
#PRODUCT_PACKAGES += \
#	android.hardware.gatekeeper@1.0-service.software
#else # TEE GATEKEEPER
#PRODUCT_PACKAGES += \
#	android.hardware.gatekeeper@1.0-impl \
#	android.hardware.gatekeeper@1.0-service \
#	gatekeeper.$(TARGET_SOC)
#
## GateKeeper TA
#PRODUCT_COPY_FILES += \
#        vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/07061000000000000000000000000000.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/07061000000000000000000000000000.tlbin
#endif


## CryptoManager - tlcmdrv
#PRODUCT_COPY_FILES += \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/FFFFFFFFD00000000000000000000016.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/FFFFFFFFD00000000000000000000016.tlbin
#
## CryptoManager test app for eng build
#ifneq (,$(filter eng, $(TARGET_BUILD_VARIANT)))
## tlcmtest / cm_test
#PRODUCT_COPY_FILES += \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/04010000000000000000000000000000.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/04010000000000000000000000000000.tlbin \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/cm_test:$(TARGET_COPY_OUT_VENDOR)/app/cm_test
#endif


## RPMB TA - tlrpmb
#PRODUCT_COPY_FILES += \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/09090000070100010000000000000000.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/09090000070100010000000000000000.tlbin

## RPMB Unit Test - bRPMB
#PRODUCT_COPY_FILES += \
#	vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/bRPMB:$(TARGET_COPY_OUT_VENDOR)/app/bRPMB

## TEE_keymint
#ifeq ($(BOARD_TEE_KEYMINT_ENABLE), false)
#PRODUCT_PACKAGES += \
#	android.hardware.security.keymint-service
#else # TEE KEYMINT
#PRODUCT_PACKAGES += \
#    android.hardware.security.keymint@1.0_tee-service
#
#PRODUCT_SOONG_NAMESPACES += hardware/samsung_slsi/exynos/tee/TlcKeymint
#
## CTS require keystore attest key flag in package manager
#PRODUCT_COPY_FILES += \
#	frameworks/native/data/etc/android.hardware.keystore.app_attest_key.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.keystore.app_attest_key.xml \
#
#PRODUCT_COPY_FILES += \
#        vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp/0706000000000000000000000000004d.tlbin:$(TARGET_COPY_OUT_VENDOR)/app/mcRegistry/0706000000000000000000000000004d.tlbin
#endif

# SPL for AVB, KeyMint
VENDOR_SECURITY_PATCH := $(PLATFORM_SECURITY_PATCH)

# Gatekeeper
ifeq ($(shell secgetspf SEC_PRODUCT_FEATURE_SECURITY_SUPPORT_MDFPP_KEYMASTER), TRUE)
    PRODUCT_PACKAGES += \
        gatekeeper.$(TARGET_SOC)
endif
