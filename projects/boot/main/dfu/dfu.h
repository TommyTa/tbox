/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file dfu.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/
#ifndef __DFU_H__
#define __DFU_H__

#include "typedefs.h"

#define ACTION_CLS_FLAG                         0
#define ACTION_JUMP_TO_APP                      0
#define ACTION_UPGRADE_APP                      1
#define ACTION_CARRY_TO_APP                     2

void enter_dfu_mode(void);
int32_t dfu_carry_app(void);
void dfu_jump_app(void);

#endif /* __DFU_H__ */
