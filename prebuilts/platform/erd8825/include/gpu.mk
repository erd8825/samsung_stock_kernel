ifneq ($(MALI_ON_KEYSTONE),true)

MALI_VERSION ?= r49p1

ifneq ($(MALI_BUILD),true)
PRODUCT_SOONG_NAMESPACES += vendor/samsung_slsi/$(TARGET_SOC)/libs/${MALI_VERSION}
PRODUCT_PACKAGES += \
	libGLES_mali \
	mali_symlink \
	vulkan.mali_prebuilt \
	libgpudataproducer
endif

PRODUCT_PROPERTY_OVERRIDES += \
	ro.hardware.vulkan=mali \
	ro.hardware.egl = mali

ifeq ($(BOARD_USES_EXYNOS_AFBC_FEATURE), true)
PRODUCT_PROPERTY_OVERRIDES += \
        ro.vendor.ddk.set.afbc=1
endif

#Enable supporting AGI(Android GPU Inspector)
PRODUCT_PROPERTY_OVERRIDES += \
	graphics.gpu.profiler.support=true

#Gralloc Libs
ifeq ($(BOARD_USES_EXYNOS_GRALLOC_VERSION),5)
PRODUCT_SOONG_NAMESPACES += hardware/samsung_slsi/exynos/gralloc5
PRODUCT_PACKAGES += \
	android.hardware.graphics.allocator-V2-service \
	android.hardware.graphics.allocator-V2-aidl-impl \
	mapper.stableC-impl-mali \
	arm.graphics_v2-V1-ndk
endif

# whitelist for public libraries such as libOpenCL.so
PRODUCT_SOONG_NAMESPACES += vendor/samsung_slsi/$(TARGET_SOC_BASE)/secapp
PRODUCT_PACKAGES += whitelist


# FEATURE_OPENGLES_EXTENSION_PACK support string config file
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml

# vulkan version information
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.vulkan.compute-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.compute.xml \
	frameworks/native/data/etc/android.hardware.vulkan.level-1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level.xml \
	frameworks/native/data/etc/android.hardware.vulkan.version-1_3.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version.xml

# OpenGL ES and Vulkan DEQP version information
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.software.opengles.deqp.level-2024-03-01.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.opengles.deqp.level.xml \
	frameworks/native/data/etc/android.software.vulkan.deqp.level-2024-03-01.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.vulkan.deqp.level.xml

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=196610

endif
