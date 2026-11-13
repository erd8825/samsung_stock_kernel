// SPDX-License-Identifier: GPL-2.0
/*
 * COPYRIGHT(C) 2026 Samsung Electronics Co., Ltd. All Right Reserved.
 */

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s() " fmt, __func__

#include <soc/samsung/cal-if.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/sec_class.h>

static u32 cal_id_cpucl1;
static u32 cal_id_g3d;

static struct device *reloc_gpio_dev;
static int gpio_num;

static ssize_t check_requested_gpio_show(struct device *sec_class_dev,
		struct device_attribute *attr, char *buf)
{
	int val, ret;

	if (gpio_num < 0) {
		val = -ENODEV;
		goto invalid_gpio;
	}

	val = gpio_get_value(gpio_num);

invalid_gpio:
	ret = scnprintf(buf, PAGE_SIZE, "GPIO[%d] : [%d]\n", gpio_num, val);
	gpio_num = -EINVAL;
	return ret;
}

static ssize_t check_requested_gpio_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int err;

	err = kstrtoint(buf, 10, &gpio_num);
	if (err < 0) {
		dev_err(dev, "requested gpio number is malformed or wrong\n");
		return err;
	}

	return count;
}

static DEVICE_ATTR(check_requested_gpio, 0664,
		check_requested_gpio_show, check_requested_gpio_store);

static ssize_t asv_info_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	int count = 0;

	/* Set asv group info to buf */
	count += sprintf(&buf[count], "%d ", cal_asv_get_tablever());
	count += sprintf(&buf[count], "%03x ", cal_asv_get_grp(cal_id_cpucl1));
	count += sprintf(&buf[count], "%03x ", cal_asv_get_grp(cal_id_g3d));
	count += sprintf(&buf[count], "%u ", 0U);
	count += sprintf(&buf[count], "%u ", 0U);
	count += sprintf(&buf[count], "\n");

	return count;
}

static DEVICE_ATTR_RO(asv_info);

static int sec_reloc_gpio_probe(struct platform_device *pdev)
{
	int err;
	struct device *dev;

	/* Create node for asv_info */
	err = of_property_read_u32(pdev->dev.of_node, "cal_id_cpucl1", &cal_id_cpucl1);
	if (err) {
		pr_err("%s: failed to read cal_id_cpucl1 (err: %d)", __func__, err);
		goto asv_fail;
	}

	err = of_property_read_u32(pdev->dev.of_node, "cal_id_g3d", &cal_id_g3d);
	if (err) {
		pr_err("%s: failed to read cal_id_g3d (err: %d)", __func__, err);
		goto asv_fail;
	}
	
	dev = sec_device_create(NULL, "asv");
	BUG_ON(!dev);
	if (IS_ERR(dev)) {
		pr_err("%s: failed to create sec device\n", __func__);
		goto asv_fail;
	}

	if (device_create_file(dev, &dev_attr_asv_info) < 0)
		pr_err("%s: failed to create sysfs file\n", __func__);
	
asv_fail:
	/* Create node for check_requested_gpio */
	reloc_gpio_dev = sec_device_create(NULL, "gpio");
	if (IS_ERR(reloc_gpio_dev)) {
		dev_err(&pdev->dev, "Failed to create sec_device");
		err = PTR_ERR(reloc_gpio_dev);
		goto sec_device_fail;
	}

	err = device_create_file(reloc_gpio_dev, &dev_attr_check_requested_gpio);
	if (err) {
		dev_err(&pdev->dev, "Failed to create sysfs");
		goto sysfs_fail;
	}

	return 0;

sysfs_fail:
	sec_device_destroy(reloc_gpio_dev->devt);
sec_device_fail:
	return err;
}

static void sec_reloc_gpio_remove(struct platform_device *pdev)
{
	sec_device_destroy(reloc_gpio_dev->devt);
}

static const struct of_device_id sec_reloc_gpio_match_table[] = {
	{ .compatible = "samsung,reloc_gpio" },
	{},
};
MODULE_DEVICE_TABLE(of, sec_reloc_gpio_match_table);

static struct platform_driver sec_reloc_gpio_driver = {
	.driver = {
		.name = "samsung,reloc_gpio",
		.of_match_table = of_match_ptr(sec_reloc_gpio_match_table),
	},
	.probe = sec_reloc_gpio_probe,
	.remove = sec_reloc_gpio_remove,
};

module_platform_driver(sec_reloc_gpio_driver);

MODULE_AUTHOR("Samsung Electronics");
MODULE_DESCRIPTION("Legacy-Style Relocated GPIO Interface for Factory Mode");
MODULE_LICENSE("GPL v2");

