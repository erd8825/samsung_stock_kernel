/*
 * drivers/samsung/sec_reboot.c
 *
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Implementation of Exynos specific power domain control which is used in
 * conjunction with runtime-pm.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/of.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <soc/samsung/debug-snapshot.h>

extern void cache_flush_all(void);

#define PS_HOLD_EN                              (1 << 8)

/* MINFORM */
#define SEC_REBOOT_START_OFFSET		(24)
#define SEC_REBOOT_END_OFFSET		(16)

enum sec_power_flags {
	SEC_REBOOT_DEFAULT = 0x30,
	SEC_REBOOT_NORMAL = 0x4E,
	SEC_REBOOT_LPM = 0x70,
};

#define SEC_DUMPSINK_MASK 0x0000FFFF

/* PANIC INFORM */
#define SEC_RESET_REASON_PREFIX 0x12345670
#define SEC_RESET_SET_PREFIX    0xabc00000
enum sec_reset_reason {
	SEC_RESET_REASON_UNKNOWN   = (SEC_RESET_REASON_PREFIX | 0x0),
	SEC_RESET_REASON_DOWNLOAD  = (SEC_RESET_REASON_PREFIX | 0x1),
	SEC_RESET_REASON_UPLOAD    = (SEC_RESET_REASON_PREFIX | 0x2),
	SEC_RESET_REASON_CHARGING  = (SEC_RESET_REASON_PREFIX | 0x3),
	SEC_RESET_REASON_RECOVERY  = (SEC_RESET_REASON_PREFIX | 0x4),
	SEC_RESET_REASON_FOTA      = (SEC_RESET_REASON_PREFIX | 0x5),
	SEC_RESET_REASON_FOTA_BL   = (SEC_RESET_REASON_PREFIX | 0x6), /* update bootloader */
	SEC_RESET_REASON_SECURE    = (SEC_RESET_REASON_PREFIX | 0x7), /* image secure check fail */
	SEC_RESET_REASON_FWUP      = (SEC_RESET_REASON_PREFIX | 0x9), /* emergency firmware update */
	SEC_RESET_REASON_EM_FUSE   = (SEC_RESET_REASON_PREFIX | 0xa), /* EMC market fuse */
	SEC_RESET_REASON_BOOTLOADER   = (SEC_RESET_REASON_PREFIX | 0xd), /* go to download mode */
	SEC_RESET_REASON_EMERGENCY = 0x0,

	SEC_RESET_SET_FORCE_UPLOAD = (SEC_RESET_SET_PREFIX | 0x40000),
	SEC_RESET_SET_DEBUG        = (SEC_RESET_SET_PREFIX | 0xd0000),
	SEC_RESET_SET_SWSEL        = (SEC_RESET_SET_PREFIX | 0xe0000),
	SEC_RESET_SET_SUD          = (SEC_RESET_SET_PREFIX | 0xf0000),
	SEC_RESET_CP_DBGMEM        = (SEC_RESET_SET_PREFIX | 0x50000), /* cpmem_on: CP RAM logging */
#if defined(CONFIG_SEC_ABC)
	SEC_RESET_USER_DRAM_TEST   = (SEC_RESET_SET_PREFIX | 0x60000), /* USER DRAM TEST */
#endif
#if defined(CONFIG_SEC_SYSUP)
	SEC_RESET_SET_PARAM   = (SEC_RESET_SET_PREFIX | 0x70000),
#endif
	SEC_RESET_SET_DUMPSINK	   = (SEC_RESET_SET_PREFIX | 0x80000),
};

static struct regmap *pmureg;
static u32 reboot_offset;
static u32 magic_inform;
static u32 panic_inform;

void sec_set_reboot_magic(int magic, int offset, int mask)
{
	u32 tmp = 0;

	regmap_read(pmureg, magic_inform, &tmp);
	pr_info("%s: prev: %x\n", __func__, tmp);
	mask <<= offset;
	tmp &= (~mask);
	tmp |= magic << offset;
	pr_info("%s: set as: %x\n", __func__, tmp);
	regmap_write(pmureg, magic_inform, tmp); 
}

static void sec_power_off(void)
{
	dbg_snapshot_scratch_clear();
        pr_info("%s: Exynos power off does not support.\n", __func__);
//	exynos_pmu_update(PS_HOLD_CONTROL, PS_HOLD_EN, 0);
	regmap_update_bits(pmureg, reboot_offset, PS_HOLD_EN, 0);
	while (1)
//		cpu_do_idle();
		wfi();
}

static int sec_reboot(struct notifier_block *this,
				unsigned long mode, void *cmd)
{
	local_irq_disable();

	pr_emerg("%s (%d, %s)\n", __func__, reboot_mode, cmd ? cmd : "(null)");

	//secdbg_base_clear_magic_rambase();

	/* LPM mode prevention */
	sec_set_reboot_magic(SEC_REBOOT_NORMAL, SEC_REBOOT_END_OFFSET, 0xFF);

	if (cmd) {
		unsigned long value;
		if (!strcmp(cmd, "fota"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_FOTA);
		else if (!strcmp(cmd, "fota_bl"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_FOTA_BL);
		else if (!strcmp(cmd, "recovery"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_RECOVERY);
		else if (!strcmp(cmd, "download"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_DOWNLOAD);
		else if (!strcmp(cmd, "bootloader"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_BOOTLOADER);
		else if (!strcmp(cmd, "upload"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_UPLOAD);
		else if (!strcmp(cmd, "secure"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_SECURE);
		else if (!strcmp(cmd, "fwup"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_FWUP);
		else if (!strcmp(cmd, "em_mode_force_user"))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_EM_FUSE);
#if defined(CONFIG_SEC_ABC)
		else if (!strcmp(cmd, "user_dram_test") && sec_abc_get_enabled())
			regmap_write(pmureg, panic_inform, SEC_RESET_USER_DRAM_TEST);
#endif
		else if (!strncmp(cmd, "emergency", 9))
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_EMERGENCY);
		else if (!strncmp(cmd, "debug", 5) && !kstrtoul(cmd + 5, 0, &value))
			regmap_write(pmureg, panic_inform, SEC_RESET_SET_DEBUG | value);
		else if (!strncmp(cmd, "dump_sink", 9) && !kstrtoul(cmd + 9, 0, &value))
			regmap_write(pmureg, panic_inform, SEC_RESET_SET_DUMPSINK | (SEC_DUMPSINK_MASK & value));
		else if (!strncmp(cmd, "forceupload", 11) && !kstrtoul(cmd + 11, 0, &value))
			regmap_write(pmureg, panic_inform, SEC_RESET_SET_FORCE_UPLOAD | value);
		else if (!strncmp(cmd, "swsel", 5) && !kstrtoul(cmd + 5, 0, &value))
			regmap_write(pmureg, panic_inform, SEC_RESET_SET_SWSEL | value);
		else if (!strncmp(cmd, "sud", 3) && !kstrtoul(cmd + 3, 0, &value))
			regmap_write(pmureg, panic_inform, SEC_RESET_SET_SUD | value);
#if defined(CONFIG_SEC_SYSUP)
		else if (!strncmp(cmd, "param", 5))
			regmap_write(pmureg, panic_inform, SEC_RESET_SET_PARAM);
#endif
		else if (!strncmp(cmd, "cpmem_on", 8))
			regmap_write(pmureg, panic_inform, SEC_RESET_CP_DBGMEM | 0x1);
		else if (!strncmp(cmd, "cpmem_off", 9))
			regmap_write(pmureg, panic_inform, SEC_RESET_CP_DBGMEM | 0x2);
		else if (!strncmp(cmd, "mbsmem_on", 9))
			regmap_write(pmureg, panic_inform, SEC_RESET_CP_DBGMEM | 0x1);
		else if (!strncmp(cmd, "mbsmem_off", 10))
			regmap_write(pmureg, panic_inform, SEC_RESET_CP_DBGMEM | 0x2);
		else if (!strncmp(cmd, "panic", 5)) {
			/*
			 * This line is intentionally blanked because the PANIC INFORM is used for upload cause
			 * in sec_debug_set_upload_cause() only in case of  panic() .
			 */
		} else
			regmap_write(pmureg, panic_inform, SEC_RESET_REASON_UNKNOWN);
	} else {
		regmap_write(pmureg, panic_inform, SEC_RESET_REASON_UNKNOWN);
	}

	cache_flush_all();

	return NOTIFY_DONE;
}

static struct notifier_block sec_restart_nb = {
	.notifier_call = sec_reboot,
	.priority = 130,
};

static int sec_reboot_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	int err;

	pmureg = syscon_regmap_lookup_by_phandle(dev->of_node,
						"samsung,syscon-phandle");
	if (IS_ERR(pmureg)) {
		pr_err("%s: failed to get regmap of PMU\n", __func__);
		return PTR_ERR(pmureg);
	}

	if (of_property_read_u32(np, "reboot-offset", &reboot_offset) < 0) {
		pr_err("%s: failed to find reboot-offset property\n", __func__);
		return -EINVAL;
	}

	if (of_property_read_u32(np, "magic-inform", &magic_inform) < 0) {
		pr_err("%s: failed to find magic-inform property\n", __func__);
		return -EINVAL;
	}

	if (of_property_read_u32(np, "panic-inform", &panic_inform) < 0) {
		pr_err("%s: failed to find panic-inform property\n", __func__);
		return -EINVAL;
	}
/*
	if (of_property_read_u32(np, "reboot-trigger", &reboot_trigger) < 0) {
		pr_err("failed to find reboot-trigger property\n");
		return -EINVAL;
	}
*/
	err = register_restart_handler(&sec_restart_nb);
	if (err) {
		dev_err(&pdev->dev, "cannot register restart handler (err=%d)\n",
			err);
	}
	pm_power_off = sec_power_off;

	dev_info(&pdev->dev, "register restart handler successfully\n");

	return err;
}

static const struct of_device_id sec_reboot_of_match[] = {
	{ .compatible = "samsung,sec-reboot" },
	{}
};

static struct platform_driver sec_reboot_driver = {
	.probe = sec_reboot_probe,
	.driver = {
		.name = "sec-reboot",
		.of_match_table = sec_reboot_of_match,
	},
};
module_platform_driver(sec_reboot_driver);

MODULE_DESCRIPTION("Samsung Reboot driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:sec-reboot");
