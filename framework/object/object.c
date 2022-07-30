/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file object.c
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#include <string.h>

#include "errorno.h"
#include "sys_cmsis.h"

#include "object.h"

#define OBJECT_MALLOC(sz)               mem_malloc(sz)
#define OBJECT_FREE(ptr)                mem_free(ptr)

#define OBJECT_ENTER_CRITICAL()         sys_enter_critical()
#define OBJECT_EXIT_CRITICAL()          sys_exit_critical()

#define _OBJ_SET_LIST_INIT(t)     \
    { &(g_object_sets[t].obj_list), &(g_object_sets[t].obj_list) }


static object_set_t g_object_sets[OBJECT_CLASS_UNKNOWN - 1] =
{
    /* initialize object container - device */
    { OBJECT_CLASS_DEVICE, _OBJ_SET_LIST_INIT(OBJECT_CLASS_DEVICE - 1) },
    /* initialize object container - driver */
    { OBJECT_CLASS_DRIVER, _OBJ_SET_LIST_INIT(OBJECT_CLASS_DRIVER - 1) },
};

/**
 * This function will return the specified type of object information.
 *
 * @param type the type of object
 * @return the object type information or NULL
 */
object_set_t* object_get_obj_set(object_class_t type)
{
    int index;

    for (index = 0; index < (OBJECT_CLASS_UNKNOWN - 1); index ++)
    {
        if (g_object_sets[index].obj_type == type)
        {
            return &g_object_sets[index];
        }
    }

    return NULL;
}

/**
 * This function will initialize an object and add it to object system
 * management.
 *
 * @param object the specified object to be initialized.
 * @param type the object type.
 * @param name the object name. In system, the object's name must be unique.
 */
int32_t object_attack(object_t *object, object_class_t type, const char *name)
{
    // register uint32_t temp;
    object_set_t *obj_set = NULL;
    object_t *obj = NULL;
    xlist_node_t *node = NULL;

    /* get object information */
    obj_set = object_get_obj_set(type);
    if (obj_set == NULL)
    {
        return RETVAL(E_NULL);
    }

    /* check object type to avoid re-initialization */

    /* enter critical */
    OBJECT_ENTER_CRITICAL();
    /* try to find object */
    xlist_for_each(node, &(obj_set->obj_list))
    {
        obj = xlist_entry(node, object_t, list);
        if (obj == object)
        {
            OBJECT_EXIT_CRITICAL();
            return RETVAL(E_FAIL);
        }
    }
    /* leave critical */
    OBJECT_EXIT_CRITICAL();

    /* initialize object's parameters */
    /* set object type to static */
    object->type = type | OBJECT_CLASS_STATIC;

    /* copy name */
    strncpy(object->name, name, OBJECT_NAME_SIZE);
    object->name[OBJECT_NAME_SIZE-1] = '\0';

    /* lock interrupt */
    // temp = hw_interrupt_disable();
    OBJECT_ENTER_CRITICAL();

    /* insert object into information object list */
    xlist_insert_after(&(obj_set->obj_list), &(object->list));

    /* unlock interrupt */
    // hw_interrupt_enable(temp);
    OBJECT_EXIT_CRITICAL();

    return RETVAL(E_OK);
}

/**
 * This function will detach a static object from object system,
 * and the memory of static object is not freed.
 *
 * @param object the specified object to be detached.
 */
int32_t object_detach(object_t* object)
{
    // register uint32_t temp;

    /* object check */
    if (object == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (!object_is_static_obj(object))
    {
        return RETVAL(E_FAIL);
    }

    /* reset object type */
    object->type = OBJECT_CLASS_NULL;

    /* lock interrupt */
    // temp = hw_interrupt_disable();
    OBJECT_ENTER_CRITICAL();

    /* remove from old list */
    xlist_remove(&(object->list));

    /* unlock interrupt */
    // hw_interrupt_enable(temp);
    OBJECT_EXIT_CRITICAL();

    return RETVAL(E_OK);
}

/**
 * This function will allocate an object from object system
 *
 * @param type the type of object
 * @param name the object name. In system, the object's name must be unique.
 *
 * @return object
 */
object_t* object_alloc_attack(object_class_t type, const char *name, xsize_t obj_size)
{
    // register uint32_t temp;
    object_set_t *obj_set = NULL;
    object_t *obj = NULL;

    /* get object information */
    obj_set = object_get_obj_set(type);
    if (obj_set == NULL)
    {
        return NULL;
    }

    if (object_find(type, name) == NULL)
    {
        return NULL;
    }

    obj = (object_t *)OBJECT_MALLOC(obj_size);
    if (obj == NULL)
    {
        /* no memory can be allocated */
        return NULL;
    }

    /* clean memory data of object */
    memset(obj, 0, obj_size);

    /* initialize object's parameters */

    /* set object type */
    obj->type = type;

    /* copy name */
    strncpy(obj->name, name, OBJECT_NAME_SIZE);
    obj->name[OBJECT_NAME_SIZE-1] = '\0';

    /* lock interrupt */
    // temp = hw_interrupt_disable();
    OBJECT_ENTER_CRITICAL();

    /* insert object into information object list */
    xlist_insert_after(&(obj_set->obj_list), &(obj->list));

    /* unlock interrupt */
    // hw_interrupt_enable(temp);
    OBJECT_EXIT_CRITICAL();

    return obj;
}

/**
 * This function will delete an object and release object memory.
 *
 * @param object the specified object to be deleted.
 */
int32_t object_free_detack(object_t* object)
{
    // register uint32_t temp;

    /* object check */
    if (object == NULL)
    {
        return RETVAL(E_NULL);
    }

    if (object_is_static_obj(object))
    {
        return RETVAL(E_FAIL);
    }

    /* reset object type */
    object->type = OBJECT_CLASS_NULL;

    /* lock interrupt */
    // temp = hw_interrupt_disable();
    OBJECT_ENTER_CRITICAL();

    /* remove from old list */
    xlist_remove(&(object->list));

    /* unlock interrupt */
    // hw_interrupt_enable(temp);
    OBJECT_EXIT_CRITICAL();

    /* free the memory of object */
    OBJECT_FREE(object);

    return RETVAL(E_OK);
}

/**
 * This function will judge the object is system object or not.
 * Normally, the system object is a static object and the type
 * of object set to Object_Class_Static.
 *
 * @param object the specified object to be judged.
 *
 * @return TRUE if a system object, FALSE for others.
 */
bool object_is_static_obj(object_t* object)
{
    if (object->type & OBJECT_CLASS_STATIC)
    {
        return true;
    }

    return false;
}

/**
 * This function will return the type of object without
 * Object_Class_Static flag.
 *
 * @param object the specified object to be get type.
 *
 * @return the type of object.
 */
object_class_t object_get_type(object_t* object)
{
    if (object == NULL)
    {
        return OBJECT_CLASS_NULL;
    }

    return (object->type & ~OBJECT_CLASS_STATIC);
}

/**
 * This function will find specified name object from object
 * container.
 *
 * @param name the specified name of object.
 * @param type the type of object
 *
 * @return the found object or NULL if there is no this object
 * in object container.
 *
 * @note this function shall not be invoked in interrupt status.
 */
object_t* object_find(object_class_t type, const char *name)
{
    object_set_t *obj_set = NULL;
    object_t *obj = NULL;
    xlist_node_t *node = NULL;

    /* parameter check */
    if ((name == NULL) || (type >= OBJECT_CLASS_UNKNOWN))
    {
        return NULL;
    }

    /* try to find object */
    obj_set = object_get_obj_set(type);
    if (obj_set == NULL)
    {
        return NULL;
    }

    /* enter critical */
    OBJECT_ENTER_CRITICAL();
    xlist_for_each(node, &(obj_set->obj_list))
    {
        obj = xlist_entry(node, object_t, list);
        if (strncmp(obj->name, name, OBJECT_NAME_SIZE) == 0)
        {
            OBJECT_EXIT_CRITICAL();
            return obj;
        }
    }
    /* leave critical */
    OBJECT_EXIT_CRITICAL();

    return NULL;
}
