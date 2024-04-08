/*
 * Driver module exercising q400 interfaces 
 *             & processing RNG data. 
 *
 * Copyright (C) 2019 ID Quantique
 *
 */

#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>

#include <asm/byteorder.h>
#include <asm/cacheflush.h>
#include <asm/delay.h>
#include <asm/pci.h>

#include "xdma-core.h"
#include "idq-rng.h"
#include "quantis_ioctl.h"

int Q400RegInit(struct xilinx_fpga_regs __iomem *regs, uint qrng_mode,
		uint qrng_num)
{
	u32 user_data, ud2, u32data;
	char serial[256];
	mdelay(10);

	// reset FPGA
	iowrite32(FPGA_SOFT_RST_DEACTIVE, &regs->sw_rst);
	iowrite32(FPGA_SOFT_RST_ACTIVE, &regs->sw_rst);
	iowrite32(FPGA_SOFT_RST_DEACTIVE, &regs->sw_rst);
	// This sleep is part of the FPGA initialization procedure. Don't remove it.
	msleep(100);
	iowrite32(REG_CLEAR, &regs->spi_module_ctrl);

	// Display board info
	user_data = ioread32(&regs->version);
	pr_info(DRV_NAME ": PCB revision:  %d\n", (user_data>>24)&0xFF);
	pr_info(DRV_NAME ": Firmware version: %d.%d.%d\n", (user_data>>16)&0xFF, (user_data>>8)&0xFF, user_data&0xFF);
	user_data = ioread32(&regs->firmware_build_date);
	pr_info(DRV_NAME ": Firmware build date: %d\n", user_data);
	Q400GetSerialNumber(regs, serial);
	pr_info(DRV_NAME ": Serial number:  %s\n", serial);

	if (qrng_num == 2)
		iowrite32(FPGA_Q400_NUM_2, &regs->q400_num);
	else if (qrng_num == 12)
		iowrite32(FPGA_Q400_NUM_12, &regs->q400_num);
	else
		iowrite32(FPGA_Q400_NUM_AUTO, &regs->q400_num);

	if (qrng_mode == QUANTIS_QRNG_MODE_SAMPLE) {
		// Do not use post-processing feature of the chip
		pr_info(DRV_NAME ": SAMPLE MODE (qrng_mode:%d)\n", qrng_mode);
		u32data = REG_CLEAR;
		u32data |= (FPGA_REMOVE_TAIL_16BIT | FPGA_REMOVE_PKT_TAIL);
		iowrite32(u32data, &regs->testmode_hwreject);
		iowrite32(FPGA_SAMPLE_MODE, &regs->mode_sel);
		msleep(100);

		user_data = ioread32(&regs->testmode_hwreject);
		ud2 = ioread32(&regs->mode_sel);
		if (user_data != u32data || ud2 != FPGA_SAMPLE_MODE) {
			pr_err(DRV_NAME ": ERROR, sample mode setting ! (REG_0x14:0x%x, REG_0x5A8:0x%x) \n",
			       user_data, ud2);
		}

	} else { // HRNG: Hardware RNG
		// Use post-processing feature of the chip

		pr_info(DRV_NAME ": RNG MODE (qrng_mode:%d)\n", qrng_mode);
		u32data = REG_CLEAR;
		u32data |= (FPGA_REMOVE_HEAD_16BIT | FPGA_REMOVE_PKT_TAIL);
		iowrite32(u32data, &regs->testmode_hwreject);
		iowrite32(FPGA_RNG_MDOE, &regs->mode_sel);
		msleep(100);

		user_data = ioread32(&regs->testmode_hwreject);
		ud2 = ioread32(&regs->mode_sel);
		if (user_data != u32data || ud2 != FPGA_RNG_MDOE) {
			pr_err(DRV_NAME ": ERROR, rng mode setting !  (REG_0x14:0x%x,REG_0x5A8:0x%x)\n",
			       user_data, ud2);
		}
	}

	u32data = 0x0;
	u32data |= FPGA_SPI_STATUS_RESET;
	iowrite32(u32data, &regs->spi_module_ctrl);
	mdelay(300);

	u32data = REG_CLEAR;
	u32data |= (FPGA_SPI_ENABLE | 0x1);
	iowrite32(u32data, &regs->module_enable);

	u32data = REG_CLEAR;
	u32data |= (FPGA_SPI_RNG_MODULE_ENABLE | FPGA_SPI_DATA_ACCESS_ENABLE);
	iowrite32(u32data, &regs->spi_module_ctrl);

	user_data = ioread32(&regs->mode_sel);
	pr_debug(DRV_NAME ": mode : %s (0x05A8:0x%x)\n",
	       (user_data == FPGA_RNG_MDOE) ? "RNG" : "SAMPLE", user_data);

	return 1;
}

int Q400RegExit(struct xilinx_fpga_regs __iomem *regs)
{
	u32 user_data, u32data;

	iowrite32(0xA1, &regs->spi_module_ctrl);
	user_data = ioread32(&regs->spi_module_ctrl);
	pr_debug(DRV_NAME ": xdma_user 0x08: 0x%x (expected:0xA1)\n", user_data);
	mdelay(1000);

	u32data = REG_CLEAR;
	u32data |= FPGA_SPI_STATUS_RESET;
	iowrite32(u32data, &regs->spi_module_ctrl);
	user_data = ioread32(&regs->spi_module_ctrl);
	pr_debug(DRV_NAME ": xdma_user 0x08: 0x%x (expected:0x40)\n", user_data);
	mdelay(1);

	u32data = REG_CLEAR;
	iowrite32(u32data, &regs->spi_module_ctrl);
	user_data = ioread32(&regs->spi_module_ctrl);
	pr_debug(DRV_NAME ": xdma_user 0x08: 0x%x (expected:0x00)\n", user_data);
	mdelay(100);

	return 1;
}

int QrngPciGetSensorNum(struct xilinx_fpga_regs __iomem *regs)
{
	unsigned int recvsensornum;

	// get sensor number connected.
	recvsensornum = ioread32(&regs->q400_num);
	if (recvsensornum == FPGA_Q400_NUM_2)
		recvsensornum = 2;
	else
		recvsensornum = 12;

	return (int)recvsensornum;
}

int Q400WaitForReady(struct xilinx_fpga_regs __iomem *regs, u32 *result)
{
	u32 cnt = 0;
	u32 status_result;

	do {
		mdelay(1);
		status_result = ioread32(&regs->reg_init_status_chk);
	} while (!(status_result & Q400_SENSOR_READY) &&
		 (cnt++ < Q400_SENSOR_READY_MAX_CNT));

	if (cnt > Q400_SENSOR_READY_MAX_CNT) {
		return -1;
	}

	if (result != NULL) {
		*result = status_result;
	}

	return 0;
}

unsigned int Q400RegGetStatus(struct xilinx_fpga_regs __iomem *regs)
{
	u32 status_result;

	if (Q400WaitForReady(regs, &status_result) == -1) {
		return -1;
	}

	// check at least one sensor activated
	if ((status_result & Q400_SENSOR_BEING) == 0) {
		pr_err(DRV_NAME ": Error, no sensor connected! (status register : 0x%x )\n",
		       status_result);
		return -1;
	}

	// check each sensor packet error.
	if ((status_result & Q400_SENSOR_PKT_ERR) != 0) {
		pr_err(DRV_NAME ": Error, data broken!  (status register : 0x%x )\n",
		       status_result);
		return -1;
	}
	return status_result;
}

unsigned int QrngPciGetBoardVersion(struct xilinx_fpga_regs __iomem *regs)
{
	unsigned int recvverinfo;

	// read the information of version.
	recvverinfo = ioread32(&regs->version);
	return recvverinfo;
}

int Q400GetSerialNumber(struct xilinx_fpga_regs __iomem *regs, char result[12])
{
	u32 batch_number;
	u32 incremental_number;
	u32 product_model;

	batch_number = ioread32(&regs->batch_number);
	incremental_number = ioread32(&regs->incremental_number);
	product_model = ioread32(&regs->product_model);

	result[0] = (batch_number >> 0) & 0xFF;
	result[1] = (batch_number >> 8) & 0xFF;
	result[2] = (batch_number >> 16) & 0xFF;
	result[3] = (batch_number >> 24) & 0xFF;

	result[4] = (incremental_number >> 0) & 0xFF;
	result[5] = (incremental_number >> 8) & 0xFF;
	result[6] = (incremental_number >> 16) & 0xFF;

	result[7] = (product_model >> 0) & 0xFF;
	result[8] = (product_model >> 8) & 0xFF;
	result[9] = (product_model >> 16) & 0xFF;
	result[10] = (product_model >> 24) & 0xFF;

	result[11] = '\0';

	return 0;
}
