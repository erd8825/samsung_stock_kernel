adb reboot fastboot

fastboot flash bootloader out/target/product/erd8825/bootloader.img
fastboot flash dtb out/target/product/erd8825/dtb.img
fastboot flash dtbo out/target/product/erd8825/dtbo.img
fastboot flash boot out/target/product/erd8825/boot.img

fastboot flash vendor out/target/product/erd8825/vendor.img -S 300M
fastboot flash system out/target/product/erd8825/system.img -S 512M
fastboot flash vbmeta out/target/product/erd8825/vbmeta.img
fastboot reboot -w
