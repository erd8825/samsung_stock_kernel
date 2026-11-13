modules modules_install:
	$(MAKE) M=$(M) -C $(KERNEL_SRC) \
	KBUILD_EXTRA_SYMBOLS="$(EXTRA_SYMBOLS)" $(@)

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) clean

