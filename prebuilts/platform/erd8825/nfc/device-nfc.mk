#############################################################################################
## Includes NFC Packages
#############################################################################################

# Modify it to project environment
LOCAL_PATH                     := device/samsung/erd8825
BOARD_USES_SAMSUNG_NFC_CHIP    := sn4v

$(warning "Building S.LSI SEC NFC packages")
# HAL
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/nfc/init.nfc.sec.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.nfc.sec.rc \
    $(LOCAL_PATH)/nfc/bin/sec_s3nsn4v_firmware.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/sec_s3nsn4v_firmware.bin \
    $(LOCAL_PATH)/nfc/bin/sec_s3nsn4v_hwreg.bin:$(TARGET_COPY_OUT_VENDOR)/etc/sec_s3nsn4v_hwreg.bin \
    $(LOCAL_PATH)/nfc/bin/sec_s3nsn4v_swreg.bin:$(TARGET_COPY_OUT_VENDOR)/etc/sec_s3nsn4v_swreg.bin
#    $(LOCAL_PATH)/nfc/bin/sec_s3nrnsn4v_firmware_v_6_2_15_20210202.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/sec_s3nsn4v_firmware.bin \
#    $(LOCAL_PATH)/nfc/bin/sec_s3nrnsn4v_hwreg_M52_5G_EUR_6_2_15_01_20210506.bin:$(TARGET_COPY_OUT_VENDOR)/etc/sec_s3nsn4v_hwreg.bin \
#    $(LOCAL_PATH)/nfc/bin/sec_s3nsen4_firmware_global_v_6_1_3_20201123.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/sec_s3nsen4_firmware.bin \
#    $(LOCAL_PATH)/nfc/bin/sec_s3nsen4_hwreg_HubbleZ3_EUR_6_1_3_11_20201123.bin:$(TARGET_COPY_OUT_VENDOR)/etc/sec_s3nsen4_hwreg.bin \
#    $(LOCAL_PATH)/nfc/bin/sec_s3nsen4_swreg_Hubble_EUR_6_1_3_11_20201123.bin:$(TARGET_COPY_OUT_VENDOR)/etc/sec_s3nsen4_swreg.bin


# Feature files + configuration
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/nfc/configs/libnfc-sec-vendor_debug_$(BOARD_USES_SAMSUNG_NFC_CHIP).conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-sec-vendor.conf
#    $(LOCAL_PATH)/nfc/configs/libnfc-nci_debug.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nci.conf \

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/android.hardware.nfc.hcef.xml:system/etc/permissions/android.hardware.nfc.hcef.xml \
    frameworks/native/data/etc/android.hardware.nfc.uicc.xml:system/etc/permissions/android.hardware.nfc.uicc.xml \
    frameworks/native/data/etc/android.hardware.nfc.ese.xml:system/etc/permissions/android.hardware.nfc.ese.xml \
    $(LOCAL_PATH)/nfc/android.software.nfc.beam.xml:system/etc/permissions/android.software.nfc.beam.xml

# HIDL service
PRODUCT_PACKAGES += \
    nfc_nci_sec \
    android.hardware.nfc@1.2-service.sec \
    sec_nfc_test

# NFC stack and app service
PRODUCT_PACKAGES += \
    libnfc-sec \
    libnfc_sec_jni \
    NfcSec \
    Tag \
#    com.android.nfc_extras
