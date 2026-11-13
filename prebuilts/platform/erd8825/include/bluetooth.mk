# Bluetooth libbt namespace
PRODUCT_SOONG_NAMESPACES += hardware/samsung_slsi/libbt

# Bluetooth configuration
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    hardware/samsung_slsi/libbt/conf/bt_did.conf:$(TARGET_COPY_OUT_VENDOR)/etc/bluetooth/bt_did.conf \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml

# Bluetooth HAL
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.bt.bdaddr_path=/sys/module/scsc_bt/parameters/bluetooth_address_fallback

PRODUCT_PACKAGES += \
    android.hardware.bluetooth-service.default \
    android.hardware.bluetooth.audio-impl
# Set supported Bluetooth profiles to enabled
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.profile.asha.central.enabled=true \
    bluetooth.profile.a2dp.source.enabled=true \
    bluetooth.profile.avrcp.target.enabled=true \
    bluetooth.profile.bap.broadcast.assist.enabled=false \
    bluetooth.profile.bap.broadcast.source.enabled=false \
    bluetooth.profile.bap.unicast.client.enabled=false \
    bluetooth.profile.bas.client.enabled=false \
    bluetooth.profile.csip.set_coordinator.enabled=false \
    bluetooth.profile.gatt.enabled=true \
    bluetooth.profile.hap.client.enabled=true \
    bluetooth.profile.hfp.ag.enabled=true \
    bluetooth.profile.hid.device.enabled=true \
    bluetooth.profile.hid.host.enabled=true \
    bluetooth.profile.map.server.enabled=true \
    bluetooth.profile.mcp.server.enabled=false \
    bluetooth.profile.opp.enabled=true \
    bluetooth.profile.pan.nap.enabled=true \
    bluetooth.profile.pan.panu.enabled=true \
    bluetooth.profile.pbap.server.enabled=true \
    bluetooth.profile.pbap.sim.enabled=true \
    bluetooth.profile.ccp.server.enabled=false \
    bluetooth.profile.vcp.controller.enabled=false \
    bluetooth.profile.sap.server.enabled=false

# A2DP Offload
PRODUCT_PROPERTY_OVERRIDES += \
    ro.bluetooth.a2dp_offload.supported=true \
    persist.bluetooth.a2dp_offload.disabled=true

# LE Audio Offload
PRODUCT_PROPERTY_OVERRIDES += \
    ro.bluetooth.leaudio_offload.supported=false \
    persist.bluetooth.leaudio_offload.disabled=true

# Support A2dp Dual Audio feature
PRODUCT_PROPERTY_OVERRIDES += \
    ro.bluetooth.a2dp.dual_audio.supported=false

# Set the Bluetooth Class of Device
# Service Field: 0x5A -> 90
#    Bit 17: Networking
#    Bit 19: Capturing
#    Bit 20: Object Transfer
#    Bit 22: Telephony
# MAJOR_CLASS: 0x02 -> 2 (Phone)
# MINOR_CLASS: 0x0C -> 12 (Smart Phone)
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.device.class_of_device=90,2,12

# LE Audio dynamic switcher
PRODUCT_PROPERTY_OVERRIDES += \
    ro.bluetooth.leaudio_switcher.supported=false \
    ro.bluetooth.leaudio_broadcast_switcher.supported=false \
    persist.bluetooth.leaudio_switcher.disabled=true

# Support LeAudio connection by default without allow list
PRODUCT_PROPERTY_OVERRIDES += \
    persist.bluetooth.leaudio.bypass_allow_list=false

# Enable Dual mode audio
PRODUCT_PROPERTY_OVERRIDES += \
    persist.bluetooth.enable_dual_mode_audio=false

# LE audio group turn to idle during call notify
PRODUCT_PROPERTY_OVERRIDES += \
    persist.bluetooth.leaudio.notify.idle.during.call=false

# LE Audio dual mode preferred audio to OUTPUT ONLY = A2DP / DUPLEX = HFP
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.device.default_output_only_audio_profile=2 \
    bluetooth.device.default_duplex_audio_profile=1

# LE Audio instant audio suspend timeout
PRODUCT_PROPERTY_OVERRIDES += \
    persist.bluetooth.leaudio.audio.suspend.timeoutms=1000

# LE Audio disallow metadata multiple contexts
PRODUCT_PROPERTY_OVERRIDES += \
    persist.bluetooth.leaudio.allow.multiple.contexts=false

# PRODUCT_PROPDUCT_PROPERTIES for fwsnoop
PRODUCT_PRODUCT_PROPERTIES += \
    persist.bluetooth.fwsnoop_supported=false

# Enable HFP v1.9 SWB feature
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.hfp.swb.supported=false

# Support max 15 LE concurrent_connections
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.core.le.max_number_of_concurrent_connections=15

# Set configured HCI timeout milliseconds
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.hci.timeout_milliseconds=5000
