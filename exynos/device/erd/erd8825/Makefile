# SPDX-License-Identifier: GPL-2.0

include $(KERNEL_SRC)/../${EXYNOS_SOC_DIR}/Makefile.include
include $(KERNEL_SRC)/../${EXYNOS_BASE_DIR}/external-modules/bsp/class/Makefile.include
include $(KERNEL_SRC)/../${EXYNOS_BASE_DIR}/external-modules/bsp/reboot/6.6/Makefile.include
include $(KERNEL_SRC)/../${EXYNOS_BASE_DIR}/external-modules/bsp/debug/6.6_fvc/Makefile.include
include $(KERNEL_SRC)/../${EXYNOS_BASE_DIR}/external-modules/camera/Makefile.include

modules modules_install:
	$(MAKE) M=$(M) -C $(KERNEL_SRC) \
	KBUILD_EXTRA_SYMBOLS="$(EXTRA_SYMBOLS)" $(@)

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean

