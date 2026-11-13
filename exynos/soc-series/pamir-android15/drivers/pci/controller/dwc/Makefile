# SPDX-License-Identifier: GPL-2.0
obj-$(CONFIG_PCI_EXYNOS_HOST) += pcie-exynos-rc-core.o
ifdef CONFIG_PCIE_EXYNOS_DWPHY
pcie-exynos-rc-core-objs += pcie-exynos-rc-dwphy.o pcie-exynos-dbg-dwphy.o
else
pcie-exynos-rc-core-objs += pcie-exynos-rc.o pcie-exynos-dbg.o
endif
ifdef CONFIG_PCI_EXYNOS_S5E9925_S5300
pcie-exynos-rc-core-objs += pcie-exynosS5E9925-rc-cal_s5300.o
else
pcie-exynos-rc-core-objs += pcie-exynosS5E9925-rc-cal.o
endif