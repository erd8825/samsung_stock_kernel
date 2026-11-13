# Camera HAL
PRODUCT_PACKAGES += \
    vendor.samsung.hardware.camera.provider-service_64 \
    camera.$(TARGET_SOC)

# Copy FIMC_IS DDK Libraries
ifneq ($(filter full_erd8825_v,$(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/evt0/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/evt0/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/evt0/setfile_2l4.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_2l4.bin \
    device/samsung/erd8825/firmware/camera/evt0/setfile_3h1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_3h1.bin \
    device/samsung/erd8825/firmware/camera/evt0/setfile_3m5.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_3m5.bin \
    device/samsung/erd8825/firmware/camera/evt0/setfile_3p9.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_3p9.bin
else ifneq ($(filter a33x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/a33x/is_fw_mcu.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_fw_mcu.bin \
    device/samsung/erd8825/firmware/camera/a33x/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/a33x/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_imx582.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx582.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_4ha.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_4ha.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_imx355.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx355.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_imx258.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx258.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_3l6.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_3l6.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_gc02m1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1.bin \
    device/samsung/erd8825/firmware/camera/a33x/setfile_gc5035.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc5035.bin \
    device/samsung/erd8825/firmware/camera/a33x/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/a33x/dual_cal_wide_sub.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_sub.bin
else ifneq ($(filter a53x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/a53x/is_fw_mcu.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_fw_mcu.bin \
    device/samsung/erd8825/firmware/camera/a53x/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/a53x/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_imx682.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx682.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_imx616.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx616.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_imx258.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx258.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_gc5035d.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc5035d.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_gc5035m.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc5035m.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_hi1336.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_hi1336.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_gd2.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gd2.bin \
    device/samsung/erd8825/firmware/camera/a53x/setfile_gw1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gw1.bin \
    device/samsung/erd8825/firmware/camera/a53x/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/a53x/dual_cal_wide_sub.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_sub.bin
else ifneq ($(filter m33x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/m33x/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/m33x/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/m33x/setfile_jn1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_jn1.bin \
    device/samsung/erd8825/firmware/camera/m33x/setfile_imx355.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx355.bin \
    device/samsung/erd8825/firmware/camera/m33x/setfile_gc5035.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc5035.bin \
    device/samsung/erd8825/firmware/camera/m33x/setfile_gc02m1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1.bin \
    device/samsung/erd8825/firmware/camera/m33x/setfile_gc02m1b.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1b.bin\
    device/samsung/erd8825/firmware/camera/m33x/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/m33x/dual_cal_wide_sub.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_sub.bin
else ifneq ($(filter m34x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/m34x/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/m34x/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/m34x/setfile_jn1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_jn1.bin \
    device/samsung/erd8825/firmware/camera/m34x/setfile_imx355.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx355.bin \
    device/samsung/erd8825/firmware/camera/m34x/setfile_gc5035.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc5035.bin \
    device/samsung/erd8825/firmware/camera/m34x/setfile_gc02m1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1.bin \
    device/samsung/erd8825/firmware/camera/m34x/setfile_gc02m2.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m2.bin \
    device/samsung/erd8825/firmware/camera/m34x/setfile_gc02m1b.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1b.bin\
    device/samsung/erd8825/firmware/camera/m34x/setfile_hi1339.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_hi1339.bin\
    device/samsung/erd8825/firmware/camera/m34x/setfile_4ha.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_4ha.bin\
    device/samsung/erd8825/firmware/camera/m34x/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/m34x/dual_cal_wide_swide.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_swide.bin
else ifneq ($(filter gts8l%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/gts8l/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/gts8l/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin
else ifneq ($(filter a25x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/a25x/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/a25x/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/a25x/setfile_jn1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_jn1.bin \
    device/samsung/erd8825/firmware/camera/a25x/setfile_gc02m1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1.bin \
    device/samsung/erd8825/firmware/camera/a25x/setfile_hi1339.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_hi1339.bin \
    device/samsung/erd8825/firmware/camera/a25x/setfile_imx258.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx258.bin \
    device/samsung/erd8825/firmware/camera/a25x/setfile_4ha.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_4ha.bin \
    device/samsung/erd8825/firmware/camera/a25x/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/a25x/dual_cal_wide_swide.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_swide.bin 
else ifneq ($(filter a26x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/a26x/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/a26x/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/a26x/setfile_jn1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_jn1.bin \
    device/samsung/erd8825/firmware/camera/a26x/setfile_gc02m1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc02m1.bin \
    device/samsung/erd8825/firmware/camera/a26x/setfile_imx258.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_imx258.bin \
    device/samsung/erd8825/firmware/camera/a26x/setfile_4ha.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_4ha.bin \
    device/samsung/erd8825/firmware/camera/a26x/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/a26x/dual_cal_wide_swide.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_swide.bin
else ifneq ($(filter gta4xls%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/gta4xlsxx/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/gta4xlsxx/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/gta4xlsxx/setfile_gc5035.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_gc5035.bin \
    device/samsung/erd8825/firmware/camera/gta4xlsxx/setfile_4ha.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_4ha.bin \
    device/samsung/erd8825/firmware/camera/gta4xlsxx/SetMultiCalInfo.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/SetMultiCalInfo.bin \
    device/samsung/erd8825/firmware/camera/gta4xlsxx/dual_cal_wide_swide.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/dual_cal_wide_swide.bin 
else
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/is_lib.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_lib.bin \
    device/samsung/erd8825/firmware/camera/is_rta.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/is_rta.bin \
    device/samsung/erd8825/firmware/camera/setfile_2l4.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_2l4.bin \
    device/samsung/erd8825/firmware/camera/setfile_3j1.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/setfile_3j1.bin
endif

# Camera feature
ifneq ($(filter gta4xls%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.xml
else
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.xml
endif

# Copy Camera HFD Setfiles
#PRODUCT_COPY_FILES += \
    device/samsung/universal9925/firmware/camera/libhfd/default_configuration.hfd.cfg.json:$(TARGET_COPY_OUT_VENDOR)/firmware/default_configuration.hfd.cfg.json \
    device/samsung/universal9925/firmware/camera/libhfd/pp_cfg.json:$(TARGET_COPY_OUT_VENDOR)/firmware/pp_cfg.json \
    device/samsung/universal9925/firmware/camera/libhfd/tracker_cfg.json:$(TARGET_COPY_OUT_VENDOR)/firmware/tracker_cfg.json \
    device/samsung/universal9925/firmware/camera/libhfd/WithLightFixNoBN.SDNNmodel:$(TARGET_COPY_OUT_VENDOR)/firmware/WithLightFixNoBN.SDNNmodel

# vpl files
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libvpl/default_configuration.flm.cfg.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/default_configuration.flm.cfg.bin \
    device/samsung/erd8825/firmware/camera/libvpl/secure_configuration.flm.cfg.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/secure_configuration.flm.cfg.bin \
    device/samsung/erd8825/firmware/camera/libvpl/OD_V2.1.6_01_26_QVGA.nnc:$(TARGET_COPY_OUT_VENDOR)/firmware/OD_V2.1.6_01_26_QVGA.nnc \
    device/samsung/erd8825/firmware/camera/libvpl/OD_V2.3.7_02_05_VGA.nnc:$(TARGET_COPY_OUT_VENDOR)/firmware/OD_V2.3.7_02_05_VGA.nnc

# SWLME TNR capture
ifneq ($(filter a33x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_GC02M1B.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC02M1B.json \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_GC5035.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC5035.json \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_IMX258.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX258.json \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_IMX355.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX355.json \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_S5K3L6.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_S5K3L6.json \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_S5K4HA.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_S5K4HA.json \
    device/samsung/erd8825/firmware/camera/libLME/A33/lmecpu_IMX582.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX582.json 
else ifneq ($(filter a53x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_GC5035.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC5035.json \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_IMX258.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX258.json \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_IMX616.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX616.json \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_HI1336.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_HI1336.json \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_S5KGD2.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_S5KGD2.json \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_S5KGW1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_S5KGW1.json \
    device/samsung/erd8825/firmware/camera/libLME/A53/lmecpu_IMX682.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX682.json 
else ifneq ($(filter m33x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/M33/lmecpu_GC02M1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC02M1.json \
    device/samsung/erd8825/firmware/camera/libLME/M33/lmecpu_GC5035.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC5035.json \
    device/samsung/erd8825/firmware/camera/libLME/M33/lmecpu_IMX355.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX355.json \
    device/samsung/erd8825/firmware/camera/libLME/M33/lmecpu_JN1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_JN1.json 
else ifneq ($(filter m34x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/M34/lmecpu_4HA.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_4HA.json \
    device/samsung/erd8825/firmware/camera/libLME/M34/lmecpu_GC02M2.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC02M2.json \
    device/samsung/erd8825/firmware/camera/libLME/M34/lmecpu_HI1339.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_HI1339.json \
    device/samsung/erd8825/firmware/camera/libLME/M34/lmecpu_JN1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_JN1.json 
else ifneq ($(filter a25x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/A25/lmecpu_4HA.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_4HA.json \
    device/samsung/erd8825/firmware/camera/libLME/A25/lmecpu_GC02M1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC02M1.json \
    device/samsung/erd8825/firmware/camera/libLME/A25/lmecpu_HI1339.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_HI1339.json \
    device/samsung/erd8825/firmware/camera/libLME/A25/lmecpu_IMX258.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX258.json \
    device/samsung/erd8825/firmware/camera/libLME/A25/lmecpu_JN1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_JN1.json 
else ifneq ($(filter a26x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/A26/lmecpu_4HA.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_4HA.json \
    device/samsung/erd8825/firmware/camera/libLME/A26/lmecpu_GC02M1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC02M1.json \
    device/samsung/erd8825/firmware/camera/libLME/A26/lmecpu_IMX258.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_IMX258.json \
    device/samsung/erd8825/firmware/camera/libLME/A26/lmecpu_JN1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_JN1.json 
else ifneq ($(filter gta4xls%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libLME/GTA4XLS/lmecpu_S5K4HA.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_S5K4HA.json \
    device/samsung/erd8825/firmware/camera/libLME/GTA4XLS/lmecpu_GC5035.json:$(TARGET_COPY_OUT_VENDOR)/firmware/lmecpu_GC5035.json
endif

# swcnr files
ifeq ($(BOARD_CAMERA_USES_EXYNOS_CNR),true)
ifneq ($(filter a53x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libswcnr/a53x/swcnr_IMX682.json:$(TARGET_COPY_OUT_VENDOR)/firmware/swcnr_IMX682.json \
    device/samsung/erd8825/firmware/camera/libswcnr/a53x/swcnr_S5KGW1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/swcnr_S5KGW1.json
else ifneq ($(filter a33x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libswcnr/a33x/swcnr_IMX582.json:$(TARGET_COPY_OUT_VENDOR)/firmware/swcnr_IMX582.json
else ifneq ($(filter m33x%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libswcnr/m33x/swcnr_S5KJN1.json:$(TARGET_COPY_OUT_VENDOR)/firmware/swcnr_S5KJN1.json
else ifneq ($(filter gta4xls%, $(TARGET_PRODUCT)),)
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libswcnr/gta4xls/swcnr_S5K4HA.json:$(TARGET_COPY_OUT_VENDOR)/firmware/swcnr_S5K4HA.json
else
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/firmware/camera/libswcnr/swcnr_SAK2L4.json:$(TARGET_COPY_OUT_VENDOR)/firmware/swcnr_SAK2L4.json
endif
endif

# MFC firmware overwrite
PRODUCT_COPY_FILES += \
	device/samsung/erd8825/firmware/mfc_fw_v14.20.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/mfc_fw.bin

# Camera
PRODUCT_COPY_FILES += \
    device/samsung/erd8825/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml

$(call inherit-product-if-exists, vendor/samsung_slsi/exynos/camera/O21/hal3/src/camera.mk)
$(call inherit-product-if-exists, hardware/samsung_slsi/s5e8825/s5e8825.mk)