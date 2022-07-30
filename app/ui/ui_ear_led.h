/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ui_ear_led.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef _EARLED_H_
#define _EARLED_H_

#include <stdint.h>

void ui_show_ear_soc(uint8_t soc);
void ui_show_ear_reset(void);
void ui_show_ear_pair(void);
void ui_ear_off_led(void);

#endif // _EARLED_H_
