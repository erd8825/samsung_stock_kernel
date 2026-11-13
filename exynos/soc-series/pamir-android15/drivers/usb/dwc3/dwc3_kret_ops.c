// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012-2021, The Linux Foundation. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/sched.h>
#include "core.h"
#include "gadget.h"
#include "dwc3-exynos.h"
#include "exynos-otg.h"

#include <linux/platform_device.h>
#include "../host/xhci.h"
#include "../../../sound/usb/exynos_usb_audio.h"

#if IS_ENABLED(CONFIG_SND_EXYNOS_USB_AUDIO_MODULE)
extern struct hcd_hw_info *g_hwinfo;
#endif
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
extern struct wakeup_source *main_hcd_wakelock;
extern struct wakeup_source *shared_hcd_wakelock;
#endif
extern int otg_connection;
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
extern struct usb_hcd *exynos_main_hcd;
extern struct usb_hcd *exynos_shared_hcd;
#endif
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
extern int xhci_exynos_pm_state;
extern int exynos_usb_scenario_info(void);
#endif
extern struct dwc3_exynos *g_dwc3_exynos;

struct kprobe_data {
	void *x0;
	void *x1;
	void *x2;
};

static int entry___dwc3_set_mode(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	pr_info("%s\n", __func__);

	return 0;
}

static int entry_dwc3_core_soft_reset(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	pr_info("%s\n", __func__);

	return 0;
}

static int entry_dwc3_core_init(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct dwc3 *dwc = (struct dwc3 *)regs->regs[0];

	pr_info("%s+++\n", __func__);

	kret_data->x0 = dwc;

	return 0;
}

static int exit_dwc3_core_init(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct dwc3 *dwc = (struct dwc3 *)kret_data->x0;
	u32			reg;

	reg = dwc3_exynos_readl(dwc->regs, DWC3_GCTL);
	reg |= DWC3_GCTL_DSBLCLKGTNG;
	pr_info("%s : 0x%x\n", __func__, reg);
	dwc3_exynos_writel(dwc->regs, DWC3_GCTL, reg);

	pr_info("%s---\n", __func__);
	return 0;
}

static int entry_dwc3_gadget_init(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct dwc3 *dwc = (struct dwc3 *)regs->regs[0];

	pr_info("%s\n", __func__);

	kret_data->x0 = dwc;

	return 0;
}
static int exit_dwc3_gadget_init(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct dwc3 *dwc = (struct dwc3 *)kret_data->x0;

	pr_info("%s\n", __func__);

	dwc->gadget->sg_supported = false;
	pr_info("%s: clear sg_supported :%d\n", __func__, dwc->gadget->sg_supported);

	return 0;
}

#if IS_ENABLED(CONFIG_SOC_S5E8855)
static int entry_dwc3_send_gadget_ep_cmd(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct dwc3_ep *dep = (struct dwc3_ep *)regs->regs[0];

	kret_data->x0 = dep;

	return 0;
}

static int exit_dwc3_send_gadget_ep_cmd(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct dwc3_ep *dep = (struct dwc3_ep *)kret_data->x0;
	int ret = (int)regs->regs[0];
	u32			reg;
	u8			epnum;

	reg = dwc3_exynos_readl(dep->regs, DWC3_DEPCMD);
	epnum = dep->number;

	if ((reg & DWC3_DEPCMD_ENDTRANSFER) && ((epnum == 0) || (epnum == 1))) {
		if (ret == -ETIMEDOUT) {
			pr_info("%s : End Transfer Command timed out ret = %d\n", __func__, ret);
			regs->regs[0] = 0;
		}
	}

	return 0;
}
#endif

static int entry_dwc3_gadget_pullup(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct usb_gadget *g = (struct usb_gadget *)regs->regs[0];
	struct dwc3	*dwc = gadget_to_dwc(g);
	int is_on = (int)regs->regs[1];

	pr_info("%s on=%d, pullups_connected=%d\n", __func__, is_on, dwc->pullups_connected);

#if IS_ENABLED(CONFIG_SOC_S5E8855)
	/*
	 * Avoid unnecessary DWC3 core wake-ups If the Exynos configuration
	 * is not completed.
	 */
	if (is_on && g_dwc3_exynos && !g_dwc3_exynos->vbus_state) {
		pr_info("%s exynos setup is not done: vbus_state =%d!!\n",
				__func__, g_dwc3_exynos->vbus_state);

			regs->regs[1] = 0;
		}
#endif

	if (is_on == true)
		__irq_apply_affinity_hint(dwc->irq_gadget, cpumask_of(0x1), true);
	else
		__irq_apply_affinity_hint(dwc->irq_gadget, NULL, true);

	return 0;
}

static int exit_dwc3_gadget_pullup(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	int ret = (int)regs->regs[0];

	pr_info("%s--- ret = %d\n", __func__, ret);

	return 0;
}

static int entry_xhci_start(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct xhci_hcd *xhci = (struct xhci_hcd *)regs->regs[0];

	pr_info("%s\n", __func__);

	kret_data->x0 = xhci;

	return 0;
}
static int exit_xhci_start(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct xhci_hcd *xhci = (struct xhci_hcd *)kret_data->x0;

	pr_info("%s\n", __func__);

	pr_info("%s: xhci->run_graceperiod: %lu\n", __func__, xhci->run_graceperiod);
	if (xhci->run_graceperiod) {
		pr_info("%s: need to clear xhci->run_graceperiod\n", __func__);
		xhci->run_graceperiod = 0;
		pr_info("%s: xhci->run_graceperiod: %lu\n", __func__, xhci->run_graceperiod);
	}

	return 0;
}

static int entry_xhci_mem_init(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	pr_info("%s+++\n", __func__);

	return 0;
}

static int exit_xhci_mem_init(struct kretprobe_instance *ri,
				   struct pt_regs *regs)
{
	pr_info("%s---\n", __func__);

#if IS_ENABLED(CONFIG_SND_EXYNOS_USB_AUDIO_MODULE)
	g_hwinfo->need_first_probe = true;
#endif
	return 0;
}

static int entry_usb_add_hcd(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct usb_hcd *hcd = (struct usb_hcd *)regs->regs[0];

	pr_info("%s\n", __func__);

	hcd->skip_phy_initialization = 1;
	kret_data->x0 = hcd;

	return 0;
}

static int exit_usb_add_hcd(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct usb_hcd *hcd = (struct usb_hcd *)kret_data->x0;
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
#endif
	int ret = (int)regs->regs[0];

	pr_info("%s\n", __func__);

	if (ret < 0) {
		pr_info("%s is abnormally terminated!! Err %d\n", __func__, ret);
		return 0;
	}

#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
	if (xhci->main_hcd)
		exynos_main_hcd = xhci->main_hcd;
	if (xhci->shared_hcd)
		exynos_shared_hcd = xhci->shared_hcd;
#endif

	return 0;
}

static int entry_usb_remove_hcd(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	pr_info("%s\n", __func__);

#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
	exynos_main_hcd = NULL;
	exynos_shared_hcd = NULL;
#endif

	return 0;
}

static int entry_usb_gadget_connect_locked(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	g_dwc3_exynos->pullup_state = 1;
	pr_info("%s enter !!\n", __func__);

	return 0;
}

static int exit_usb_gadget_connect_locked(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	g_dwc3_exynos->pullup_state = 0;
	pr_info("%s exit !!\n", __func__);

	return 0;
}

static int entry_xhci_bus_suspend(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct usb_hcd *hcd = (struct usb_hcd *)regs->regs[0];

	pr_info("%s\n", __func__);

	kret_data->x0 = hcd;

	return 0;
}

static int exit_xhci_bus_suspend(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	struct kprobe_data *kret_data = (struct kprobe_data *)ri->data;
	struct usb_hcd *hcd = (struct usb_hcd *)kret_data->x0;
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	int main_hcd;

	if (!otg_connection) {
		pr_info("%s: otg_connection is 0. Skip unlock\n", __func__);
		return 0;
	}

	if (hcd == xhci->main_hcd) {
		main_hcd = 1;
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
		__pm_relax(main_hcd_wakelock);
		pr_info("%s, pm_state = %d\n", __func__, xhci_exynos_pm_state);
		xhci_exynos_pm_state = BUS_SUSPEND;
#endif

	} else {
		main_hcd = 0;
#ifdef CONFIG_EXYNOS_USBDRD_PHY30
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
		__pm_relax(shared_hcd_wakelock);
#endif
#endif
	}

	pr_info("%s: %s unlock\n", __func__, main_hcd ? "main_hcd" : "shared_hcd");

	return 0;
}

static int entry_xhci_bus_resume(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	struct usb_hcd *hcd = (struct usb_hcd *)regs->regs[0];
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	int main_hcd;

	if (!otg_connection) {
		pr_info("%s: otg_connection is 0. Skip lock\n", __func__);
		return 0;
	}

	if (hcd == xhci->main_hcd) {
		main_hcd = 1;
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
		__pm_stay_awake(main_hcd_wakelock);
		pr_info("%s, pm_state = %d\n", __func__, xhci_exynos_pm_state);
		xhci_exynos_pm_state = BUS_RESUME;
#endif
		/*
		 * Wakelock should be released again because call event is
		 * already done. Sometimes bus resume happens after call event.
		 */
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
		if (exynos_usb_scenario_info() == AUDIO_MODE_IN_CALL) {
			pr_info("%s, release wakelock in call state\n", __func__);
			__pm_relax(main_hcd_wakelock);
		}
#endif
	} else {
		main_hcd = 0;
#ifdef CONFIG_EXYNOS_USBDRD_PHY30
#ifdef CONFIG_SND_EXYNOS_USB_AUDIO_GIC
		__pm_stay_awake(shared_hcd_wakelock);
#endif
#endif
	}

	pr_info("%s: %s lock\n", __func__, main_hcd ? "main_hcd" : "shared_hcd");

	return 0;
}

static int entry_dwc3_event_buffers_setup(struct kretprobe_instance *ri,
		struct pt_regs *regs)
{
	struct dwc3 *dwc = (struct dwc3 *)regs->regs[0];
	struct dwc3_event_buffer *evt = dwc->ev_buf;

	pr_info("%s evt->flags %x\n", __func__, evt->flags);

	/*Clearing of DWC3_EVENT_PENDING if already set to avoid IRQ storm*/
	if (evt->flags & DWC3_EVENT_PENDING)
		evt->flags &= ~DWC3_EVENT_PENDING;

	return 0;
}

static int entry_storage_probe(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	pr_info("%s +++\n", __func__);
	g_dwc3_exynos->is_perf = 1;
	queue_work(g_dwc3_exynos->cpufreq_wq, &g_dwc3_exynos->cpufreq_work);

	return 0;
}

static int entry_usb_stor_disconnect(struct kretprobe_instance *ri,
					struct pt_regs *regs)
{
	pr_info("%s +++\n", __func__);
	g_dwc3_exynos->is_perf = 0;
	queue_work(g_dwc3_exynos->cpufreq_wq, &g_dwc3_exynos->cpufreq_work);

	return 0;
}

#define ENTRY_EXIT(name) {\
	.handler = exit_##name,\
	.entry_handler = entry_##name,\
	.data_size = sizeof(struct kprobe_data),\
	.maxactive = 8,\
	.kp.symbol_name = #name,\
}

#define ENTRY(name) {\
	.entry_handler = entry_##name,\
	.data_size = sizeof(struct kprobe_data),\
	.maxactive = 8,\
	.kp.symbol_name = #name,\
}

static struct kretprobe dwc3_kret_probes[] = {
	ENTRY(__dwc3_set_mode),
	ENTRY(dwc3_core_soft_reset),
	ENTRY_EXIT(dwc3_core_init),
	ENTRY_EXIT(dwc3_gadget_init),
	ENTRY_EXIT(dwc3_gadget_pullup),
	ENTRY_EXIT(xhci_start),
	ENTRY_EXIT(xhci_mem_init),
	ENTRY_EXIT(xhci_bus_suspend),
	ENTRY(xhci_bus_resume),
	ENTRY_EXIT(usb_add_hcd),
	ENTRY(dwc3_event_buffers_setup),
	ENTRY(usb_remove_hcd),
	ENTRY_EXIT(usb_gadget_connect_locked),
#if IS_ENABLED(CONFIG_SOC_S5E8855)
	ENTRY_EXIT(dwc3_send_gadget_ep_cmd),
#endif
	ENTRY(storage_probe),
	ENTRY(usb_stor_disconnect),
};

int dwc3_kretprobe_init(void)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(dwc3_kret_probes); i++) {
		ret = register_kretprobe(&dwc3_kret_probes[i]);
		if (ret < 0) {
			pr_err("register_kretprobe failed, returned %d\n", ret);
			return ret;
		}
	}

	return 0;
}

void dwc3_kretprobe_exit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dwc3_kret_probes); i++)
		unregister_kretprobe(&dwc3_kret_probes[i]);
}

MODULE_SOFTDEP("pre:dwc3-exynos-usb");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("DesignWare USB3 EXYNOS Glue Layer function handler");
