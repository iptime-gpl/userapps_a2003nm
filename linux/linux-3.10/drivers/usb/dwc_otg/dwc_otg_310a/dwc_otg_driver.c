/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_driver.c $
 * $Revision: 1.1.1.1 $
 * $Date: 2016/11/01 07:34:23 $
 * $Change: 2234037 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/** @file
 * The dwc_otg_driver module provides the initialization and cleanup entry
 * points for the DWC_otg driver. This module will be dynamically installed
 * after Linux is booted using the insmod command. When the module is
 * installed, the dwc_otg_driver_init function is called. When the module is
 * removed (using rmmod), the dwc_otg_driver_cleanup function is called.
 *
 * This module also defines a data structure for the dwc_otg_driver, which is
 * used in conjunction with the standard ARM lm_device structure. These
 * structures allow the OTG driver to comply with the standard Linux driver
 * model in which devices and drivers are registered with a bus driver. This
 * has the benefit that Linux can expose attributes of the driver and device
 * in its special sysfs file system. Users can then read or write files in
 * this file system to perform diagnostics on the driver components or the
 * device.
 */

#include "dwc_otg_os_dep.h"
#include "dwc_os.h"
#include "dwc_otg_dbg.h"
#include "dwc_otg_driver.h"
#include "dwc_otg_attr.h"
#include "dwc_otg_core_if.h"
#include "dwc_otg_pcd_if.h"
#include "dwc_otg_hcd_if.h"

#include "bspchip.h"  //wei add
#include "lm.h"  //wei add
#include <linux/sizes.h>  //wei add
//#define _init //wei add

#define DWC_DRIVER_VERSION	"3.10b 20-MAY-2013"
#define DWC_DRIVER_DESC		"HS OTG USB Controller driver"


static const char dwc_driver_name[] = "dwc_otg";

extern int pcd_init(   struct lm_device *_dev   );
extern int hcd_init(   struct lm_device *_dev   );
extern int pcd_remove(     struct lm_device *_dev   );
extern void hcd_remove(    struct lm_device *_dev   );

extern void dwc_otg_adp_start(dwc_otg_core_if_t * core_if, uint8_t is_host);

/*-------------------------------------------------------------------------*/
/* Encapsulate the module parameter settings */

struct dwc_otg_driver_module_params {
	int32_t opt;
	int32_t otg_cap;
	int32_t dma_enable;
	int32_t dma_desc_enable;
	int32_t dma_burst_size;
	int32_t speed;
	int32_t host_support_fs_ls_low_power;
	int32_t host_ls_low_power_phy_clk;
	int32_t enable_dynamic_fifo;
	int32_t data_fifo_size;
	int32_t dev_rx_fifo_size;
	int32_t dev_nperio_tx_fifo_size;
	uint32_t dev_perio_tx_fifo_size[MAX_PERIO_FIFOS];
	int32_t host_rx_fifo_size;
	int32_t host_nperio_tx_fifo_size;
	int32_t host_perio_tx_fifo_size;
	int32_t max_transfer_size;
	int32_t max_packet_count;
	int32_t host_channels;
	int32_t dev_endpoints;
	int32_t phy_type;
	int32_t phy_utmi_width;
	int32_t phy_ulpi_ddr;
	int32_t phy_ulpi_ext_vbus;
	int32_t i2c_enable;
	int32_t ulpi_fs_ls;
	int32_t ts_dline;
	int32_t en_multiple_tx_fifo;
	uint32_t dev_tx_fifo_size[MAX_TX_FIFOS];
	uint32_t thr_ctl;
	uint32_t tx_thr_length;
	uint32_t rx_thr_length;
	int32_t pti_enable;
	int32_t mpi_enable;
	int32_t lpm_enable;
	int32_t besl_enable;
	int32_t baseline_besl;
	int32_t deep_besl;
	int32_t ic_usb_cap;
	int32_t ahb_thr_ratio;
	int32_t power_down;
	int32_t reload_ctl;
	int32_t dev_out_nak;
	int32_t cont_on_bna;
	int32_t ahb_single;
	int32_t otg_ver;
	int32_t adp_enable;
};

static struct dwc_otg_driver_module_params dwc_otg_module_params = {
	.opt = -1,
	.otg_cap = DWC_OTG_CAP_PARAM_NO_HNP_SRP_CAPABLE,   //wei add
	.dma_enable = -1,
	.dma_desc_enable = -1,
	.dma_burst_size = -1,
	.speed = -1,
	.host_support_fs_ls_low_power = -1,  
	.host_ls_low_power_phy_clk = -1,  
	.enable_dynamic_fifo = -1,
	.data_fifo_size = -1,
	.dev_rx_fifo_size = -1,
	.dev_nperio_tx_fifo_size = -1,
	.dev_perio_tx_fifo_size = {
				   /* dev_perio_tx_fifo_size_1 */
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1
				   /* 15 */
				   },
	.host_rx_fifo_size = -1,
	.host_nperio_tx_fifo_size = -1,
	.host_perio_tx_fifo_size = -1,
	.max_transfer_size = -1,
	.max_packet_count = -1,
	.host_channels = -1,
	.dev_endpoints = -1,
	.phy_type = -1,
	.phy_utmi_width = -1,
	.phy_ulpi_ddr = -1,
	.phy_ulpi_ext_vbus = -1,
	.i2c_enable = -1,
	.ulpi_fs_ls = -1,
	.ts_dline = -1,
	.en_multiple_tx_fifo = -1,
	.dev_tx_fifo_size = {
			     /* dev_tx_fifo_size */
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1
			     /* 15 */
			     },
	.thr_ctl = -1,
	.tx_thr_length = -1,
	.rx_thr_length = -1,
	.pti_enable = -1,
	.mpi_enable = -1,
	.lpm_enable = -1,
	.besl_enable = -1,
	.baseline_besl = -1,
	.deep_besl = -1,
	.ic_usb_cap = -1,
	.ahb_thr_ratio = -1,
	.power_down = -1,
	.reload_ctl = -1,
	.dev_out_nak = -1,
	.cont_on_bna = -1,
	.ahb_single = -1,
	.otg_ver = -1,
	.adp_enable = -1,
};

/**
 * This function shows the Driver Version.
 */
static ssize_t version_show(struct device_driver *dev, char *buf)
{
	return snprintf(buf, sizeof(DWC_DRIVER_VERSION) + 2, "%s\n",
			DWC_DRIVER_VERSION);
}

static DRIVER_ATTR(version, S_IRUGO, version_show, NULL);

/**
 * Global Debug Level Mask.
 */
uint32_t g_dbg_lvl = 0;		/* OFF */

/**
 * This function shows the driver Debug Level.
 */
static ssize_t dbg_level_show(struct device_driver *drv, char *buf)
{
	return sprintf(buf, "0x%0x\n", g_dbg_lvl);
}

/**
 * This function stores the driver Debug Level.
 */
static ssize_t dbg_level_store(struct device_driver *drv, const char *buf,
			       size_t count)
{
	g_dbg_lvl = simple_strtoul(buf, NULL, 16);
	return count;
}

static DRIVER_ATTR(debuglevel, S_IRUGO | S_IWUSR, dbg_level_show,
		   dbg_level_store);

/**
 * This function is called during module intialization
 * to pass module parameters to the DWC_OTG CORE.
 */
static int set_parameters(dwc_otg_core_if_t * core_if)
{
	int retval = 0;
	int i;

	if (dwc_otg_module_params.otg_cap != -1) {
		retval +=
		    dwc_otg_set_param_otg_cap(core_if,
					      dwc_otg_module_params.otg_cap);
	}
	if (dwc_otg_module_params.dma_enable != -1) {
		retval +=
		    dwc_otg_set_param_dma_enable(core_if,
						 dwc_otg_module_params.
						 dma_enable);
	}
	if (dwc_otg_module_params.dma_desc_enable != -1) {
		retval +=
		    dwc_otg_set_param_dma_desc_enable(core_if,
						      dwc_otg_module_params.
						      dma_desc_enable);
	}
	if (dwc_otg_module_params.opt != -1) {
		retval +=
		    dwc_otg_set_param_opt(core_if, dwc_otg_module_params.opt);
	}
	if (dwc_otg_module_params.dma_burst_size != -1) {
		retval +=
		    dwc_otg_set_param_dma_burst_size(core_if,
						     dwc_otg_module_params.
						     dma_burst_size);
	}
	if (dwc_otg_module_params.host_support_fs_ls_low_power != -1) {
		retval +=
		    dwc_otg_set_param_host_support_fs_ls_low_power(core_if,
								   dwc_otg_module_params.
								   host_support_fs_ls_low_power);
	}
	if (dwc_otg_module_params.enable_dynamic_fifo != -1) {
		retval +=
		    dwc_otg_set_param_enable_dynamic_fifo(core_if,
							  dwc_otg_module_params.
							  enable_dynamic_fifo);
	}
	if (dwc_otg_module_params.data_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_data_fifo_size(core_if,
						     dwc_otg_module_params.
						     data_fifo_size);
	}
	if (dwc_otg_module_params.dev_rx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_dev_rx_fifo_size(core_if,
						       dwc_otg_module_params.
						       dev_rx_fifo_size);
	}
	if (dwc_otg_module_params.dev_nperio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_dev_nperio_tx_fifo_size(core_if,
							      dwc_otg_module_params.
							      dev_nperio_tx_fifo_size);
	}
	if (dwc_otg_module_params.host_rx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_rx_fifo_size(core_if,
							dwc_otg_module_params.host_rx_fifo_size);
	}
	if (dwc_otg_module_params.host_nperio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_nperio_tx_fifo_size(core_if,
							       dwc_otg_module_params.
							       host_nperio_tx_fifo_size);
	}
	if (dwc_otg_module_params.host_perio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_perio_tx_fifo_size(core_if,
							      dwc_otg_module_params.
							      host_perio_tx_fifo_size);
	}
	if (dwc_otg_module_params.max_transfer_size != -1) {
		retval +=
		    dwc_otg_set_param_max_transfer_size(core_if,
							dwc_otg_module_params.
							max_transfer_size);
	}
	if (dwc_otg_module_params.max_packet_count != -1) {
		retval +=
		    dwc_otg_set_param_max_packet_count(core_if,
						       dwc_otg_module_params.
						       max_packet_count);
	}
	if (dwc_otg_module_params.host_channels != -1) {
		retval +=
		    dwc_otg_set_param_host_channels(core_if,
						    dwc_otg_module_params.
						    host_channels);
	}
	if (dwc_otg_module_params.dev_endpoints != -1) {
		retval +=
		    dwc_otg_set_param_dev_endpoints(core_if,
						    dwc_otg_module_params.
						    dev_endpoints);
	}
	if (dwc_otg_module_params.phy_type != -1) {
		retval +=
		    dwc_otg_set_param_phy_type(core_if,
					       dwc_otg_module_params.phy_type);
	}
	if (dwc_otg_module_params.speed != -1) {
		retval +=
		    dwc_otg_set_param_speed(core_if,
					    dwc_otg_module_params.speed);
	}
	if (dwc_otg_module_params.host_ls_low_power_phy_clk != -1) {
		retval +=
		    dwc_otg_set_param_host_ls_low_power_phy_clk(core_if,
								dwc_otg_module_params.
								host_ls_low_power_phy_clk);
	}
	if (dwc_otg_module_params.phy_ulpi_ddr != -1) {
		retval +=
		    dwc_otg_set_param_phy_ulpi_ddr(core_if,
						   dwc_otg_module_params.
						   phy_ulpi_ddr);
	}
	if (dwc_otg_module_params.phy_ulpi_ext_vbus != -1) {
		retval +=
		    dwc_otg_set_param_phy_ulpi_ext_vbus(core_if,
							dwc_otg_module_params.
							phy_ulpi_ext_vbus);
	}
	if (dwc_otg_module_params.phy_utmi_width != -1) {
		retval +=
		    dwc_otg_set_param_phy_utmi_width(core_if,
						     dwc_otg_module_params.
						     phy_utmi_width);
	}
	if (dwc_otg_module_params.ulpi_fs_ls != -1) {
		retval +=
		    dwc_otg_set_param_ulpi_fs_ls(core_if,
						 dwc_otg_module_params.ulpi_fs_ls);
	}
	if (dwc_otg_module_params.ts_dline != -1) {
		retval +=
		    dwc_otg_set_param_ts_dline(core_if,
					       dwc_otg_module_params.ts_dline);
	}
	if (dwc_otg_module_params.i2c_enable != -1) {
		retval +=
		    dwc_otg_set_param_i2c_enable(core_if,
						 dwc_otg_module_params.
						 i2c_enable);
	}
	if (dwc_otg_module_params.en_multiple_tx_fifo != -1) {
		retval +=
		    dwc_otg_set_param_en_multiple_tx_fifo(core_if,
							  dwc_otg_module_params.
							  en_multiple_tx_fifo);
	}
	for (i = 0; i < 15; i++) {
		if (dwc_otg_module_params.dev_perio_tx_fifo_size[i] != -1) {
			retval +=
			    dwc_otg_set_param_dev_perio_tx_fifo_size(core_if,
								     dwc_otg_module_params.
								     dev_perio_tx_fifo_size
								     [i], i);
		}
	}

	for (i = 0; i < 15; i++) {
		if (dwc_otg_module_params.dev_tx_fifo_size[i] != -1) {
			retval += dwc_otg_set_param_dev_tx_fifo_size(core_if,
								     dwc_otg_module_params.
								     dev_tx_fifo_size
								     [i], i);
		}
	}
	if (dwc_otg_module_params.thr_ctl != -1) {
		retval +=
		    dwc_otg_set_param_thr_ctl(core_if,
					      dwc_otg_module_params.thr_ctl);
	}
	if (dwc_otg_module_params.mpi_enable != -1) {
		retval +=
		    dwc_otg_set_param_mpi_enable(core_if,
						 dwc_otg_module_params.
						 mpi_enable);
	}
	if (dwc_otg_module_params.pti_enable != -1) {
		retval +=
		    dwc_otg_set_param_pti_enable(core_if,
						 dwc_otg_module_params.
						 pti_enable);
	}
	if (dwc_otg_module_params.lpm_enable != -1) {
		retval +=
		    dwc_otg_set_param_lpm_enable(core_if,
						 dwc_otg_module_params.
						 lpm_enable);
	}	
	if (dwc_otg_module_params.besl_enable != -1) {
		retval +=
		    dwc_otg_set_param_besl_enable(core_if,
						 dwc_otg_module_params.
						 besl_enable);
	}
	if (dwc_otg_module_params.baseline_besl != -1) {
		retval +=
		    dwc_otg_set_param_baseline_besl(core_if,
						 dwc_otg_module_params.
						 baseline_besl);
	}
	if (dwc_otg_module_params.deep_besl != -1) {
		retval +=
		    dwc_otg_set_param_deep_besl(core_if,
						 dwc_otg_module_params.
						 deep_besl);
	}		
	if (dwc_otg_module_params.ic_usb_cap != -1) {
		retval +=
		    dwc_otg_set_param_ic_usb_cap(core_if,
						 dwc_otg_module_params.
						 ic_usb_cap);
	}
	if (dwc_otg_module_params.tx_thr_length != -1) {
		retval +=
		    dwc_otg_set_param_tx_thr_length(core_if,
						    dwc_otg_module_params.tx_thr_length);
	}
	if (dwc_otg_module_params.rx_thr_length != -1) {
		retval +=
		    dwc_otg_set_param_rx_thr_length(core_if,
						    dwc_otg_module_params.
						    rx_thr_length);
	}
	if (dwc_otg_module_params.ahb_thr_ratio != -1) {
		retval +=
		    dwc_otg_set_param_ahb_thr_ratio(core_if,
						    dwc_otg_module_params.ahb_thr_ratio);
	}
	if (dwc_otg_module_params.power_down != -1) {
		retval +=
		    dwc_otg_set_param_power_down(core_if,
						 dwc_otg_module_params.power_down);
	}
	if (dwc_otg_module_params.reload_ctl != -1) {
		retval +=
		    dwc_otg_set_param_reload_ctl(core_if,
						 dwc_otg_module_params.reload_ctl);
	}

	if (dwc_otg_module_params.dev_out_nak != -1) {
		retval +=
			dwc_otg_set_param_dev_out_nak(core_if,
			dwc_otg_module_params.dev_out_nak);
	}

	if (dwc_otg_module_params.cont_on_bna != -1) {
		retval +=
			dwc_otg_set_param_cont_on_bna(core_if,
			dwc_otg_module_params.cont_on_bna);
	}

	if (dwc_otg_module_params.ahb_single != -1) {
		retval +=
			dwc_otg_set_param_ahb_single(core_if,
			dwc_otg_module_params.ahb_single);
	}

	if (dwc_otg_module_params.otg_ver != -1) {
		retval +=
		    dwc_otg_set_param_otg_ver(core_if,
					      dwc_otg_module_params.otg_ver);
	}
	if (dwc_otg_module_params.adp_enable != -1) {
		retval +=
		    dwc_otg_set_param_adp_enable(core_if,
						 dwc_otg_module_params.
						 adp_enable);
	}
	return retval;
}

/**
 * This function is the top level interrupt handler for the Common
 * (Device and host modes) interrupts.
 */
static irqreturn_t dwc_otg_common_irq(int irq, void *dev)
{
	int32_t retval = IRQ_NONE;

	retval = dwc_otg_handle_common_intr(dev);
	if (retval != 0) {
		S3C2410X_CLEAR_EINTPEND();
	}
	return IRQ_RETVAL(retval);
}

/**
 * This function is called when a lm_device is unregistered with the
 * dwc_otg_driver. This happens, for example, when the rmmod command is
 * executed. The device may or may not be electrically present. If it is
 * present, the driver stops device processing. Any resources used on behalf
 * of this device are freed.
 *
 * @param _dev
 */
static void dwc_otg_driver_remove(
#ifdef LM_INTERFACE
					 struct lm_device *_dev
#elif defined(PCI_INTERFACE)
					 struct pci_dev *_dev
#endif
    )
{
#ifdef LM_INTERFACE
	dwc_otg_device_t *otg_dev = lm_get_drvdata(_dev);
#elif defined(PCI_INTERFACE)
	dwc_otg_device_t *otg_dev = pci_get_drvdata(_dev);
#endif

	DWC_DEBUGPL(DBG_ANY, "%s(%p)\n", __func__, _dev);

	if (!otg_dev) {
		/* Memory allocation for the dwc_otg_device failed. */
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev NULL!\n", __func__);
		return;
	}
#ifndef DWC_DEVICE_ONLY
	if (otg_dev->hcd) {
		hcd_remove(_dev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->hcd NULL!\n", __func__);
		return;
	}
#endif

#ifndef DWC_HOST_ONLY
	if (otg_dev->pcd) {
		pcd_remove(_dev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->pcd NULL!\n", __func__);
		return;
	}
#endif
	/*
	 * Free the IRQ
	 */
	if (otg_dev->common_irq_installed) {
		free_irq(_dev->irq, otg_dev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: There is no installed irq!\n", __func__);
		return;
	}

	if (otg_dev->core_if) {
		dwc_otg_cil_remove(otg_dev->core_if);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->core_if NULL!\n", __func__);
		return;
	}

	/*
	 * Remove the device attributes
	 */
	dwc_otg_attr_remove(_dev);

	/*
	 * Return the memory.
	 */
	if (otg_dev->os_dep.base) {
		iounmap(otg_dev->os_dep.base);
	}
	DWC_FREE(otg_dev);

	/*
	 * Clear the drvdata pointer.
	 */
#ifdef LM_INTERFACE
	lm_set_drvdata(_dev, 0);
#elif defined(PCI_INTERFACE)
    release_mem_region(otg_dev->os_dep.rsrc_start, otg_dev->os_dep.rsrc_len);
	pci_set_drvdata(_dev, 0);
#endif
}

/**
 * This function is called when an lm_device is bound to a
 * dwc_otg_driver. It creates the driver components required to
 * control the device (CIL, HCD, and PCD) and it initializes the
 * device. The driver components are stored in a dwc_otg_device
 * structure. A reference to the dwc_otg_device is saved in the
 * lm_device. This allows the driver to access the dwc_otg_device
 * structure on subsequent calls to driver methods for this device.
 *
 * @param _dev Bus device
 */
static int dwc_otg_driver_probe( struct lm_device *_dev )
{
#define dev_dbg(x,y...) printk(y) //wei add

	int retval = 0;
	dwc_otg_device_t *dwc_otg_device;

	dev_dbg(&_dev->dev, "dwc_otg_driver_probe(%p)\n", _dev);
#ifdef LM_INTERFACE

	dev_dbg(&_dev->dev, "start=0x%08x\n", (unsigned)_dev->resource.start);

#endif

	dwc_otg_device = DWC_ALLOC(sizeof(dwc_otg_device_t));

	if (!dwc_otg_device) {
		dev_err(&_dev->dev, "kmalloc of dwc_otg_device failed\n");
		return -ENOMEM;
	}

	memset(dwc_otg_device, 0, sizeof(*dwc_otg_device));
	dwc_otg_device->os_dep.reg_offset = 0xFFFFFFFF;

	/*
	 * Map the DWC_otg Core memory into virtual address space.
	 */
#ifdef LM_INTERFACE
//	dwc_otg_device->os_dep.base = ioremap(_dev->resource.start, SZ_256K);
	dwc_otg_device->os_dep.base = _dev->resource.start;


	if (!dwc_otg_device->os_dep.base) {
		dev_err(&_dev->dev, "ioremap() failed\n");
		DWC_FREE(dwc_otg_device);
		return -ENOMEM;
	}
	dev_dbg(&_dev->dev, "base=0x%08x\n",
		(unsigned)dwc_otg_device->os_dep.base);

#endif

	/*
	 * Initialize driver data to point to the global DWC_otg
	 * Device structure.
	 */
#ifdef LM_INTERFACE
	lm_set_drvdata(_dev, dwc_otg_device);
#endif
	dev_dbg(&_dev->dev, "dwc_otg_device=0x%p\n", dwc_otg_device);

	dwc_otg_device->core_if = dwc_otg_cil_init(dwc_otg_device->os_dep.base);
	if (!dwc_otg_device->core_if) {
		dev_err(&_dev->dev, "CIL initialization failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	/*
	 * Attempt to ensure this device is really a DWC_otg Controller.
	 * Read and verify the SNPSID register contents. The value should be
	 * 0x45F42XXX or 0x45F42XXX, which corresponds to either "OT2" or "OTG3",
	 * as in "OTG version 2.XX" or "OTG version 3.XX".
	 */
printk("=> SNPSID=%x\n", dwc_otg_get_gsnpsid(dwc_otg_device->core_if) );  //wei add
	if (((dwc_otg_get_gsnpsid(dwc_otg_device->core_if) & 0xFFFFF000) !=	0x4F542000) &&
		((dwc_otg_get_gsnpsid(dwc_otg_device->core_if) & 0xFFFFF000) != 0x4F543000)) {
		dev_err(&_dev->dev, "Bad value for SNPSID: 0x%08x\n",
			dwc_otg_get_gsnpsid(dwc_otg_device->core_if));
		retval = -EINVAL;
		goto fail;
	}

	/*
	 * Validate parameter values.
	 */
	if (set_parameters(dwc_otg_device->core_if)) {
		retval = -EINVAL;
		goto fail;
	}
#if 0 //wei add	
	/*
	 * Create Device Attributes in sysfs
	 */
	dwc_otg_attr_create(_dev);
#endif
	/*
	 * Disable the global interrupt until all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_disable_global_interrupts(dwc_otg_device->core_if);

	/*
	 * Install the interrupt handler for the common interrupts before
	 * enabling common interrupts in core_init below.
	 */
	DWC_DEBUGPL(DBG_CIL, "registering (common) handler for irq%d\n",
		    _dev->irq);
	retval = request_irq(_dev->irq, dwc_otg_common_irq,
			     IRQF_SHARED | IRQF_DISABLED | IRQ_LEVEL, "dwc_otg",
			     dwc_otg_device);
	if (retval) {
		DWC_ERROR("request of irq%d failed\n", _dev->irq);
		retval = -EBUSY;
		goto fail;
	} else {
		dwc_otg_device->common_irq_installed = 1;
	}

#ifdef LM_INTERFACE
//	set_irq_type(_dev->irq, IRQT_LOW);  //wei del
#endif
	/*
	 * Initialize the DWC_otg core.
	 */
	dwc_otg_core_init(dwc_otg_device->core_if);
		
#ifndef DWC_HOST_ONLY
//(gHostMode==0)
{
	/*
	 * Initialize the PCD
	 */
	retval = pcd_init(_dev);
	if (retval != 0) {
		DWC_ERROR("pcd_init failed\n");
		dwc_otg_device->pcd = NULL;
		goto fail;
	}	
}
#endif
#ifndef DWC_DEVICE_ONLY
//(gHostMode==1)
{

	/*
	 * Initialize the HCD
	 */
	retval = hcd_init(_dev);
	if (retval != 0) {
		DWC_ERROR("hcd_init failed\n");
		dwc_otg_device->hcd = NULL;
		goto fail;
	}
}	
#endif

	/*
	 * Enable the global interrupt after all the interrupt
	 * handlers are installed if there is no ADP support else 
	 * perform initial actions required for Internal ADP logic.
	 */
	if (!dwc_otg_get_param_adp_enable(dwc_otg_device->core_if))	
		dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);
	else
		dwc_otg_adp_start(dwc_otg_device->core_if, 
							dwc_otg_is_host_mode(dwc_otg_device->core_if));

	return 0;

fail:
	dwc_otg_driver_remove(_dev);
	return retval;
}

/**
 * This structure defines the methods to be called by a bus driver
 * during the lifecycle of a device on that bus. Both drivers and
 * devices are registered with a bus driver. The bus driver matches
 * devices to drivers based on information in the device and driver
 * structures.
 *
 * The probe function is called when the bus driver matches a device
 * to this driver. The remove function is called when a device is
 * unregistered with the bus driver.
 */
#ifdef LM_INTERFACE
static struct lm_driver dwc_otg_driver = {
	.driver = {.name = (char *)dwc_driver_name,},
	.probe = dwc_otg_driver_probe,
	.remove = dwc_otg_driver_remove,
};
#endif
//======================================================================
void dwc_otg_phy_write(unsigned char reg, unsigned char val)
{
	#define USB2_PHY_DELAY mdelay(1) 

	if((reg < 0xE0) || (reg > 0xF6) || ((reg>0xE7)&&(reg<0xF0))) {
		printk("DWC_OTG: Wrong register address: 0x%02x\n", reg);
		return;
	}
	

	REG32(0xb814030c) = (val << 0) ; USB2_PHY_DELAY;

	REG32(0xb8030034) = ((reg & 0x0F) << 8) | 0x02010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0x0F) << 8) | 0x00010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0x0F) << 8) | 0x02010000; USB2_PHY_DELAY;
	
	REG32(0xb8030034) = ((reg & 0xF0) << 4) | 0x02010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0xF0) << 4) | 0x00010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0xF0) << 4) | 0x02010000; USB2_PHY_DELAY;

	return;
}
//------------------------------------------------------------------------
unsigned char dwc_otg_phy_read(unsigned char reg)
{
	#define USB2_PHY_DELAY mdelay(1)   //__delay(50000) 
	unsigned char val = 0;

	if((reg < 0xE0) || (reg > 0xF6) || ((reg>0xE7)&&(reg<0xF0))) {
		printk("DWC_OTG: Wrong register address: 0x%02x\n", reg);
		return 0;
	}
	
	reg = reg - 0x20;
	REG32(0xb8030034) = ((reg & 0x0F) << 8) | 0x02010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0x0F) << 8) | 0x00010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0x0F) << 8) | 0x02010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0xF0) << 4) | 0x02010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0xF0) << 4) | 0x00010000; USB2_PHY_DELAY;
	REG32(0xb8030034) = ((reg & 0xF0) << 4) | 0x02010000; USB2_PHY_DELAY;

//	val = (REG32(0xb8030034) & 0xFF000000) >> 24;
	val = REG32(0xb8030034) & 0xff;
	//printk("DWC_OTG: phy(0x%02x) = 0x%02x\n", reg + 0x20, val);
	
	return val;
}
//------------------------------------------------------------------------
void dump_otg_phy()
{
	int i;
	dwc_otg_phy_write( 0xf4, 0x9b );
	
	for(i=0xe0;i<=0xe7; i++)
		printk("reg %x=%x\n", i,dwc_otg_phy_read(i) );	
	for(i=0xf0;i<=0xf4; i++)
		printk("reg %x=%x\n", i,dwc_otg_phy_read(i) );	

	dwc_otg_phy_write( 0xf4, 0xbb);
	for(i=0xe0;i<=0xe7; i++)
		printk("reg %x=%x\n", i,dwc_otg_phy_read(i) );	
	for(i=0xf0;i<=0xf4; i++)
		printk("reg %x=%x\n", i,dwc_otg_phy_read(i) );	

	dwc_otg_phy_write( 0xf4, 0x9b );

}

void Set_otg_phy_Patch()
{

#if 0	//cathy, test_J, test_K
//write phy register
	#define USB2_PHY_DELAY {int i=100; while(i>0) {i--;}}
	int tmp = REG32(0xb8003314);
	REG32(0xb8003314) = tmp|0x008E0000; USB2_PHY_DELAY;	
	REG32(0xb8030034) = 0x8E014002; USB2_PHY_DELAY;	
	REG32(0xb8030034) = 0x8E014000; USB2_PHY_DELAY;	
	REG32(0xb8030034) = 0x8E014002; USB2_PHY_DELAY;	
	REG32(0xb8030034) = 0x8E0F4002; USB2_PHY_DELAY;	
	REG32(0xb8030034) = 0x8E0F4000; USB2_PHY_DELAY;	
	REG32(0xb8030034) = 0x8E0F4002; USB2_PHY_DELAY;	
#endif


#if 0//CONFIG_RTL_8198C
	
//	REG32(0xb8000010) = REG32(0xb8000010) | (0x1<<29);
	//USBPhyReset(0);
	 dwc_otg_phy_write(0xe0,0x44);  //disconnect, work
        dwc_otg_phy_write(0xe1,0xe9);  //disconnect, work
        dwc_otg_phy_write(0xe2,0x9a);  //disconnect, work
        dwc_otg_phy_write(0xe3,0xa1);  //disconnect, work
        dwc_otg_phy_write(0xe5,0x95);  //disconnect, work
        dwc_otg_phy_write(0xe6,0x98);  //disco

        dwc_otg_phy_write(0xf5,0x49);  //disco
	printk("b8000010=%x\r\n",REG32(0xb8000010));
	
	printk("b8000090=%x\r\n",REG32(0xb8000090));

#endif
#if 0
	//USBPhyReset(0);  //1: in reset, 0: working	
	//PHYPatch();  //wei add	
	dwc_otg_phy_write(0xe0,0x99);  //disconnect, work
	dwc_otg_phy_write(0xe1,0xac);  //disconnect, work
	dwc_otg_phy_write(0xe2,0x98);  //disconnect, work
	dwc_otg_phy_write(0xe3,0xc1);  //disconnect, work
	dwc_otg_phy_write(0xe4,0x1);  //disconnect, work
	dwc_otg_phy_write(0xe5,0x89);  //disconnect, work
	dwc_otg_phy_write(0xe6,0x98);  //disconnect, worki
	dwc_otg_phy_write(0xe7,0x1d);  //disconnect, work
	dwc_otg_phy_write(0xf0,0xfc);  //disconnect, work
	dwc_otg_phy_write(0xf1,0x8c);  //disconnect, work
	dwc_otg_phy_write(0xf2,0x0);  //disconnect, work
	dwc_otg_phy_write(0xf3,0x11);  //disconnect, work
	dwc_otg_phy_write(0xf4,0xfb);  //disconnect, work
	dwc_otg_phy_write(0xf5,0xd2);  //disconnect, 
	dwc_otg_phy_write(0xf6,0x0);  //disconnect, work
	
	dwc_otg_phy_write(0xe6,0xb8);  //disconnect, work
#endif

#if 0
	printk("RLE0599 u2 phy patch\n");   //FPGA sub-board.
	//page 0
	dwc_otg_phy_write( 0xf4, 0x9b );

	dwc_otg_phy_write( 0xe0, 0xe1);
	dwc_otg_phy_write( 0xe1, 0x2e);
	dwc_otg_phy_write( 0xe2, 0xb5);
	//dwc_otg_phy_write( 0xe3, 0x8b);
	dwc_otg_phy_write( 0xe4, 0x9b);
	//dwc_otg_phy_write( 0xe5, 0x5d);
	dwc_otg_phy_write( 0xe6, 0xca);
	//dwc_otg_phy_write( 0xe7, 0x91);
/*
	dwc_otg_phy_write( 0xf0, 0xfc);
	dwc_otg_phy_write( 0xf1, 0x8c);
	dwc_otg_phy_write( 0xf2, 0x00);
	dwc_otg_phy_write( 0xf3, 0x11);
	dwc_otg_phy_write( 0xf4, 0x9b);
	dwc_otg_phy_write( 0xf5, 0x00);
	dwc_otg_phy_write( 0xf6, 0x00);
	dwc_otg_phy_write( 0xf6, 0x0a);
*/

	//page 1
	dwc_otg_phy_write( 0xf4, 0xbb);

	dwc_otg_phy_write( 0xe0, 0x25);
	dwc_otg_phy_write( 0xe1, 0xEF);
	//dwc_otg_phy_write( 0xe2, 0x00);
	//dwc_otg_phy_write( 0xe3, 0x00);
	//dwc_otg_phy_write( 0xe4, 0x00);
	dwc_otg_phy_write( 0xe5, 0x0F);
	dwc_otg_phy_write( 0xe6, 0x18);
	dwc_otg_phy_write( 0xe7, 0xE3);
#endif

#if 1
	
if(REG32(0xb8000008)&(1<<24))  //40MHz
{printk("RTL8197F u2 phy 40MHz patch\n");  
	//page 0

	dwc_otg_phy_write( 0xf4, 0x9b );
/*	
	dwc_otg_phy_write( 0xe0, 0xe2);
	dwc_otg_phy_write( 0xe1, 0x31);
*/	
	dwc_otg_phy_write( 0xe2, 0x33);
/*
	dwc_otg_phy_write( 0xe3, 0x8d);
*/	
	dwc_otg_phy_write( 0xe4, 0xc9);
/*
	dwc_otg_phy_write( 0xe5, 0x19);
*/	
	dwc_otg_phy_write( 0xe6, 0xc1);
/*
	dwc_otg_phy_write( 0xe7, 0x91);

	dwc_otg_phy_write( 0xf0, 0xfc);
	dwc_otg_phy_write( 0xf1, 0x8c);
	dwc_otg_phy_write( 0xf2, 0x00);
	dwc_otg_phy_write( 0xf3, 0x11);
	dwc_otg_phy_write( 0xf4, 0x9b);
	dwc_otg_phy_write( 0xf5, 0x04);
	dwc_otg_phy_write( 0xf6, 0x00);
	dwc_otg_phy_write( 0xf6, 0x0a);
*/

	//page 1
	dwc_otg_phy_write( 0xf4, 0xbb);
/*
	dwc_otg_phy_write( 0xe0, 0x25);
	dwc_otg_phy_write( 0xe1, 0x4F);
	dwc_otg_phy_write( 0xe2, 0x00);
	dwc_otg_phy_write( 0xe3, 0x00);
	dwc_otg_phy_write( 0xe4, 0x00);
	dwc_otg_phy_write( 0xe5, 0x0a);
*/	
	dwc_otg_phy_write( 0xe6, 0x00);
	dwc_otg_phy_write( 0xe7, 0x00);
}
else  //25MHz
{printk("RTL8197F u2 phy 25MHz patch\n"); 
	//page 0
	dwc_otg_phy_write( 0xf4, 0x9b );

	dwc_otg_phy_write( 0xe0, 0xe3);	
	dwc_otg_phy_write( 0xe1, 0x30);
	dwc_otg_phy_write( 0xe2, 0xd5);
/*
	dwc_otg_phy_write( 0xe3, 0x8d);
*/	
	dwc_otg_phy_write( 0xe4, 0xc9);
/*
	dwc_otg_phy_write( 0xe5, 0x19);
*/	
	dwc_otg_phy_write( 0xe6, 0xc1);
/*
	dwc_otg_phy_write( 0xe7, 0x91);

	dwc_otg_phy_write( 0xf0, 0xfc);
	dwc_otg_phy_write( 0xf1, 0x8c);
	dwc_otg_phy_write( 0xf2, 0x00);
	dwc_otg_phy_write( 0xf3, 0x11);
	dwc_otg_phy_write( 0xf4, 0x9b);
	dwc_otg_phy_write( 0xf5, 0x04);
	dwc_otg_phy_write( 0xf6, 0x00);
	dwc_otg_phy_write( 0xf6, 0x0a);
*/

	//page 1
	dwc_otg_phy_write( 0xf4, 0xbb);
/*
	dwc_otg_phy_write( 0xe0, 0x25);
	dwc_otg_phy_write( 0xe1, 0x4F);
	dwc_otg_phy_write( 0xe2, 0x00);
	dwc_otg_phy_write( 0xe3, 0x00);
	dwc_otg_phy_write( 0xe4, 0x00);
*/	
	dwc_otg_phy_write( 0xe5, 0x11);
	dwc_otg_phy_write( 0xe6, 0x06);
	dwc_otg_phy_write( 0xe7, 0x66);
}
#endif
}
//====================================================================
//8196D OTG init
//=========================================================================
TurnOn_OtgClkMgr()
{
	#define SYS_CLK_MANAGER 0xb8000010
	REG32(SYS_CLK_MANAGER) |= ((1<<19)|(1<<20));    //enable lx2, arb

	
//	REG32(SYS_CLK_MANAGER) &= ~(1<<29);    //disable otg	
//	mdelay(100);
	REG32(SYS_CLK_MANAGER) |= (1<<29);    //enable otg
	mdelay(100);
}
//-----------------------------------------------------------------------------------
void Set_SelUSBPort(int port)  // pass 2: is two port, other value is 1port
{	
	#define SYS_USB_SIE 0xb8000180
	if(port==2)
	{	REG32(SYS_USB_SIE) |= (1<<17);  //1: mean 2 ports. 
	}
	else
	{	REG32(SYS_USB_SIE) &= ~(1<<17);   //0: mean 2 ports.
	}
	REG32(SYS_USB_SIE) &= ~(1<<18);   //0: mean otg.
}
//===============================================================
USBPhyReset(int reset)  //1: in reset,  0: working
{

	#define SYS_USB_PHY 0xb800016c
	if(reset==0)
	REG32(SYS_USB_PHY) &= ~(1<<20);   //usbphy_reset=0	
	else
	REG32(SYS_USB_PHY) |=  (1<<20);   //usbphy_reset=1
	printk("=>do UTMI reset, r=%d\n",reset);
}
//-----------------------------------------------------------------------------------

void HangUpRes(int force, int termsel)  //force=0: not force, force=1 then watch termsel
{
	#define OTG_EXT_CFG 0xb8140300
	if(force==0)
	{
		REG32(OTG_EXT_CFG) &= ~(1<<22);  //not foce
	}
	else
	{
		REG32(OTG_EXT_CFG) |= (1<<22);	//force
		
		if(termsel==0)
		{	REG32(OTG_EXT_CFG) &= ~(1<<21);  //s_usbotg_fs_ternel=0	
		}
		else
		{	REG32(OTG_EXT_CFG) |= (1<<21);  //s_usbotg_fs_ternel=1
		}
	}
}
//-----------------------------------------------------------------------------------
void Select_IDPIN(int force, int value)  //force=0: not force, force=1 then watch termsel
{
	#define OTG_EXT_FM (0xb8140300+0x10)
	if(force==0)
	{
		REG32(OTG_EXT_FM) &= ~(1<<31);  //not foce
	}
	else
	{		
		if(value==0)
		{	REG32(OTG_EXT_FM) &= ~(1<<30);  //s_usbotg_fs_ternel=0	
		}
		else
		{	REG32(OTG_EXT_FM) |= (1<<30);  //s_usbotg_fs_ternel=1
		}
		REG32(OTG_EXT_FM) |= (1<<31);	//force
	}


}

//-----------------------------------------------------------------------------------
int Get_IDPIN()  //force=0: not force, force=1 then watch termsel
{
	#define OTG_EXT_FM (0xb8140300+0x10)
	unsigned int t=REG32(OTG_EXT_FM);
	if(t&(1<<31))  //force
	{
		if(t&(1<<30))
			return 1;   //device
		else
			return 0;   //hosr
	}
	else
	{
		return -1;  //not force
	}
}


//-----------------------------------------------------------------------------------

Enable_OTG_Suspend(int sel, int en)  //sel=0 src from sys, then see en, sel=1, src from otg mac,                                                                   
{
	#define OTG_EXT_CFG 0xb8140300

	if(sel==0)
	{
		if(en==1)
		{	REG32(OTG_EXT_CFG)&= ~(1<<27);        //s_suspend_n=0		
		}
		else
		{	REG32(OTG_EXT_CFG) |= (1<<27);        //s_suspend_n=1	  
		}
		
		REG32(OTG_EXT_CFG) &= ~(1<<26);     //s_suspend_sel=0  (source from system)		
	}
	else
	{
		REG32(OTG_EXT_CFG) |=  (1<<26);     //s_suspend_sel=1  (source from mac)
	}
}
//=====================================================================================
//idpin=0 host
//idpin=1 device
int otg_reset_procedure()
{


	TurnOn_OtgClkMgr();

#if 1
	int idpin=Get_IDPIN();
	if(idpin==-1)  //if not force mode, using default force host mode.
		idpin=0;
	Select_IDPIN(1, idpin);
	printk("Otg act as %s mode\n", Get_IDPIN()==0?"Host":"Device");
	
#endif

	Enable_OTG_Suspend(1,NULL);		//sel=0 src from sys, then see en, sel=1, src from otg mac,    
	 								//en=1 go to suspend, en=0 not in suspend, is working
	#define SYS_USB_SIE 0xb8000180	
	REG32(SYS_USB_SIE) |= (1<<0);     //en_usbotg=1,  connect MAC and PHY
	Set_SelUSBPort(2);  // pass 2: is two port, other value is 1port


	#define SYS_USB_PHY 0xb800016c 
	REG32(SYS_USB_PHY)|=(1<<19);  //USBPHY_EN=1
	REG32(SYS_USB_PHY)|=(1<<21);  //active_usbphy


//----------------------------------------
	
#if 1	
	USBPhyReset(1);  //1: in reset, 0: working
	
	USBPhyReset(0);  //1: in reset, 0: working	
#endif

	Set_otg_phy_Patch();
	dump_otg_phy();

#if 1	
	USBPhyReset(1);  //1: in reset, 0: working	
	USBPhyReset(0);  //1: in reset, 0: working		
#endif


#if 1	
	if(idpin==0)  //host mode, release term to mac control.
		HangUpRes(0,0);
#endif	



#if 0


 REG32(0xb8140300)=0x65800000;
	mdelay(100);
 REG32(0xb803000c)=0x20001408;
	mdelay(100);
 REG32(0xb8030008)=0x12f;
	mdelay(100);
//plug
 REG32(0xb8030440)=0x1100;
	mdelay(10);
 REG32(0xb8030440)=0x1000;
	mdelay(100);
 printk("dw 0xb8030440=%x\n", REG32(0xb8030440));


#endif

	return 0;

}
//=============================================================================


/**
 * This function is called when the dwc_otg_driver is installed with the
 * insmod command. It registers the dwc_otg_driver structure with the
 * appropriate bus driver. This will cause the dwc_otg_driver_probe function
 * to be called. In addition, the bus driver will automatically expose
 * attributes defined for the device and driver in the special sysfs file
 * system.
 *
 * @return
 */
 int  dwc_otg_driver_init(void)
{

	int retval = 0;
	int error;

	int rc;

  	if(1)		
  	{	//driver decide idpin. 
  		//Select_IDPIN(1,0);
	 	rc=otg_reset_procedure();	//host

	 	 //rc=otg_reset_procedure(1);	//device
  	}

	if(rc)
	{	printk("OTG: reset procedure init fail \n");
		return rc;
	}	


#if LM_INTERFACE  //wei add
	#define OTG_BASE	0xb8030000
	struct lm_device *lmdev;
	lmdev = kzalloc(sizeof(struct lm_device), GFP_KERNEL);
	if (!lmdev) {
		printk("kzalloc Imdev in dwc_otg_driver_init failed!\n");
		return -1;
	}
	printk("create lmdev=%x \n", lmdev);
	lmdev->resource.start = OTG_BASE;		//base of OTG, 0xb8030000
	lmdev->resource.end = lmdev->resource.start + 0x0003ffff;	
	lmdev->resource.flags = IORESOURCE_MEM;
	lmdev->irq = BSP_OTG_IRQ;	//irq of usb device	
	lmdev->id = 0;

	lm_device_register(lmdev);
#endif


	printk(KERN_INFO "%s: version %s\n", dwc_driver_name,
	       DWC_DRIVER_VERSION);
#ifdef LM_INTERFACE
	retval = lm_driver_register(&dwc_otg_driver);
#endif
	if (retval < 0) {
		printk(KERN_ERR "%s retval=%d\n", __func__, retval);
		return retval;
	}
#ifdef LM_INTERFACE
//	error = driver_create_file(&dwc_otg_driver.driver, &driver_attr_version);
//	error = driver_create_file(&dwc_otg_driver.driver, &driver_attr_debuglevel);
#endif
	return retval;
}

module_init(dwc_otg_driver_init);

/**
 * This function is called when the driver is removed from the kernel
 * with the rmmod command. The driver unregisters itself with its bus
 * driver.
 *
 */
void  dwc_otg_driver_cleanup(void)
{
	printk(KERN_DEBUG "dwc_otg_driver_cleanup()\n");

#ifdef LM_INTERFACE
//	driver_remove_file(&dwc_otg_driver.driver, &driver_attr_debuglevel);
//	driver_remove_file(&dwc_otg_driver.driver, &driver_attr_version);
	lm_driver_unregister(&dwc_otg_driver);
#endif

	printk(KERN_INFO "%s module removed\n", dwc_driver_name);
}

module_exit(dwc_otg_driver_cleanup);

MODULE_DESCRIPTION(DWC_DRIVER_DESC);
MODULE_AUTHOR("Synopsys Inc.");
MODULE_LICENSE("GPL");

module_param_named(otg_cap, dwc_otg_module_params.otg_cap, int, 0444);
MODULE_PARM_DESC(otg_cap, "OTG Capabilities 0=HNP&SRP 1=SRP Only 2=None");
module_param_named(opt, dwc_otg_module_params.opt, int, 0444);
MODULE_PARM_DESC(opt, "OPT Mode");
module_param_named(dma_enable, dwc_otg_module_params.dma_enable, int, 0444);
MODULE_PARM_DESC(dma_enable, "DMA Mode 0=Slave 1=DMA enabled");

module_param_named(dma_desc_enable, dwc_otg_module_params.dma_desc_enable, int,
		   0444);
MODULE_PARM_DESC(dma_desc_enable,
		 "DMA Desc Mode 0=Address DMA 1=DMA Descriptor enabled");

module_param_named(dma_burst_size, dwc_otg_module_params.dma_burst_size, int,
		   0444);
MODULE_PARM_DESC(dma_burst_size,
		 "DMA Burst Size 1, 4, 8, 16, 32, 64, 128, 256");
module_param_named(speed, dwc_otg_module_params.speed, int, 0444);
MODULE_PARM_DESC(speed, "Speed 0=High Speed 1=Full Speed");
module_param_named(host_support_fs_ls_low_power,
		   dwc_otg_module_params.host_support_fs_ls_low_power, int,
		   0444);
MODULE_PARM_DESC(host_support_fs_ls_low_power,
		 "Support Low Power w/FS or LS 0=Support 1=Don't Support");
module_param_named(host_ls_low_power_phy_clk,
		   dwc_otg_module_params.host_ls_low_power_phy_clk, int, 0444);
MODULE_PARM_DESC(host_ls_low_power_phy_clk,
		 "Low Speed Low Power Clock 0=48Mhz 1=6Mhz");
module_param_named(enable_dynamic_fifo,
		   dwc_otg_module_params.enable_dynamic_fifo, int, 0444);
MODULE_PARM_DESC(enable_dynamic_fifo, "0=cC Setting 1=Allow Dynamic Sizing");
module_param_named(data_fifo_size, dwc_otg_module_params.data_fifo_size, int,
		   0444);
MODULE_PARM_DESC(data_fifo_size,
		 "Total number of words in the data FIFO memory 32-32768");
module_param_named(dev_rx_fifo_size, dwc_otg_module_params.dev_rx_fifo_size,
		   int, 0444);
MODULE_PARM_DESC(dev_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(dev_nperio_tx_fifo_size,
		   dwc_otg_module_params.dev_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(dev_nperio_tx_fifo_size,
		 "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(dev_perio_tx_fifo_size_1,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_1,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_2,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_2,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_3,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_3,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_4,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_4,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_5,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_5,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_6,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_6,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_7,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_7,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_8,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_8,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_9,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_9,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_10,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_10,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_11,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_11,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_12,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_12,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_13,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_13,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_14,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_14,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_15,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_15,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(host_rx_fifo_size, dwc_otg_module_params.host_rx_fifo_size,
		   int, 0444);
MODULE_PARM_DESC(host_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(host_nperio_tx_fifo_size,
		   dwc_otg_module_params.host_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_nperio_tx_fifo_size,
		 "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(host_perio_tx_fifo_size,
		   dwc_otg_module_params.host_perio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_perio_tx_fifo_size,
		 "Number of words in the host periodic Tx FIFO 16-32768");
module_param_named(max_transfer_size, dwc_otg_module_params.max_transfer_size,
		   int, 0444);
/** @todo Set the max to 512K, modify checks */
MODULE_PARM_DESC(max_transfer_size,
		 "The maximum transfer size supported in bytes 2047-65535");
module_param_named(max_packet_count, dwc_otg_module_params.max_packet_count,
		   int, 0444);
MODULE_PARM_DESC(max_packet_count,
		 "The maximum number of packets in a transfer 15-511");
module_param_named(host_channels, dwc_otg_module_params.host_channels, int,
		   0444);
MODULE_PARM_DESC(host_channels,
		 "The number of host channel registers to use 1-16");
module_param_named(dev_endpoints, dwc_otg_module_params.dev_endpoints, int,
		   0444);
MODULE_PARM_DESC(dev_endpoints,
		 "The number of endpoints in addition to EP0 available for device mode 1-15");
module_param_named(phy_type, dwc_otg_module_params.phy_type, int, 0444);
MODULE_PARM_DESC(phy_type, "0=Reserved 1=UTMI+ 2=ULPI");
module_param_named(phy_utmi_width, dwc_otg_module_params.phy_utmi_width, int,
		   0444);
MODULE_PARM_DESC(phy_utmi_width, "Specifies the UTMI+ Data Width 8 or 16 bits");
module_param_named(phy_ulpi_ddr, dwc_otg_module_params.phy_ulpi_ddr, int, 0444);
MODULE_PARM_DESC(phy_ulpi_ddr,
		 "ULPI at double or single data rate 0=Single 1=Double");
module_param_named(phy_ulpi_ext_vbus, dwc_otg_module_params.phy_ulpi_ext_vbus,
		   int, 0444);
MODULE_PARM_DESC(phy_ulpi_ext_vbus,
		 "ULPI PHY using internal or external vbus 0=Internal");
module_param_named(i2c_enable, dwc_otg_module_params.i2c_enable, int, 0444);
MODULE_PARM_DESC(i2c_enable, "FS PHY Interface");
module_param_named(ulpi_fs_ls, dwc_otg_module_params.ulpi_fs_ls, int, 0444);
MODULE_PARM_DESC(ulpi_fs_ls, "ULPI PHY FS/LS mode only");
module_param_named(ts_dline, dwc_otg_module_params.ts_dline, int, 0444);
MODULE_PARM_DESC(ts_dline, "Term select Dline pulsing for all PHYs");
module_param_named(debug, g_dbg_lvl, int, 0444);
MODULE_PARM_DESC(debug, "");

module_param_named(en_multiple_tx_fifo,
		   dwc_otg_module_params.en_multiple_tx_fifo, int, 0444);
MODULE_PARM_DESC(en_multiple_tx_fifo,
		 "Dedicated Non Periodic Tx FIFOs 0=disabled 1=enabled");
module_param_named(dev_tx_fifo_size_1,
		   dwc_otg_module_params.dev_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_1, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_2,
		   dwc_otg_module_params.dev_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_2, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_3,
		   dwc_otg_module_params.dev_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_3, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_4,
		   dwc_otg_module_params.dev_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_4, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_5,
		   dwc_otg_module_params.dev_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_5, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_6,
		   dwc_otg_module_params.dev_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_6, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_7,
		   dwc_otg_module_params.dev_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_7, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_8,
		   dwc_otg_module_params.dev_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_8, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_9,
		   dwc_otg_module_params.dev_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_9, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_10,
		   dwc_otg_module_params.dev_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_10, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_11,
		   dwc_otg_module_params.dev_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_11, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_12,
		   dwc_otg_module_params.dev_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_12, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_13,
		   dwc_otg_module_params.dev_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_13, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_14,
		   dwc_otg_module_params.dev_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_14, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_15,
		   dwc_otg_module_params.dev_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_15, "Number of words in the Tx FIFO 4-768");

module_param_named(thr_ctl, dwc_otg_module_params.thr_ctl, int, 0444);
MODULE_PARM_DESC(thr_ctl,
		 "Thresholding enable flag bit 0 - non ISO Tx thr., 1 - ISO Tx thr., 2 - Rx thr.- bit 0=disabled 1=enabled");
module_param_named(tx_thr_length, dwc_otg_module_params.tx_thr_length, int,
		   0444);
MODULE_PARM_DESC(tx_thr_length, "Tx Threshold length in 32 bit DWORDs");
module_param_named(rx_thr_length, dwc_otg_module_params.rx_thr_length, int,
		   0444);
MODULE_PARM_DESC(rx_thr_length, "Rx Threshold length in 32 bit DWORDs");

module_param_named(pti_enable, dwc_otg_module_params.pti_enable, int, 0444);
module_param_named(mpi_enable, dwc_otg_module_params.mpi_enable, int, 0444);
module_param_named(lpm_enable, dwc_otg_module_params.lpm_enable, int, 0444);
MODULE_PARM_DESC(lpm_enable, "LPM Enable 0=LPM Disabled 1=LPM Enabled");

module_param_named(besl_enable, dwc_otg_module_params.besl_enable, int, 0444);
MODULE_PARM_DESC(besl_enable, "BESL Enable 0=BESL Disabled 1=BESL Enabled");
module_param_named(baseline_besl, dwc_otg_module_params.baseline_besl, int, 0444);
MODULE_PARM_DESC(baseline_besl, "Set the baseline besl value");
module_param_named(deep_besl, dwc_otg_module_params.deep_besl, int, 0444);
MODULE_PARM_DESC(deep_besl, "Set the deep besl value");

module_param_named(ic_usb_cap, dwc_otg_module_params.ic_usb_cap, int, 0444);
MODULE_PARM_DESC(ic_usb_cap,
		 "IC_USB Capability 0=IC_USB Disabled 1=IC_USB Enabled");
module_param_named(ahb_thr_ratio, dwc_otg_module_params.ahb_thr_ratio, int,
		   0444);
MODULE_PARM_DESC(ahb_thr_ratio, "AHB Threshold Ratio");
module_param_named(power_down, dwc_otg_module_params.power_down, int, 0444);
MODULE_PARM_DESC(power_down, "Power Down Mode");
module_param_named(reload_ctl, dwc_otg_module_params.reload_ctl, int, 0444);
MODULE_PARM_DESC(reload_ctl, "HFIR Reload Control");
module_param_named(dev_out_nak, dwc_otg_module_params.dev_out_nak, int, 0444);
MODULE_PARM_DESC(dev_out_nak, "Enable Device OUT NAK");
module_param_named(cont_on_bna, dwc_otg_module_params.cont_on_bna, int, 0444);
MODULE_PARM_DESC(cont_on_bna, "Enable Enable Continue on BNA");
module_param_named(ahb_single, dwc_otg_module_params.ahb_single, int, 0444);
MODULE_PARM_DESC(ahb_single, "Enable AHB Single Support");
module_param_named(adp_enable, dwc_otg_module_params.adp_enable, int, 0444);
MODULE_PARM_DESC(adp_enable, "ADP Enable 0=ADP Disabled 1=ADP Enabled");
module_param_named(otg_ver, dwc_otg_module_params.otg_ver, int, 0444);
MODULE_PARM_DESC(otg_ver, "OTG revision supported 0=OTG 1.3 1=OTG 2.0");

/** @page "Module Parameters"
 *
 * The following parameters may be specified when starting the module.
 * These parameters define how the DWC_otg controller should be
 * configured. Parameter values are passed to the CIL initialization
 * function dwc_otg_cil_init
 *
 * Example: <code>modprobe dwc_otg speed=1 otg_cap=1</code>
 *

 <table>
 <tr><td>Parameter Name</td><td>Meaning</td></tr>

 <tr>
 <td>otg_cap</td>
 <td>Specifies the OTG capabilities. The driver will automatically detect the
 value for this parameter if none is specified.
 - 0: HNP and SRP capable (default, if available)
 - 1: SRP Only capable
 - 2: No HNP/SRP capable
 </td></tr>

 <tr>
 <td>dma_enable</td>
 <td>Specifies whether to use slave or DMA mode for accessing the data FIFOs.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Slave
 - 1: DMA (default, if available)
 </td></tr>

 <tr>
 <td>dma_burst_size</td>
 <td>The DMA Burst size (applicable only for External DMA Mode).
 - Values: 1, 4, 8 16, 32, 64, 128, 256 (default 32)
 </td></tr>

 <tr>
 <td>speed</td>
 <td>Specifies the maximum speed of operation in host and device mode. The
 actual speed depends on the speed of the attached device and the value of
 phy_type.
 - 0: High Speed (default)
 - 1: Full Speed
 </td></tr>

 <tr>
 <td>host_support_fs_ls_low_power</td>
 <td>Specifies whether low power mode is supported when attached to a Full
 Speed or Low Speed device in host mode.
 - 0: Don't support low power mode (default)
 - 1: Support low power mode
 </td></tr>

 <tr>
 <td>host_ls_low_power_phy_clk</td>
 <td>Specifies the PHY clock rate in low power mode when connected to a Low
 Speed device in host mode. This parameter is applicable only if
 HOST_SUPPORT_FS_LS_LOW_POWER is enabled.
 - 0: 48 MHz (default)
 - 1: 6 MHz
 </td></tr>

 <tr>
 <td>enable_dynamic_fifo</td>
 <td> Specifies whether FIFOs may be resized by the driver software.
 - 0: Use cC FIFO size parameters
 - 1: Allow dynamic FIFO sizing (default)
 </td></tr>

 <tr>
 <td>data_fifo_size</td>
 <td>Total number of 4-byte words in the data FIFO memory. This memory
 includes the Rx FIFO, non-periodic Tx FIFO, and periodic Tx FIFOs.
 - Values: 32 to 32768 (default 8192)

 Note: The total FIFO memory depth in the FPGA configuration is 8192.
 </td></tr>

 <tr>
 <td>dev_rx_fifo_size</td>
 <td>Number of 4-byte words in the Rx FIFO in device mode when dynamic
 FIFO sizing is enabled.
 - Values: 16 to 32768 (default 1064)
 </td></tr>

 <tr>
 <td>dev_nperio_tx_fifo_size</td>
 <td>Number of 4-byte words in the non-periodic Tx FIFO in device mode when
 dynamic FIFO sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>dev_perio_tx_fifo_size_n (n = 1 to 15)</td>
 <td>Number of 4-byte words in each of the periodic Tx FIFOs in device mode
 when dynamic FIFO sizing is enabled.
 - Values: 4 to 768 (default 256)
 </td></tr>

 <tr>
 <td>host_rx_fifo_size</td>
 <td>Number of 4-byte words in the Rx FIFO in host mode when dynamic FIFO
 sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>host_nperio_tx_fifo_size</td>
 <td>Number of 4-byte words in the non-periodic Tx FIFO in host mode when
 dynamic FIFO sizing is enabled in the core.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>host_perio_tx_fifo_size</td>
 <td>Number of 4-byte words in the host periodic Tx FIFO when dynamic FIFO
 sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>max_transfer_size</td>
 <td>The maximum transfer size supported in bytes.
 - Values: 2047 to 65,535 (default 65,535)
 </td></tr>

 <tr>
 <td>max_packet_count</td>
 <td>The maximum number of packets in a transfer.
 - Values: 15 to 511 (default 511)
 </td></tr>

 <tr>
 <td>host_channels</td>
 <td>The number of host channel registers to use.
 - Values: 1 to 16 (default 12)

 Note: The FPGA configuration supports a maximum of 12 host channels.
 </td></tr>

 <tr>
 <td>dev_endpoints</td>
 <td>The number of endpoints in addition to EP0 available for device mode
 operations.
 - Values: 1 to 15 (default 6 IN and OUT)

 Note: The FPGA configuration supports a maximum of 6 IN and OUT endpoints in
 addition to EP0.
 </td></tr>

 <tr>
 <td>phy_type</td>
 <td>Specifies the type of PHY interface to use. By default, the driver will
 automatically detect the phy_type.
 - 0: Full Speed
 - 1: UTMI+ (default, if available)
 - 2: ULPI
 </td></tr>

 <tr>
 <td>phy_utmi_width</td>
 <td>Specifies the UTMI+ Data Width. This parameter is applicable for a
 phy_type of UTMI+. Also, this parameter is applicable only if the
 OTG_HSPHY_WIDTH cC parameter was set to "8 and 16 bits", meaning that the
 core has been configured to work at either data path width.
 - Values: 8 or 16 bits (default 16)
 </td></tr>

 <tr>
 <td>phy_ulpi_ddr</td>
 <td>Specifies whether the ULPI operates at double or single data rate. This
 parameter is only applicable if phy_type is ULPI.
 - 0: single data rate ULPI interface with 8 bit wide data bus (default)
 - 1: double data rate ULPI interface with 4 bit wide data bus
 </td></tr>

 <tr>
 <td>i2c_enable</td>
 <td>Specifies whether to use the I2C interface for full speed PHY. This
 parameter is only applicable if PHY_TYPE is FS.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>

 <tr>
 <td>ulpi_fs_ls</td>
 <td>Specifies whether to use ULPI FS/LS mode only.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>

 <tr>
 <td>ts_dline</td>
 <td>Specifies whether term select D-Line pulsing for all PHYs is enabled.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>
 
 <tr>
 <td>en_multiple_tx_fifo</td>
 <td>Specifies whether dedicatedto tx fifos are enabled for non periodic IN EPs.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Disabled
 - 1: Enabled (default, if available)
 </td></tr>

 <tr>
 <td>dev_tx_fifo_size_n (n = 1 to 15)</td>
 <td>Number of 4-byte words in each of the Tx FIFOs in device mode
 when dynamic FIFO sizing is enabled.
 - Values: 4 to 768 (default 256)
 </td></tr>

 <tr>
 <td>tx_thr_length</td>
 <td>Transmit Threshold length in 32 bit double words
 - Values: 8 to 128 (default 64)
 </td></tr>

 <tr>
 <td>rx_thr_length</td>
 <td>Receive Threshold length in 32 bit double words
 - Values: 8 to 128 (default 64)
 </td></tr>

<tr>
 <td>thr_ctl</td>
 <td>Specifies whether to enable Thresholding for Device mode. Bits 0, 1, 2 of 
 this parmater specifies if thresholding is enabled for non-Iso Tx, Iso Tx and
 Rx transfers accordingly.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - Values: 0 to 7 (default 0)
 Bit values indicate:
 - 0: Thresholding disabled
 - 1: Thresholding enabled
 </td></tr>

<tr>
 <td>dma_desc_enable</td>
 <td>Specifies whether to enable Descriptor DMA mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Descriptor DMA disabled
 - 1: Descriptor DMA (default, if available)
 </td></tr>

<tr>
 <td>mpi_enable</td>
 <td>Specifies whether to enable MPI enhancement mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: MPI disabled (default)
 - 1: MPI enable
 </td></tr>

<tr>
 <td>pti_enable</td>
 <td>Specifies whether to enable PTI enhancement support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: PTI disabled (default)
 - 1: PTI enable
 </td></tr>

<tr>
 <td>lpm_enable</td>
 <td>Specifies whether to enable LPM support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: LPM disabled
 - 1: LPM enable (default, if available)
 </td></tr>
  
 <tr>
 <td>besl_enable</td>
 <td>Specifies whether to enable LPM Errata support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: LPM Errata disabled (default)
 - 1: LPM Errata enable 
 </td></tr>
 
  <tr>
 <td>baseline_besl</td>
 <td>Specifies the baseline besl value.
 - Values: 0 to 15 (default 0)
 </td></tr>
 
  <tr>
 <td>deep_besl</td>
 <td>Specifies the deep besl value.
 - Values: 0 to 15 (default 15)
 </td></tr>

<tr>
 <td>ic_usb_cap</td>
 <td>Specifies whether to enable IC_USB capability.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: IC_USB disabled (default, if available)
 - 1: IC_USB enable 
 </td></tr>

<tr>
 <td>ahb_thr_ratio</td>
 <td>Specifies AHB Threshold ratio.
 - Values: 0 to 3 (default 0)
 </td></tr>

<tr>
 <td>power_down</td>
 <td>Specifies Power Down(Hibernation) Mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Power Down disabled (default)
 - 2: Power Down enabled
 </td></tr>
 
 <tr>
 <td>reload_ctl</td>
 <td>Specifies whether dynamic reloading of the HFIR register is allowed during
 run time. The driver will automatically detect the value for this parameter if
 none is specified. In case the HFIR value is reloaded when HFIR.RldCtrl == 1'b0
 the core might misbehave.
 - 0: Reload Control disabled (default)
 - 1: Reload Control enabled
 </td></tr>

 <tr>
 <td>dev_out_nak</td>
 <td>Specifies whether  Device OUT NAK enhancement enabled or no.
 The driver will automatically detect the value for this parameter if
 none is specified. This parameter is valid only when OTG_EN_DESC_DMA == 1'b1.
 - 0: The core does not set NAK after Bulk OUT transfer complete (default)
 - 1: The core sets NAK after Bulk OUT transfer complete
 </td></tr>

 <tr>
 <td>cont_on_bna</td>
 <td>Specifies whether Enable Continue on BNA enabled or no. 
 After receiving BNA interrupt the core disables the endpoint,when the
 endpoint is re-enabled by the application the  
 - 0: Core starts processing from the DOEPDMA descriptor (default)
 - 1: Core starts processing from the descriptor which received the BNA.
 This parameter is valid only when OTG_EN_DESC_DMA == 1'b1.
 </td></tr>

 <tr>
 <td>ahb_single</td>
 <td>This bit when programmed supports SINGLE transfers for remainder data
 in a transfer for DMA mode of operation. 
 - 0: The remainder data will be sent using INCR burst size (default)
 - 1: The remainder data will be sent using SINGLE burst size.
 </td></tr>

<tr>
 <td>adp_enable</td>
 <td>Specifies whether ADP feature is enabled.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: ADP feature disabled (default)
 - 1: ADP feature enabled
 </td></tr>

  <tr>
 <td>otg_ver</td>
 <td>Specifies whether OTG is performing as USB OTG Revision 2.0 or Revision 1.3
 USB OTG device.
 - 0: OTG 2.0 support disabled (default)
 - 1: OTG 2.0 support enabled 
 </td></tr>

*/
