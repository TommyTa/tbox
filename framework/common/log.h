/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file log.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include "../trace/trace.h"

#define LOG_D(str, ...) LOG_DEBUG(0, str, ##__VA_ARGS__)
#define LOG_I(str, ...) LOG_INFO(0, str, ##__VA_ARGS__)
#define LOG_N(str, ...) LOG_NOTIFY(0, str, ##__VA_ARGS__)
#define LOG_W(str, ...) LOG_WARN(0, str, ##__VA_ARGS__)
#define LOG_E(str, ...) LOG_ERROR(0, str, ##__VA_ARGS__)
#define LOG_C(str, ...) LOG_CRITICAL(0, str, ##__VA_ARGS__)

#endif // _LOG_H_
