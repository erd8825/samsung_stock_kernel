include $(KERNEL_SRC)/../${EXYNOS_BASE_DIR}/external-modules/bsp/sec_key_notifier/6.6/Makefile.include

modules modules_install:
	$(MAKE) M=$(M) -C $(KERNEL_SRC) \
	KBUILD_EXTRA_SYMBOLS="$(EXTRA_SYMBOLS)" $(@)

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean

