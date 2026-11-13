PRODUCT_PACKAGES += \
    android.hardware.graphics.composer3-service.exynos

# HWC properties
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.hwc.exynos.vsync_mode=0 \
    debug.hwc.winupdate=1

# HWComposer
BOARD_HWC_VERSION := hwc3
HWC_SKIP_VALIDATE := false
#BOARD_USES_DISPLAYPORT := true
#BOARD_USES_EXTERNAL_DISPLAY_POWERMODE := true
BOARD_USES_DISPLAY_COLOR_INTERFACE := true
BOARD_USES_HWC_CPU_PERF_MODE := false
BOARD_USES_HWC_SERVICES := true
BOARD_USES_HWC_FORCE_GPU := false
BOARD_USES_EXYNOS_AFBC_FEATURE := true
ifeq ($(EXYNOS_EMULATOR_SUPPORT), true)
BOARD_DISABLE_WATCHDOG := true
endif
TARGET_HAS_WIDE_COLOR_DISPLAY := true
TARGET_HAS_HDR_DISPLAY := true

ifeq ($(BOARD_USES_DISPLAY_COLOR_INTERFACE), true)
HWC_SUPPORT_COLOR_TRANSFORM := true
TARGET_USES_DISPLAY_RENDER_INTENTS := true
endif

# DisplayConfig xml

# hw composer property : Properties related to hwc will be defined in hwcomposer_property.mk
$(call inherit-product-if-exists, hardware/samsung_slsi/graphics/base/hwcomposer_property.mk)
