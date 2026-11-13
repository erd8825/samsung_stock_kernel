# Sensor Hub HAL

PRODUCT_PACKAGES += \
	android.hardware.sensors-service.multihal \
	libsensorndkbridge

PRODUCT_PACKAGES += \
	sensors.sensorhub \
	sensors.inputvirtual

PRODUCT_COPY_FILES += \
	device/samsung/erd8825/sensorhub/init.sensorhub.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.sensorhub.rc

PRODUCT_COPY_FILES += \
	device/samsung/erd8825/firmware/sensorhub/shub_papaya_$(TARGET_DEVICE).bin:$(TARGET_COPY_OUT_VENDOR)/firmware/os.checked.bin