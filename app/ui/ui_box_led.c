/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ui_box_led.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include "ui_indicator.h"

#define ELECTRICITY_LOW     20
#define ELECTRICITY_MIDDLE  60
#define ELECTRICITY_HIHG    100

void ui_show_box_soc(uint8_t soc)
{
    if (soc < ELECTRICITY_LOW)
    {
        ui_indicator(IND_ID_ELECTRICITY_LOW);
    }
    else if (soc < ELECTRICITY_MIDDLE)
    {
        ui_indicator(IND_ID_ELECTRICITY_MIDDLE);
    }
    else if (soc <= ELECTRICITY_HIHG)
    {
        ui_indicator(IND_ID_ELECTRICITY_HIHG);
    }
}

void ui_show_charging_state(uint8_t soc)
{
    if (soc < ELECTRICITY_MIDDLE)
    {
        ui_indicator(IND_ID_ELECTRICITY_LOW);
    }
    else if (soc < ELECTRICITY_HIHG)
    {
        ui_indicator(IND_ID_ELECTRICITY_MIDDLE);
    }
    else if (soc == ELECTRICITY_HIHG)
    {
        ui_indicator(IND_ID_ELECTRICITY_HIHG);
    }
}

void ui_box_off_led(void)
{
    ui_indicator(IND_ID_OFF_LED);
}
