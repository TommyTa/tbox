/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file x_stype.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <string.h>

#include "x_stype.h"

bool is_str_hex(const char* s)
{
    size_t size = strlen(s);
    if (s != NULL && size > 2)
    {
        if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
        {
            s += 2;
            do {
                if ((*s < '0' || *s > '9') &&
                    (*s < 'a' || *s > 'f') &&
                    (*s < 'A' || *s > 'F'))
                {
                    return false;
                }
            } while (*(++s) != '\0');
            return true;
        }
    }

    return false;
}

bool is_str_digit(const char* s)
{
    size_t size = strlen(s);
    if (s != NULL && size > 0)
    {
        do{
            if (*s < '0' || *s > '9')
            {
                return false;
            }
        } while (*(++s) != '\0');
        return true;
    }

    return false;
}
