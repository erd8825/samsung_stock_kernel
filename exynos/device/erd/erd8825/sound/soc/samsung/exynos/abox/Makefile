subdir-ccflags-y += -I${ROOT_DIR}/${EXYNOS_SOC_DIR}/sound/soc/samsung/exynos/abox

snd-soc-samsung-abox-sync-aw-$(CONFIG_SEC_SND_SYNCHRONIZED_IPC_AW) += abox_synchronized_ipc_aw.o
snd-soc-samsung-abox-sync-gdx-$(CONFIG_SEC_SND_SYNCHRONIZED_IPC_GDX) += abox_synchronized_ipc_gdx.o

obj-$(CONFIG_SEC_SND_SYNCHRONIZED_IPC_AW) += snd-soc-samsung-abox-sync-aw.o
obj-$(CONFIG_SEC_SND_SYNCHRONIZED_IPC_GDX) += snd-soc-samsung-abox-sync-gdx.o