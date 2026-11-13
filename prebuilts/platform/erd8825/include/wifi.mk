# WLAN configuration
# device specific wpa_supplicant.conf
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/wifi/p2p_supplicant.conf:vendor/etc/wifi/p2p_supplicant.conf \
    device/samsung/erd8825/wifi/init.insmod.wifi.cfg:/vendor/etc/wifi/init.insmod.wifi.cfg \
    device/samsung/erd8825/wifi/init.exynos.wifi.rc:/vendor/etc/init/init.exynos.wifi.rc

PRODUCT_PROPERTY_OVERRIDES += \
        wifi.interface=wlan0
PRODUCT_PROPERTY_OVERRIDES += \
        wifi.aware.interface=wifi-aware0

# Packages needed for WLAN
# Note android.hardware.wifi@1.0-service is used by HAL interface 1.2
PRODUCT_PACKAGES += \
    android.hardware.wifi-service \
    android.hardware.wifi.supplicant@1.1-service \
    android.hardware.wifi.offload@1.0-service \
    dhcpcd.conf \
    hostapd \
    wificond \
    wifilog \
    wpa_supplicant.conf \
    wpa_supplicant \
    wpa_cli \
    hostapd_cli

PRODUCT_PACKAGES += \
    libwifi-hal \
    libwifi-system \
    libwifikeystorehal \
    android.hardware.wifi.supplicant@1.2 \
    android.hardware.wifi@1.3

PRODUCT_COPY_FILES += device/samsung/erd8825/wpa_supplicant.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant.conf \
    frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.aware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.aware.xml \
    frameworks/native/data/etc/android.hardware.wifi.rtt.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.rtt.xml

#supplicant configs
ifneq ($(BOARD_WPA_SUPPLICANT_DRIVER),)
CONFIG_DRIVER_$(BOARD_WPA_SUPPLICANT_DRIVER) := y
endif
WPA_SUPPL_DIR = external/wpa_supplicant_8
include $(WPA_SUPPL_DIR)/wpa_supplicant/android.config