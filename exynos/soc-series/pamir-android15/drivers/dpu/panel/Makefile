ifdef CONFIG_SOC_S5E9925
dpu-dir := exynos_p1
cal-dir := $(dpu-dir)/cal_pm
endif

subdir-ccflags-y	:= -I${ROOT_DIR}/${EXYNOS_SOC_DIR}/drivers/dpu				\
			   -I${ROOT_DIR}/${EXYNOS_SOC_DIR}/drivers/dpu/$(dpu-dir)		\
			   -I${ROOT_DIR}/${EXYNOS_SOC_DIR}/drivers/dpu/include			\
			   -I${ROOT_DIR}/${EXYNOS_SOC_DIR}/drivers/dpu/cal_common		\
			   -I${ROOT_DIR}/${EXYNOS_SOC_DIR}/drivers/dpu/$(cal-dir)

obj-$(CONFIG_DRM_PANEL_SAMSUNG_COMMON) += panel-samsung-drv.o
obj-$(CONFIG_DRM_PANEL_SAMSUNG_S6E3FAB) += panel-samsung-s6e3fab.o
obj-$(CONFIG_DRM_PANEL_SAMSUNG_S6E3FAC) += panel-samsung-s6e3fac.o
obj-$(CONFIG_DRM_PANEL_SAMSUNG_S6E3HAB) += panel-samsung-s6e3hab.o
obj-$(CONFIG_DRM_PANEL_SAMSUNG_S6E3FC3) += panel-samsung-s6e3fc3.o
obj-$(CONFIG_DRM_PANEL_SAMSUNG_S6E8FC1) += panel-samsung-s6e8fc1.o
obj-$(CONFIG_DRM_PANEL_SAMSUNG_EMUL) += panel-samsung-emul.o
