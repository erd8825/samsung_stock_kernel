ifeq ($(BOARD_USES_EXYNOS_GNSS_HAL), true)

#only GPS libraries and binaries to the target directory
GPS_ROOT := device/samsung/erd8825/gnss_binaries/release

PRODUCT_COPY_FILES += \
    $(GPS_ROOT)/ca.pem:vendor/etc/gnss/ca.pem \
    $(GPS_ROOT)/gps.cfg:vendor/etc/gnss/gps.cfg \
    $(GPS_ROOT)/gnssd:vendor/bin/hw/gpsd \
    $(GPS_ROOT)/vendor_samsung_hardware_gnss_1_0-impl.so:vendor/lib64/hw/vendor.samsung.hardware.gnss@1.0-impl.so \
    $(GPS_ROOT)/android_hardware_gnss_2_1-impl.so:vendor/lib64/hw/android.hardware.gnss@2.1-impl.so

endif
