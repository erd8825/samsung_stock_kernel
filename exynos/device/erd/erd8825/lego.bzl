
# SPDX-License-Identifier: GPL-2.0
# COPYRIGHT(C) 2023 Samsung Electronics Co., Ltd. All Right Reserved.

lego_module_list_first = [

]

lego_module_list_second = [
"drivers/sensorhub/shub.ko",
"drivers/sensorhub/utility/sensor_core.ko",
"drivers/sensors_lego/isg6320.ko"
]

lego_module_list = [
"drivers/battery/charger/nu2111a_charger/nu2111a_charger.ko",
"drivers/battery/charger/s2mf301_charger/s2mf301_charger.ko",
"drivers/battery/charger/s2mf301_charger/s2mf301_pmeter.ko",
"drivers/battery/common/sb_wireless.ko",
"drivers/battery/common/sec-battery.ko",
"drivers/battery/common/sec-direct-charger.ko",
"drivers/battery/common/sec_pd.ko",
"drivers/battery/core/sb-core.ko",
"drivers/battery/fuelgauge/s2mf301_fuelgauge/s2mf301_fuelgauge.ko",
"drivers/dpu/panel/mcd-panel-samsung-drv.ko",
"drivers/dpu/panel/mcd-panel-samsung-helper.ko",
"drivers/fingerprint/fingerprint.ko",
"drivers/fingerprint/fingerprint_sysfs.ko",
"drivers/gpu/drm/samsung/panel/db7f2010b/mcd-panel-db7f2010b_a26x.ko",
"drivers/gpu/drm/samsung/panel/mcd-panel.ko",
"drivers/gpu/drm/samsung/panel/oled_common/usdm-panel-oled-common.ko",
"drivers/gpu/drm/samsung/panel/s6e8fc3/mcd-panel-s6e8fc3_a26x.ko",
"drivers/input/input_boost/input_booster_lkm.ko",
"drivers/input/sec_input/sec_common_fn.ko",
"drivers/input/sec_input/sec_input_notifier.ko",
"drivers/input/touchscreen/focaltech/ft3519/focaltech_ts_ft3519.ko",
"drivers/kperfmon/kperfmon.ko",
"drivers/kq/mesh/mesh.ko",
"drivers/mfd/slsi/s2mf301/mfd_s2mf301.ko",
"drivers/misc/drb/dev_ril_bridge.ko",
"drivers/muic/common/common_muic.ko",
"drivers/muic/slsi/platform/muic_platform.ko",
"drivers/muic/slsi/s2mf301/muic_s2mf301.ko",
"drivers/muic/slsi/s2mf301/top_s2mf301.ko",
"drivers/net/dropdump/dropdump.ko",
"drivers/nfc/nfc_stm_sec.ko",
"drivers/nfc/snvm/snvm.ko",
"drivers/sec_panel_notifier_v2/sec_panel_notifier_v2.ko",
"drivers/staging/android/switch/sec_switch_class.ko",
"drivers/sti/abc/abc.ko",
"drivers/sti/abc/abc_hub.ko",
"drivers/usb/common/vbus_notifier/vbus_notifier.ko",
"drivers/usb/notify/usb_notifier.ko",
"drivers/usb/notify/usb_notify_layer.ko",
"drivers/usb/typec/common/pdic_notifier_module.ko",
"drivers/usb/typec/manager/if_cb_manager.ko",
"drivers/usb/typec/manager/usb_typec_manager.ko",
"drivers/usb/typec/slsi/common/s2m_pdic_notifier_module.ko",
"drivers/usb/typec/slsi/s2mf301/pdic_s2mf301.ko",
"drivers/vibrator/common/sec_vibrator.ko",
"drivers/vibrator/common/vib_info/vibrator_vib_info.ko",
"drivers/vibrator/dc/dc_vibrator.ko"
]

lego_dtbo_list = [
"a26xs_ltn_open_vince_6.6_w00_r05.dtbo",
"a26xs_ltn_open_vince_6.6_w00_r04.dtbo",
"a26xs_ltn_open_vince_6.6_w00_r02.dtbo",
"a26xs_ltn_open_vince_6.6_w00_r00.dtbo"
]
lego_model = 'a26xs'