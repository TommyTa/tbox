/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file modules.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _MODULES_H_
#define _MODULES_H_

#include "typedefs.h"

/*
 * INIT_ATCMD
 */
typedef struct {
    char* name;
    uint8_t level;
    int32_t (*callback)(char* argv[], int argc);
} atcmd_info_t;

#define ATCMD_INIT(name, level, callback)   \
    static atcmd_info_t __atcmd_##callback __attribute__((used)) \
    __attribute__((section(".init_atcmd."))) = { name, level, callback }

/*
 * INIT_CALL
 */
typedef struct {
    char* name;
    void (*initcall)(void);
} initcall_t;

#define __define_initcall(name, level, fn)  \
    static initcall_t __initcall_##fn __attribute__((used))   \
    __attribute__((section(".initcall." #level ".init"))) = { name, fn }

#define CORE_INITCALL(name, fn)     __define_initcall(name, 1, fn)
#define BASIC_INITCALL(name, fn)    __define_initcall(name, 2, fn)
#define DRIVER_INITCALL(name, fn)   __define_initcall(name, 3, fn)
#define SUBSYS_INITCALL(name, fn)   __define_initcall(name, 4, fn)
#define DEVICE_INITCALL(name, fn)   __define_initcall(name, 5, fn)
#define APP_INITCALL(name, fn)      __define_initcall(name, 6, fn)
#define LATE_INITCALL(name, fn)     __define_initcall(name, 7, fn)

void modules_setup();

#endif // _MODULES_H_
