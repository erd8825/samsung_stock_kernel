# firmware signing
ifeq ($(SEC_BUILD_OPTION_KNOX_CSB),true)
type=${VBMETA_TYPE}
project=${SEC_BUILD_CONF_MODEL_SIGNING_NAME}
SECURE_SCRIPT=../buildscript/tools/signclient.jar

UNSIGN_SENSORHUB := device/samsung/erd8825/firmware/sensorhub/shub_papaya_$(TARGET_DEVICE)_unchecked.bin
SIGN_SENSORHUB := device/samsung/erd8825/firmware/sensorhub/shub_papaya_$(TARGET_DEVICE).bin
RENAMING_SENSORHUB := $(shell mv $(SIGN_SENSORHUB) $(UNSIGN_SENSORHUB))
SIGNNING_SENSORHUB := $(shell java -jar $(SECURE_SCRIPT) -runtype ${type} -model ${project} -input $(UNSIGN_SENSORHUB) -output $(SIGN_SENSORHUB))
endif

SENSOR_HAL := SENSOR_HUB
SENSOR_MCU := LSI

USE_SENSOR_MULTI_HAL := true
SENSOR_INPUT_VIRTUAL_HAL := true