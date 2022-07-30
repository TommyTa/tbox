/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ui_box_led.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef _BOXLED_H_
#define _BOXLED_H_

#include <stdint.h>

void ui_show_box_soc(uint8_t soc);
void ui_show_charging_state(uint8_t soc);
void ui_box_off_led(void);

#endif // _BOXLED_H_
