/*
 * Driver module exercising q400 interfaces 
 *             & processing RNG data. 
 *
 * Copyright (C) 2018 SK telecom
 *
 */

#define REG_CLEAR 0x00000000UL

// sw_rst register (0x00)
#define FPGA_SOFT_RST_DEACTIVE (1UL << 4)
#define FPGA_SOFT_RST_ACTIVE (0UL << 4)

// Module enable register
#define FPGA_SPI_ENABLE (1UL << 4)

// SPI module control
#define FPGA_SPI_DATA_ACCESS_ENABLE (1UL << 0)
#define FPGA_TEST_MODE_S2Q400_INITIAL (1UL << 2)
#define FPGA_SPI_STATUS_RESET (1UL << 6)
#define FPGA_SPI_RNG_MODULE_ENABLE (1UL << 7)

// Burst mode
#define FPGA_SPI_IN_BUS_SEL (3UL << 0)
#define FPGA_BURST_MODE_EN (1UL << 4)
#define FPGA_QMODE (7UL << 7)

// Test mode & HW reject
#define FPGA_RESERVED1 (1UL << 0)
#define FPGA_REMOVE_HEAD_16BIT (1UL << 1)
#define FPGA_REMOVE_TAIL_16BIT (1UL << 2)
#define FPGA_REMOVE_PKT_TAIL (1UL << 3)
#define FPGA_BIST_MODE (1UL << 6)
#define FPGA_TEST_MODE_RESERVED (1UL << 7)

// q400 num
#define FPGA_Q400_NUM_AUTO (0x00000000)
#define FPGA_Q400_NUM_2 (0x00000010)
#define FPGA_Q400_NUM_12 (0x00000020)

// Q400 mode
#define FPGA_RNG_MDOE 0xA0000
#define FPGA_SAMPLE_MODE 0xA0001

// Q400 status
#define REG_INIT_STATUS_CHK (0x00000084)
#define Q400_SENSOR_READY (0x00001000)
#define Q400_SENSOR_BEING (0x00000FFF)
#define Q400_SENSOR_PKT_ERR (0xFFF00000)
#define Q400_SENSOR_PKT_ERR_SHIFT (20)
#define Q400_SENSOR_READY_MAX_CNT (1000)

// FPGA User-Logic Register
struct xilinx_fpga_regs {
	u32 sw_rst; // 0x000
	u32 module_enable; // 0x004
	u32 spi_module_ctrl; // 0x008
	u32 cmd_ram_end_addr; // 0x00c
	u32 burst_mode; // 0x010
	u32 testmode_hwreject; // 0x014
	u32 Reserved[6]; // 0x018
	//...
	u32 q400_num; // 0x030
	u32 Reserved1[20]; // 0x034
	//...
	u32 reg_init_status_chk; // 0x084
	u32 reg_loopback_test; // 0x088
	u32 Reserved2[14]; // 0x08c
	//...
	u32 batch_number; // 0x0c4
	u32 incremental_number; // 0x0c8
	u32 product_model; // 0x0cc
	u32 Reserved3[12]; // 0x0d0
	//...
	u32 version; // 0x100
	u32 firmware_build_date; // 0x104
	u32 Reserved4[296]; // 0x108
	//...
	u32 mode_sel; // 0x5a8
} __packed;

int Q400RegInit(struct xilinx_fpga_regs __iomem *regs, uint qrng_mode,
		uint qrng_num);
int Q400RegExit(struct xilinx_fpga_regs __iomem *);
int QrngPciGetSensorNum(struct xilinx_fpga_regs __iomem *);
int Q400WaitForReady(struct xilinx_fpga_regs __iomem *regs, u32 *result);
unsigned int Q400RegGetStatus(struct xilinx_fpga_regs __iomem *);
unsigned int QrngPciGetBoardVersion(struct xilinx_fpga_regs __iomem *);
int Q400GetSerialNumber(struct xilinx_fpga_regs __iomem *regs, char result[12]);
