/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file comm_box.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _COMM_BOX_H_
#define _COMM_BOX_H_

#include <stdint.h>

#include "comm_core.h"

typedef void (*chan_msg_cbk_t)(uint8_t* buff, uint16_t len);

void comm_box_register_msg_channel(const char* name, chan_msg_cbk_t cb);
int32_t comm_box_send_msg(uint8_t *buff, uint16_t len);

#endif // _COMM_BOX_H_
