/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file module_setup.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include "typedefs.h"

#include "modules.h"

extern initcall_t __initcall_start;
extern initcall_t __initcall_end;

void modules_setup(void)
{
    initcall_t *call;

    for (call = &__initcall_start; call < &__initcall_end; call++)
    {
        call->initcall();
    }
}
