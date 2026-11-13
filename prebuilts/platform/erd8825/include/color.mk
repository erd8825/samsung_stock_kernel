########################################################
# Color configurations
########################################################

# DQE/HDR xmls
BOARD_DQE_VENDOR_XML_PATH := device/samsung/erd8825/dqe
BOARD_DQE_VENDOR_XML_SUBDIR := $(TARGET_PRODUCT:full_%=%)

# DQE libs
BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT0 := libdisplaycolor_default
#BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT1 :=
#BOARD_LIBDISPLAYCOLOR_UTC := true

# HDR/WCG libs
BOARD_LIBHDR_PLUGIN_MAIN := libhdr_plugin
#BOARD_LIBHDR_PLUGIN_SUB :=
BOARD_LIBHDR_META_PLUGIN := libhdr_meta_plugin_default
#BOARD_LIBHDR_UTC := true

########################################################
# Load xmls/libs by Color configurations
########################################################

# Load DQE/HDR xmls
ifneq ($(BOARD_DQE_VENDOR_XML_PATH),)
ifneq ($(BOARD_DQE_VENDOR_XML_SUBDIR),)
PRODUCT_COPY_FILES += $(foreach xml,\
       $(wildcard $(BOARD_DQE_VENDOR_XML_PATH)/$(BOARD_DQE_VENDOR_XML_SUBDIR)/*.xml),\
       $(xml):$(TARGET_COPY_OUT_VENDOR)/etc/dqe/$(notdir $(xml)))
endif
PRODUCT_COPY_FILES += $(foreach xml,\
       $(wildcard $(BOARD_DQE_VENDOR_XML_PATH)/*.xml),\
       $(xml):$(TARGET_COPY_OUT_VENDOR)/etc/dqe/$(notdir $(xml)))
endif

# Load DQE libs
ifneq ($(or $(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT0),$(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT1)),)
SOONG_CONFIG_NAMESPACES += libdisplaycolor

ifneq ($(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT0),)
SOONG_CONFIG_libdisplaycolor += slot0
SOONG_CONFIG_libdisplaycolor_slot0 := $(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT0)
endif

ifneq ($(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT1),)
SOONG_CONFIG_libdisplaycolor += slot1
SOONG_CONFIG_libdisplaycolor_slot1 := $(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT1)
endif

ifeq ($(BOARD_LIBDISPLAYCOLOR_UTC),true)
SOONG_CONFIG_libdisplaycolor += CS02 CS03 CS05
SOONG_CONFIG_libdisplaycolor_CS02 := true
SOONG_CONFIG_libdisplaycolor_CS03 := true
SOONG_CONFIG_libdisplaycolor_CS05 := true
endif
endif

PRODUCT_PACKAGES += \
       $(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT0)\
       $(BOARD_LIBDISPLAYCOLOR_PLUGIN_SLOT1)

# Load HDR/WCG libs
ifneq ($(BOARD_LIBHDR_PLUGIN_MAIN),)
SOONG_CONFIG_NAMESPACES += libhdr
SOONG_CONFIG_libhdr := main
SOONG_CONFIG_libhdr_main := $(BOARD_LIBHDR_PLUGIN_MAIN)

ifneq ($(BOARD_LIBHDR_PLUGIN_SUB),)
SOONG_CONFIG_libhdr += sub
SOONG_CONFIG_libhdr_sub := $(BOARD_LIBHDR_PLUGIN_SUB)
endif

ifneq ($(BOARD_LIBHDR_META_PLUGIN),)
SOONG_CONFIG_libhdr += meta_plugin
SOONG_CONFIG_libhdr_meta_plugin := $(BOARD_LIBHDR_META_PLUGIN)
endif

ifeq ($(BOARD_LIBHDR_UTC),true)
SOONG_CONFIG_libhdr += CS01 CS04 CS06
SOONG_CONFIG_libhdr_CS01 := true
SOONG_CONFIG_libhdr_CS04 := true
SOONG_CONFIG_libhdr_CS06 := true
endif
endif

PRODUCT_PACKAGES += \
       $(BOARD_LIBHDR_PLUGIN_MAIN)\
       $(BOARD_LIBHDR_PLUGIN_SUB)
