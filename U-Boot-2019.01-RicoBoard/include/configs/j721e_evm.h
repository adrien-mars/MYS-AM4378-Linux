/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for K3 J721E EVM
 *
 * Copyright (C) 2018-2019 Texas Instruments Incorporated - http://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 */

#ifndef __CONFIG_J721E_EVM_H
#define __CONFIG_J721E_EVM_H

#include <linux/sizes.h>
#include <config_distro_bootcmd.h>
#include <environment/ti/mmc.h>
#include <environment/ti/k3_rproc.h>
#include <environment/ti/k3_dfu.h>
#include <environment/ti/ufs.h>

#define CONFIG_ENV_SIZE			(128 << 10)

/* DDR Configuration */
#define CONFIG_SYS_SDRAM_BASE1		0x880000000

/* SPL Loader Configuration */
#if defined(CONFIG_TARGET_J721E_A72_EVM)
#define CONFIG_SPL_TEXT_BASE		0x80080000
#define CONFIG_SYS_INIT_SP_ADDR         (CONFIG_SPL_TEXT_BASE +	\
					 CONFIG_SYS_K3_NON_SECURE_MSRAM_SIZE)
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x280000
#define CONFIG_SYS_UBOOT_BASE		0x50280000
#define CONFIG_SPL_LOAD_FIT_ADDRESS	0x81000000
#else
#define CONFIG_SPL_TEXT_BASE		0x41c00000
#define CONFIG_SYS_UBOOT_BASE		0x50080000
#define CONFIG_SPL_LOAD_FIT_ADDRESS	0x80080000
/*
 * Maximum size in memory allocated to the SPL BSS. Keep it as tight as
 * possible (to allow the build to go through), as this directly affects
 * our memory footprint. The less we use for BSS the more we have available
 * for everything else.
 */
#define CONFIG_SPL_BSS_MAX_SIZE		0xA000
/*
 * Link BSS to be within SPL in a dedicated region located near the top of
 * the MCU SRAM, this way making it available also before relocation. Note
 * that we are not using the actual top of the MCU SRAM as there is a memory
 * location filled in by the boot ROM that we want to read out without any
 * interference from the C context.
 */
#define CONFIG_SPL_BSS_START_ADDR	(CONFIG_SYS_K3_BOOT_PARAM_TABLE_INDEX -\
					 CONFIG_SPL_BSS_MAX_SIZE)
/* Set the stack right below the SPL BSS section */
#define CONFIG_SYS_INIT_SP_ADDR         CONFIG_SPL_BSS_START_ADDR
/* Configure R5 SPL post-relocation malloc pool in DDR */
#define CONFIG_SYS_SPL_MALLOC_START	0x84000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_16M
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x80000
#endif

#ifdef CONFIG_SYS_K3_SPL_ATF
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"tispl.bin"
#endif

#define CONFIG_SPL_MAX_SIZE		CONFIG_SYS_K3_MAX_DOWNLODABLE_IMAGE_SIZE

#define CONFIG_SYS_BOOTM_LEN		SZ_64M

#define CONFIG_SF_DEFAULT_SPEED		0
#define CONFIG_SF_DEFAULT_MODE		0
#define CONFIG_CQSPI_REF_CLK		133333333

/* HyperFlash related configuration */
#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT 1

/* U-Boot general configuration */
#define EXTRA_ENV_J721E_BOARD_SETTINGS					\
	"findfdt="							\
		"if test $board_name = J721EX-PM2-SOM; then "		\
			"setenv fdtfile k3-j721e-common-proc-board.dtb; fi;" \
		"if test $board_name = J721EX-PM1-SOM; then "		\
			"setenv fdtfile k3-j721e-proc-board-tps65917.dtb; fi;" \
		"setenv overlay_files ${name_overlays}\0"		\
	"loadaddr=0x80080000\0"						\
	"fdtaddr=0x82000000\0"						\
	"overlayaddr=0x83000000\0"					\
	"name_kern=Image\0"						\
	"console=ttyS2,115200n8\0"					\
	"args_all=setenv optargs earlycon=ns16550a,mmio32,0x02800000 "	\
		"${mtdparts}\0"						\
	"run_kern=booti ${loadaddr} ${rd_spec} ${fdtaddr}\0"

#define PARTS_DEFAULT \
	/* Linux partitions */ \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=rootfs,start=0,size=-,uuid=${uuid_gpt_rootfs}\0" \
	/* Android partitions */ \
	"partitions_android=" \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=bootloader,start=5M,size=8M,uuid=${uuid_gpt_bootloader};" \
	"name=tiboot3,start=4M,size=1M,uuid=${uuid_gpt_tiboot3};" \
	"name=boot,start=13M,size=40M,uuid=${uuid_gpt_boot};" \
	"name=vendor,size=1024M,uuid=${uuid_gpt_vendor};" \
	"name=system,size=2048M,uuid=${uuid_gpt_system};" \
	"name=userdata,size=-,uuid=${uuid_gpt_userdata}\0" \

/* U-Boot MMC-specific configuration */
#define EXTRA_ENV_J721E_BOARD_SETTINGS_MMC				\
	"boot=mmc\0"							\
	"mmcdev=1\0"							\
	"bootpart=1:2\0"						\
	"bootdir=/boot\0"						\
	"mainr5f0_0loadaddr=88000000\0"					\
	"mainr5f0_0fwname=/lib/firmware/j7-main-r5f0_0-fw\0"		\
	"mcur5f0_0loadaddr=89000000\0"					\
	"mcur5f0_0fwname=/lib/firmware/j7-mcu-r5f0_0-fw\0"		\
	"rd_spec=-\0"							\
	"init_mmc=run args_all args_mmc\0"				\
	"get_fdt_mmc=load mmc ${bootpart} ${fdtaddr} ${bootdir}/${fdtfile}\0" \
	"get_overlay_mmc="						\
		"fdt address ${fdtaddr};"				\
		"fdt resize 0x100000;"					\
		"for overlay in $overlay_files;"			\
		"do;"							\
		"load mmc ${bootpart} ${overlayaddr} ${bootdir}/${overlay} && "	\
		"fdt apply ${overlayaddr};"				\
		"done;\0"						\
	"partitions=" PARTS_DEFAULT					\
	"get_kern_mmc=load mmc ${bootpart} ${loadaddr} "		\
		"${bootdir}/${name_kern}\0"

/* Commands for booting Android from eMMC */
#define EXTRA_ENV_J721E_BOARD_SETTINGS_EMMC_ANDROID			\
	"check_dofastboot="						\
		"if test ${dofastboot} -eq 1; then "			\
			"echo Boot fastboot requested, "		\
				"resetting dofastboot ...;"		\
			"setenv dofastboot 0; env save; "		\
			"echo Booting into fastboot ...; "		\
			"fastboot "					\
			__stringify(CONFIG_FASTBOOT_USB_DEV) "; "	\
		"fi\0"							\
	"check_android="						\
		"setenv mmcdev 0; "					\
		"env delete boot_start; "				\
		"part start mmc ${mmcdev} boot boot_start; "		\
		"if test \"$boot_start\" = \"\"; then "			\
			"env set is_android 0; "			\
		"else "							\
			"env set is_android 1; "			\
		"fi; "							\
		"env delete boot_start\0"				\
	"emmc_android_boot="						\
		"echo Trying to boot Android from eMMC ...; "		\
		"run update_to_fit; "					\
		"setenv eval_bootargs setenv bootargs $bootargs; "	\
		"run eval_bootargs; "					\
		"setenv mmcdev 0; "					\
		"mmc dev $mmcdev; "					\
		"mmc rescan; "						\
		"part start mmc ${mmcdev} boot boot_start; "		\
		"part size mmc ${mmcdev} boot boot_size; "		\
		"mmc read ${fit_loadaddr} ${boot_start} ${boot_size}; "	\
		"run get_overlaystring; "				\
		"run run_fit\0"

#ifdef DEFAULT_RPROCS
#undef DEFAULT_RPROCS
#endif
#define DEFAULT_RPROCS	""						\
		"3 /lib/firmware/j7-main-r5f0_1-fw "			\
		"4 /lib/firmware/j7-main-r5f1_0-fw "			\
		"6 /lib/firmware/j7-c66_0-fw "				\
		"7 /lib/firmware/j7-c66_1-fw "				\
		"8 /lib/firmware/j7-c71_0-fw "

#ifdef CONFIG_TARGET_J721E_A72_EVM
#define EXTRA_ENV_J721E_BOARD_SETTINGS_MTD				\
	"mtdids=" CONFIG_MTDIDS_DEFAULT "\0"				\
	"mtdparts=" CONFIG_MTDPARTS_DEFAULT "\0"
#else
#define EXTRA_ENV_J721E_BOARD_SETTINGS_MTD
#endif

/* set default dfu_bufsiz to 128KB (sector size of OSPI) */
#define DFUARGS \
	"dfu_bufsiz=0x20000\0" \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_EMMC \
	DFU_ALT_INFO_RAM \
	DFU_ALT_INFO_OSPI

/* Incorporate settings into the U-Boot environment */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	DEFAULT_MMC_TI_ARGS						\
	DEFAULT_FIT_TI_ARGS						\
	EXTRA_ENV_J721E_BOARD_SETTINGS					\
	EXTRA_ENV_J721E_BOARD_SETTINGS_MMC				\
	EXTRA_ENV_J721E_BOARD_SETTINGS_EMMC_ANDROID			\
	EXTRA_ENV_RPROC_SETTINGS					\
	EXTRA_ENV_J721E_BOARD_SETTINGS_MTD				\
	DFUARGS								\
	DEFAULT_UFS_TI_ARGS

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

/* MMC ENV related defines */
#ifdef CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART	1
#define CONFIG_ENV_SIZE		(128 << 10)
#define CONFIG_ENV_OFFSET		0x680000
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#endif

#define CONFIG_SUPPORT_EMMC_BOOT

#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET		0x680000
#define CONFIG_ENV_SECT_SIZE		0x20000
#define CONFIG_ENV_OFFSET_REDUND        (CONFIG_ENV_OFFSET + \
					 CONFIG_ENV_SECT_SIZE)
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#endif

#endif /* __CONFIG_J721E_EVM_H */
