# Libacryl
SOONG_CONFIG_NAMESPACES += libacryl
SOONG_CONFIG_libacryl += default_scaler
SOONG_CONFIG_libacryl_default_scaler := mscl_votf

# JPEG HAL
PRODUCT_PACKAGES += \
    libhwjpeg
