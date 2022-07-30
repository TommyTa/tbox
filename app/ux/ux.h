/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ux.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef _APP_UX_H_
#define _APP_UX_H_

#include <stdint.h>

#include "sys_cmsis.h"

#define UX_SIGNAL_BOX_CLOSE              (0x00)
#define UX_SIGNAL_BOX_OPEN               (0x01)
#define UX_SIGNAL_BOX_LONG               (0x02)
#define UX_SIGNAL_BOX_LONG_LONG          (0x03)
#define UX_SIGNAL_BOX_FIVE_CLICK         (0x04)

void ux_event_put_signal(uint32_t signal);

#endif // _APP_UX_H_
