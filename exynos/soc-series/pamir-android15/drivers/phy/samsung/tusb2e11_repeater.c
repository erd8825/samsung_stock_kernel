// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 * http://www.samsungsemi.com/
 *
 * Core file for Samsung TUSB2E11 driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "tusb2e11_repeater.h"

struct tusb2e11_data *g_tud;

int tusb2e11_i2c_write(struct tusb2e11_data *tud, u8 reg, u8 *data, int len)
{
	u8 buf[I2C_WRITE_BUFFER_SIZE + 1];
	int ret;
	unsigned char retry;
	struct i2c_msg msg;

	if (len > I2C_WRITE_BUFFER_SIZE) {
		dev_err(&tud->client->dev, "%s: len is larger than buffer size\n", __func__);
		return -EINVAL;
	}

	buf[0] = reg;
	memcpy(buf + 1, data, len);

	msg.addr = tud->client->addr;
	msg.flags = 0;
	msg.len = len + 1;
	msg.buf = buf;
	mutex_lock(&tud->i2c_mutex);
	for (retry = 0; retry < TUSB_I2C_RETRY_CNT; retry++) {
		ret = i2c_transfer(tud->client->adapter, &msg, 1);
		if (ret == 1)
			break;

		usleep_range(1 * 1000, 1 * 1000);

		if (retry > 1) {
			dev_err(&tud->client->dev, "%s: I2C retry %d\n", __func__, retry + 1);
			tud->comm_err_count++;
		}
	}

	mutex_unlock(&tud->i2c_mutex);

	if (ret == 1)
		return 0;

	return -EIO;
}

int tusb2e11_i2c_read(struct tusb2e11_data *tud, u8 reg, u8 *data, int len)
{
	u8 buf[4];
	int ret;
	unsigned char retry;
	struct i2c_msg msg[2];

	if (!len) {
		dev_err(&tud->client->dev,
			"%s: I2c message length is wrong!\n", __func__);
		goto err;
	}

	buf[0] = reg;

	msg[0].addr = tud->client->addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = buf;

	msg[1].addr = tud->client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = len;
	msg[1].buf = data;

	mutex_lock(&tud->i2c_mutex);

	for (retry = 0; retry < TUSB_I2C_RETRY_CNT; retry++) {
		ret = i2c_transfer(tud->client->adapter, msg, 2);
		if (ret == 2)
			break;
		usleep_range(1 * 1000, 1 * 1000);

		if (retry > 1) {
			dev_err(&tud->client->dev, "%s: I2C retry %d\n", __func__, retry + 1);
			tud->comm_err_count++;
		}
	}

	mutex_unlock(&tud->i2c_mutex);

	return ret;
err:
	return -EIO;
}

static int tusb2e11_write_reg(struct tusb2e11_data *tud, u8 reg, u8 *data, int len)
{
	int ret = 0;

	ret = tusb2e11_i2c_write(tud, reg, data, len);
	if (ret < 0) {
		pr_info("%s: reg(0x%x), ret(%d)\n",
			__func__, reg, ret);
	}
	return ret;
}

static int tusb2e11_read_reg(struct tusb2e11_data *tud, u8 reg, u8 *data, int len)
{
	int ret = 0;

	ret = tusb2e11_i2c_read(tud, reg, data, len);
	if (ret < 0) {
		pr_info("%s: reg(0x%x), ret(%d)\n",
			__func__, reg, ret);
		return ret;
	}
	ret &= 0xff;

	return ret;
}

static int tusb2e11_fill_tune_param(struct tusb2e11_data *tud,
				struct device_node *node)
{
	struct device *dev = tud->dev;
	struct device_node *child = NULL;
	struct tusb2e11_tune_param *tune_param;
	size_t size = sizeof(struct tusb2e11_tune_param);
	int ret;
	u32 res[2];
	u32 idx = 0;
	const char *name;

	ret = of_property_read_u32_array(node, "repeater_tune_cnt", &res[0], 1);
	tud->tune_cnt = res[0];

	dev_info(dev, "%s repeater tune cnt = %d\n", __func__, res[0]);

	tune_param = devm_kzalloc(dev, size * (res[0] + 1), GFP_KERNEL);
	if (!tune_param)
		return -ENOMEM;

	tud->tune_param = tune_param;

	for_each_child_of_node(node, child) {
		ret = of_property_read_string(child, "tune_name", &name);
		if (ret == 0) {
			memcpy(tune_param[idx].name, name, strlen(name));
		} else {
			dev_err(dev, "failed to read tune name from %s node\n", child->name);
			return ret;
		}
		ret = of_property_read_u32_array(child, "tune_value", res, 2);
		if (ret == 0) {
			tud->tune_param[idx].reg = res[0];
			tud->tune_param[idx].value = res[1];
		} else {
			dev_err(dev, "failed to read tune value from %s node\n", child->name);
			return -EINVAL;
		}
		dev_dbg(dev, "%s, tune name = %s, reg = 0x%x, value = 0x%x\n",
			 __func__, tud->tune_param[idx].name,
			 tud->tune_param[idx].reg, tud->tune_param[idx].value);

		idx++;
	}

	tune_param[idx].value = EXYNOS_USB_TUNE_LAST;

	return 0;
}

static int tusb2e11_ctrl_off(void)
{
	struct tusb2e11_data *tud = g_tud;
	int ret = 0;

	if (gpio_is_valid(tud->gpio_eusb_ctrl_sel)) {
		dev_info(tud->dev, "Succeeded to get gpio_eusb_ctrl_sel\n");
		ret = gpio_direction_output(tud->gpio_eusb_ctrl_sel, 0);
		if (ret) {
			dev_err(tud->dev, "ctrl_sel off failed\n");
			return -EIO;
		}
		tud->ctrl_sel_status = false;
		return 0;
	} else {
		dev_err(tud->dev, "Failed to get gpio_eusb_ctrl_sel (%d)\n",
		       tud->gpio_eusb_ctrl_sel);
	}
	return -ENODEV;
}

static int tusb2e11_ctrl_on(void)
{
	struct tusb2e11_data *tud = g_tud;
	int ret = 0;

	if (gpio_is_valid(tud->gpio_eusb_ctrl_sel)) {
		dev_info(tud->dev, "Succeeded to get gpio_eusb_ctrl_sel\n");
		ret = gpio_direction_output(tud->gpio_eusb_ctrl_sel, 1);
		if (ret) {
			dev_err(tud->dev, "ctrl_sel on failed\n");
			return -EIO;
		}
		tud->ctrl_sel_status = true;
		return 0;
	} else {
		dev_err(tud->dev, "Failed to get gpio_eusb_ctrl_sel (%d)\n",
		       tud->gpio_eusb_ctrl_sel);
	}
	return -ENODEV;
}

static ssize_t
tusb2e11_repeater_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct tusb2e11_data *tud = dev_get_drvdata(dev);
	int len = 0, i;

	len += snprintf(buf + len, PAGE_SIZE, "\t==== Print Repeater Tune Value ====\n");
	len += snprintf(buf + len, PAGE_SIZE, "Tune value count : %d\n", tud->tune_cnt);

	for (i = 0; i < tud->tune_cnt; i++) {
		len += snprintf(buf + len, PAGE_SIZE, "%s\t\t\t: 0x%x\n",
				tud->tune_param[i].name,
				tud->tune_param[i].value);
	}

	return len;
}

static ssize_t
tusb2e11_repeater_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t n)
{
	char tune_name[30];
	u32 tune_val;
	struct tusb2e11_data *tud = dev_get_drvdata(dev);
	int ret, i;
	u8 read_data, write_data;

	if (sscanf(buf, "%s %x", tune_name, &tune_val) != 2)
		return -EINVAL;

	if (tud->ctrl_sel_status == false) {
		tusb2e11_ctrl_on();
		mdelay(3);
	}

	for (i = 0; i < tud->tune_cnt; i++) {
		if (!strncmp(tud->tune_param[i].name, tune_name,
			strlen(tud->tune_param[i].name))) {
			write_data = (u8)tune_val;
			ret = tusb2e11_write_reg(tud, (u8)tud->tune_param[i].reg,
						 &write_data, 1);
			if (ret < 0)
				dev_info(&tud->client->dev, "%s: i2c write error\n", __func__);

			ret = tusb2e11_read_reg(tud, (u8)tud->tune_param[i].reg,
						&read_data, 1);
			if (ret < 0)
				dev_info(&tud->client->dev, "%s: i2c read error\n", __func__);

			tud->tune_param[i].value = read_data;

			dev_info(&tud->client->dev, "%s, 0x%x = 0x%x\n", tud->tune_param[i].name,
				 tud->tune_param[i].reg, read_data);

		}
	}

	return n;
}
static DEVICE_ATTR_RW(tusb2e11_repeater);

static struct attribute *tusb2e11_repeater_attrs[] = {
	&dev_attr_tusb2e11_repeater.attr,
	NULL
};
ATTRIBUTE_GROUPS(tusb2e11_repeater);

static int tusb2e11_parse_dt(struct device *dev, struct tusb2e11_data *tud,
				     struct tusb2e11_plat_data *pdata)
{
	struct device_node *np = dev->of_node;
	struct device_node *tune_node;
	int ret = 0;

	if (np == NULL) {
		pr_err("%s np is NULL\n", __func__);
		return -1;
	}

	tune_node = of_parse_phandle(dev->of_node, "repeater_tune_param", 0);
	if (tune_node != NULL) {
		ret = tusb2e11_fill_tune_param(tud, tune_node);
		if (ret < 0) {
			dev_err(dev, "can't fill repeater tuning param\n");
			return -EINVAL;
		}
	} else
		dev_info(dev, "don't need repeater tuning param\n");

	tud->gpio_eusb_ctrl_sel = of_get_named_gpio(np, "eusb,gpio_eusb_ctrl", 0);

	return 0;
}

static const struct of_device_id tusb2e11_match_table[] = {
		{ .compatible = "samsung,tusb2e11-repeater",},
		{},
};

int tusb2e11_power_off(void)
{
	struct tusb2e11_data *tud = g_tud;

	if (!tud)
		return -EEXIST;

	tusb2e11_ctrl_off();
	return 0;
}
EXPORT_SYMBOL_GPL(tusb2e11_power_off);

int tusb2e11_power_on(void)
{
	struct tusb2e11_data *tud = g_tud;
	u8 read_data, write_data;
	int ret, i;

	if (!tud)
		return -EEXIST;

	tusb2e11_ctrl_on();
	mdelay(3);

	ret = tusb2e11_read_reg(tud, 0x50,
				&read_data, 1);
	if (ret < 0) {
		dev_info(tud->dev, "%s: i2c read error\n", __func__);
		goto err;
	}
	pr_info("usb: %s : check eusb reg(0x50) = %x\n", __func__, read_data);

	for (i = 0; i < tud->tune_cnt; i++) {
		write_data = (u8)tud->tune_param[i].value;
		ret = tusb2e11_write_reg(tud, (u8)tud->tune_param[i].reg,
					 &write_data, 1);
		if (ret < 0) {
			dev_info(tud->dev, "%s: i2c read error\n", __func__);
			goto err;
		}

		ret = tusb2e11_read_reg(tud, tud->tune_param[i].reg,
					&read_data, 1);
		if (ret < 0) {
			dev_info(tud->dev, "%s: i2c read error\n", __func__);
			goto err;
		}
		tud->tune_param[i].value = read_data;
	}

	dev_info(tud->dev,
		 "%s, 0x%x=0x%x, 0x%x=0x%x, 0x%x=0x%x, 0x%x=0x%x 0x%x=0x%x\n",
		 __func__, tud->tune_param[0].reg, tud->tune_param[0].value,
		 tud->tune_param[1].reg, tud->tune_param[1].value,
		 tud->tune_param[2].reg, tud->tune_param[2].value,
		 tud->tune_param[3].reg, tud->tune_param[3].value,
		 tud->tune_param[4].reg, tud->tune_param[4].value);

	return 0;
err:
	dev_info(tud->dev, "%s: err = %d\n", __func__, ret);
	return ret;

}
EXPORT_SYMBOL_GPL(tusb2e11_power_on);

// ori: static int tusb2e11_probe(struct i2c_client *client,
// ori: 				const struct i2c_device_id *id)
static int tusb2e11_probe(struct i2c_client *client)
{
	struct device_node *of_node = client->dev.of_node;
	struct tusb2e11_data *tud;
	struct tusb2e11_plat_data *pdata = client->dev.platform_data;
	int ret = 0;

	tud = kzalloc(sizeof(*tud), GFP_KERNEL);
	if (tud == NULL) {
		ret = -ENOMEM;
		goto err_repeater_nomem;
	}
	tud->dev = &client->dev;

	if (of_node) {
		pdata = devm_kzalloc(&client->dev, sizeof(*pdata), GFP_KERNEL);
		if (!pdata) {
			ret = -ENOMEM;
			goto err_repeater_func;
		}

		ret = tusb2e11_parse_dt(&client->dev, tud, pdata);
		if (ret < 0)
			goto err_parse_dt;
	} else {
		pdata = client->dev.platform_data;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "%s: EIO err!\n", __func__);
		ret = -ENOMEM;
		goto err_parse_dt;
	}

	tud->client = client;
	tud->pdata = pdata;
	g_tud = tud;

	ret = gpio_request(tud->gpio_eusb_ctrl_sel, "eusb,gpio_eusb_ctrl");
	if (ret) {
		pr_err("failed to gpio_request eusb,gpio_eusb_ctrl\n");
		goto err_parse_dt;
	}

	i2c_set_clientdata(client, tud);
	mutex_init(&tud->i2c_mutex);

	/*
	 * eUSB repeater control should be done with ldo on.
	 * So control will be done according to utmi_init/exit.
	 * tusb2e11_power_on();
	 */

	return 0;

err_parse_dt:
	devm_kfree(&client->dev, pdata);
err_repeater_func:
	kfree(tud);
err_repeater_nomem:
	dev_info(&client->dev, "%s: err = %d\n", __func__, ret);

	return ret;
}

static const struct i2c_device_id tusb2e11_id[] = {
	{"tusb2e11-repeater", 0},
	{}
};

static void tusb2e11_shutdown(struct i2c_client *client)
{
	pr_info("%s\n", __func__);
}

// ori: static int tusb2e11_remove(struct i2c_client *client)
static void tusb2e11_remove(struct i2c_client *client)
{
	struct tusb2e11_data *tud = i2c_get_clientdata(client);

	mutex_destroy(&tud->i2c_mutex);
// ori:	return 0;
	return;
}


#if IS_ENABLED(CONFIG_PM)
static int tusb2e11_suspend(struct device *dev)
{
	return 0;
}

static int tusb2e11_resume(struct device *dev)
{
	return 0;
}
#else
#define tusb2e11_suspend NULL
#define tusb2e11_resume NULL
#endif

#ifdef CONFIG_PM
static const struct dev_pm_ops tusb2e11_pm_ops = {
	.suspend = tusb2e11_suspend,
	.resume = tusb2e11_resume,
};
#endif

static struct i2c_driver tusb2e11_driver = {
	.probe  = tusb2e11_probe,
	.remove = tusb2e11_remove,
	.shutdown   = tusb2e11_shutdown,
	.id_table   = tusb2e11_id,
	.driver = {
		.name = "tusb2e11-repeater",
		.owner = THIS_MODULE,
		.dev_groups = tusb2e11_repeater_groups,
		.pm = &tusb2e11_pm_ops,
		.of_match_table = tusb2e11_match_table,
	},
};

static int __init tusb2e11_init(void)
{
	return i2c_add_driver(&tusb2e11_driver);
}

static void __exit tusb2e11_exit(void)
{
	i2c_del_driver(&tusb2e11_driver);
}
module_init(tusb2e11_init);
module_exit(tusb2e11_exit);

MODULE_DESCRIPTION("Samsung eUSB Repeater Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");

