
// SPDX-License-Identifier: GPL-2.0
/**
 * exynos-dwc3.h - Header for inteface PMIC
 *
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2  of
 * the License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __LINUX_USB_DWC3_EXYNOS_H
#define __LINUX_USB_DWC3_EXYNOS_H
extern int exynos_usbdrd_ldo_manual_control(bool on);
extern int dwc3_exynos_id_event(struct device *dev, int state);
extern int dwc3_exynos_vbus_event(struct device *dev, bool vbus_active);
#endif /* __LINUX_USB_DWC3_EXYNOS_H */
