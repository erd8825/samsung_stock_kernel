# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Linux Wireless network device drivers.
#
clean-files := built-in.o

subdir-ccflags-y += -I$(srctree)/$(src) \
		    -I$(srctree)/$(src)/include \
		    -I${ROOT_DIR}/${KERNEL_DIR}/include

obj-$(CONFIG_SCSC_WLAN) += scsc/
