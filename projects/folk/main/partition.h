/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file partition.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef __PARTITION_H__
#define __PARTITION_H__

/*
 * FLASH(512K)                      : 0x0800_0000 – 0x0807_FFFF
 * SRAM(128KB)                      : 0x2000_0000 – 0x2001_FFFF
 * Retention SRAM(16KB)             : 0x2002_0000 – 0x2002_3FFF
 */

/* Heap 8KB */
#define MEM_HEAP_SIZE               0x2000
/* Stack 4KB */
#define MEM_STACK_SIZE              0x1000

/* Memory base */
#define MEM_FLASH_BASE              0x08000000
#define MEM_SRAM_BASE               0x20000000

/* Bootloader memory area */
#define FLASH_BOOT_REGION_BASE      MEM_FLASH_BASE

#if SUPPORT_BOOTLOADER
/* 16KB */
#define FLASH_BOOT_REGION_SIZE      0x4000
#define SRAM_BOOT_REGION_SIZE       0x800
#else
/* 0KB */
#define FLASH_BOOT_REGION_SIZE      0x0
#define SRAM_BOOT_REGION_SIZE       0x0
#endif

/* Application memory area */
#define FLASH_APP_REGION_BASE       FLASH_BOOT_REGION_BASE + FLASH_BOOT_REGION_SIZE
/* 128KB */
#define FLASH_APP_REGION_SIZE       0x20000

/* SARM area */
#define SRAM_APP_REGION_BASE        MEM_SRAM_BASE + SRAM_BOOT_REGION_SIZE
/* 128KB */
#define SRAM_APP_REGION_SIZE        0x20000

/* OTA memory area */
#define FLASH_OTA_REGION_BASE       FLASH_APP_REGION_BASE + FLASH_APP_REGION_SIZE
/* 128KB */
#define FLASH_OTA_REGION_SIZE       SRAM_BOOT_REGION_SIZE

#endif /* __PARTITION_H__ */
