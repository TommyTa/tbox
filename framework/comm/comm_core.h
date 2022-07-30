/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file comm_core.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _COMM_CORE_H_
#define _COMM_CORE_H

#include <stdint.h>

#include "sys_cmsis.h"

typedef struct {
    uint8_t name;
    sys_msg_id_t msg_id;
} comm_node_t;

typedef struct {
    uint8_t *buff;
    uint16_t len;
} comm_msg_t;

typedef struct {
    uint8_t channel;
    uint8_t response : 4;
    uint8_t noprefix : 4;
    uint8_t src;
    uint8_t dst;
    uint16_t len;
} __attribute__((packed)) comm_hdr_t;

int32_t comm_register_node(uint8_t name, sys_msg_id_t msg_id);
int32_t comm_forward_msg(comm_hdr_t *p_hdr, uint8_t *buff, uint16_t len);

#endif // _COMM_CORE_H_
