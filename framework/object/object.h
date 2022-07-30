/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file object.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "typedefs.h"
#include "xlist2.h"

#define OBJECT_NAME_SIZE     10

/**
 *  The object type can be one of the follows with specific
 */
typedef enum
{
    OBJECT_CLASS_NULL,
    OBJECT_CLASS_DEVICE,                    /**< The object is a device */
    OBJECT_CLASS_DRIVER,                    /**< The object is a driver */
    OBJECT_CLASS_UNKNOWN,                   /**< The object is unknown. */
    OBJECT_CLASS_STATIC = 0x80              /**< The object is a static object. */
} object_class_t;

/**
 * Base structure of Kernel object
 */
typedef struct
{
    object_class_t  type;                   /**< type of kernel object */
    char            name[OBJECT_NAME_SIZE]; /**< name of kernel object */
    xlist_t         list;                   /**< list node of kernel object */
} object_t;

/**
 * The information of the kernel object
 */
typedef struct
{
    object_class_t  obj_type;               /**< object class type */
    xlist_t         obj_list;               /**< object list */
} object_set_t;


object_set_t* object_get_obj_set(object_class_t type);

int32_t object_attack(object_t *object, object_class_t type, const char *name);
int32_t object_detach(object_t* object);

object_t* object_alloc_attack(object_class_t type, const char *name, xsize_t obj_size);
int32_t object_free_detack(object_t* object);

bool object_is_static_obj(object_t* object);

object_class_t object_get_type(object_t* object);
object_t* object_find(object_class_t type, const char *name);

#endif // _OBJECT_H_
