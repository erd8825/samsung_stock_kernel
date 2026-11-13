#
# Makefile for BTS device driver
#

obj-$(CONFIG_EXYNOS_BTS)	+= exynos-bts.o

# Chipset Dependent code
ifdef CONFIG_SOC_S5E9925
obj-$(CONFIG_EXYNOS_BTS)	+= exynos-btsops9925.o
endif
