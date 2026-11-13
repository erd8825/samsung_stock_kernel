# Add efs to ROOT
BOARD_ROOT_EXTRA_FOLDERS += efs

# Build a separate odm.img partition
BOARD_ODMIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_COPY_OUT_ODM := odm
BOARD_GROUP_BASIC_PARTITION_LIST += odm

# Common rc files
PRODUCT_COPY_FILES += \
	device/samsung/common/init.rc:$(TARGET_COPY_OUT_ODM)/etc/init/init.rc \
	device/samsung/common/ueventd.rc:$(TARGET_COPY_OUT_ODM)/etc/ueventd.rc

BOARD_BOOTCONFIG := buildtime_bootconfig=enable \
		    androidboot.serialconsole=0
