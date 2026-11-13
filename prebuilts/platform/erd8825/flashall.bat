fastboot flash dtb dtb.img
fastboot flash dtbo dtbo.img
fastboot flash boot boot.img

fastboot flash vendor vendor.img -S 300M
fastboot flash system system.img -S 512M
fastboot flash vbmeta vbmeta.img
fastboot reboot -w
