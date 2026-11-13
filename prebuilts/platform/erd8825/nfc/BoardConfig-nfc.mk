###########################################################################################
## Includes SEPolicy and device manifest
###########################################################################################

# Modify it to project environment
LOCAL_PATH := device/samsung/erd8825

# SAMSUNG NFC
BOARD_USES_SAMSUNG_NFC           := true
BOARD_USES_SAMSUNG_NFC_DTA       := true
BOARD_USES_SAMSUNG_NFC_DTA_64    := true

#BOARD_VENDOR_SEPOLICY_DIRS       += $(LOCAL_PATH)/nfc/sepolicy/vendor
#BOARD_PLAT_PRIVATE_SEPOLICY_DIR  += $(LOCAL_PATH)/nfc/sepolicy/private

DEVICE_MANIFEST_FILE             += $(LOCAL_PATH)/nfc/manifest-nfc.xml

$(warning "Uses BOARD_USES_SAMSUNG_NFC in BoardConfig")
