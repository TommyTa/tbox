/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file sys_cfg.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  SYSCLK_FREQ_HSI_24MHz  24000000
#define  SYSCLK_FREQ_HSI_36MHz  36000000
#define  SYSCLK_FREQ_HSI_72MHz  72000000
#define  SYSCLK_FREQ_HSI_96MHz  96000000
#define  SYSCLK_FREQ_HSE_24MHz  24000000
#define  SYSCLK_FREQ_HSE_36MHz  36000000
#define  SYSCLK_FREQ_HSE_48MHz  48000000
#define  SYSCLK_FREQ_HSE_56MHz  56000000
#define  SYSCLK_FREQ_HSE_72MHz  72000000
#define  SYSCLK_FREQ_HSE_128MHz 128000000
#define  SYSCLK_FREQ_HSE_144MHz 144000000

#define  SOFTVERSION "1.0.0"
#define  DEBUG_PRINTF 1
#define  SYSCLK_FREQ  SYSCLK_FREQ_HSI_96MHz

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CFG_H__ */
