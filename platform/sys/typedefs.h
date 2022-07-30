/*
 * Copyright (c) 2018 Mingming Wang.
 * All rights, including trade secret rights, reserved.
 */

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#define USE_STD_LIB 1

#if USE_STD_LIB

#include <stdint.h>
#include <stdbool.h>

#else

/* This is needed for compilers that don't have a stdint.h file. */
typedef signed char                     int8_t;
typedef short                           int16_t;
typedef int                             int32_t;
typedef long long                       int64_t;
typedef unsigned char                   uint8_t;
typedef unsigned short                  uint16_t;
typedef unsigned int                    uint32_t;
typedef unsigned long long              uint64_t;

typedef signed char                     int_least8_t;
typedef short                           int_least16_t;
typedef int                             int_least32_t;
typedef long long                       int_least64_t;
typedef unsigned char                   uint_least8_t;
typedef unsigned short                  uint_least16_t;
typedef unsigned int                    uint_least32_t;
typedef unsigned long long              uint_least64_t;

typedef signed char                     int_fast8_t;
typedef int                             int_fast16_t;
typedef int                             int_fast32_t;
typedef long long                       int_fast64_t;
typedef unsigned char                   uint_fast8_t;
typedef unsigned int                    uint_fast16_t;
typedef unsigned int                    uint_fast32_t;
typedef unsigned long long              uint_fast64_t;

typedef long long                       intmax_t;
typedef unsigned long long              uintmax_t;

#endif /* USE_STD_LIB */

#ifndef NULL
#define NULL                            ((void*)0)
#endif

#ifndef bool
#define bool                            uint8_t
#endif

#ifndef true
#define true                            (bool)1
#endif

#ifndef false
#define false                           (bool)0
#endif

typedef int32_t                         xerr_t;     /**< Type for error number */
typedef uint32_t                        xtime_t;    /**< Type for time stamp */
typedef uint32_t                        xtick_t;    /**< Type for tick count */
typedef uint16_t                        xflag_t;    /**< Type for flags, O_RDONLY, O_NONBLOCK, O_SYNC */
typedef uint8_t                         xmode_t;    /**< Type for modes, FMODE_READ , FMODE_WRITE  */
typedef uint32_t                        xsize_t;    /**< Type for size number */
typedef int32_t                         xssize_t;   /**< Type for size number */
typedef uint32_t                        xdev_t;     /**< Type for device */
typedef uint32_t                        xloff_t;    /**< Type for offset */

/* Frequently used macros */
#ifndef ALIGN
#define ALIGN(val,exp)                  (((val) + ((exp)-1)) & ~((exp)-1))
#endif

#define ARRAY_SIZE(a)                   (sizeof(a)/sizeof(a[0]))
#define LAST_ELEMENT(x)                 (&x[ARRAY_SIZE(x)-1])
#define BOUND(x, min, max)              ( (x) < (min) ? (min) : ((x) > (max) ? (max):(x)) )
#define ROUND_SIZEOF(t)                 ((sizeof(t) + sizeof(int)-1) & ~(sizeof(int)-1))

#ifndef MAX
#define MAX(a,b)                        (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)                        (((a) < (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(x)                          ((x<0)?(-(x)):(x))
#endif

#define TO_STRING_A(s)                  # s
#define TO_STRING(s)                    TO_STRING_A(s)

#define BITFIELD_VAL(field, value)      (((value) & (field ## _MASK >> field ## _SHIFT)) << field ## _SHIFT)
#define SET_BITFIELD(reg, field, value) (((reg) & ~field ## _MASK) | BITFIELD_VAL(field, value))
#define GET_BITFIELD(reg, field)        (((reg) & field ## _MASK) >> field ## _SHIFT)

#endif /* _TYPEDEFS_H_ */
