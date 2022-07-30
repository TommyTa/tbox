/*****************************************************************************
 * Copyright (c) 2021, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file ui_indicator.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef _INDICATOR_H_
#define _INDICATOR_H_

#include "typedefs.h"

typedef enum {
    IND_ID_OFF_LED,
    IND_ID_ELECTRICITY_LOW,
    IND_ID_ELECTRICITY_MIDDLE,
    IND_ID_ELECTRICITY_HIHG,
    IND_ID_CHARGING_LOW,
    IND_ID_CHARGING_MIDDLE,
    IND_ID_CHARGING_HIHG,
    IND_ID_EAR_OFF_LED,
    IND_ID_EAR_ELECTRICITY_LOW,
    IND_ID_EAR_ELECTRICITY_MIDDLE,
    IND_ID_EAR_ELECTRICITY_HIHG,
    IND_ID_EAR_CHARGING_LOW,
    IND_ID_EAR_CHARGING_MIDDLE,
    IND_ID_EAR_CHARGING_HIHG,
    IND_ID_PAIRING,
    IND_ID_RESET,
    IND_ID_MAX,
} ind_id_t;

int32_t ui_indicator(ind_id_t id);

#endif // _INDICATOR_H_
