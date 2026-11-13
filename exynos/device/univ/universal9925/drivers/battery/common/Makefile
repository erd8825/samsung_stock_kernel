obj-$(CONFIG_DIRECT_CHARGING_MODULE)		+= sec_direct_charger.o
obj-$(CONFIG_SEC_PD_MODULE)				+= sec_pd.o

obj-$(CONFIG_BATTERY_SAMSUNG_SM5714_MODULE)	+= sm/

obj-$(CONFIG_BATTERY_SAMSUNG_MODULE)	+= sec_battery_m.o
sec_battery_m-y			:= sec_battery.o sec_battery_vote.o sec_battery_thermal.o sec_battery_sysfs.o sec_battery_dt.o sec_adc.o
sec_battery_m-$(CONFIG_BATTERY_CISD)	+= sec_cisd.o
sec_battery_m-$(CONFIG_STEP_CHARGING)		+= sec_step_charging.o
sec_battery_m-$(CONFIG_WIRELESS_AUTH)		+= sec_battery_misc.o

obj-$(CONFIG_UPDATE_BATTERY_DATA)	+= sec_battery_data.o

ccflags-y := -Wformat
