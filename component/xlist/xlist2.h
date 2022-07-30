/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file device.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef _XLIST_H_
#define _XLIST_H_

/**
 * Double List structure
 */
struct xlist_node
{
    struct xlist_node *next;                        /**< point to next node. */
    struct xlist_node *prev;                        /**< point to prev node. */
};

typedef struct xlist_node xlist_node_t;             /**< Type for node. */
typedef struct xlist_node xlist_t;                  /**< Type for lists. */

/**
 * container_of - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * @brief initialize a list object
 */
#define XLIST_INIT(object) { &(object), &(object) }

/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
inline void xlist_init(xlist_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
inline void xlist_insert_after(xlist_t *l, xlist_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
inline void xlist_insert_before(xlist_t *l, xlist_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
inline void xlist_remove(xlist_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 */
inline int xlist_is_empty(const xlist_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
inline unsigned int xlist_len(const xlist_t *l)
{
    unsigned int len = 0;
    const xlist_t *p = l;

    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define xlist_entry(node, type, member) \
    container_of(node, type, member)

/**
 * xlist_for_each - iterate over a list
 * @pos:    the xlist_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define xlist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * xlist_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the xlist_t * to use as a loop cursor.
 * @n:      another xlist_t * to use as temporary storage
 * @head:   the head for your list.
 */
#define xlist_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * xlist_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define xlist_for_each_entry(pos, head, member) \
    for (pos = xlist_entry((head)->next, typeof(*pos), member); \
        &pos->member != (head); \
        pos = xlist_entry(pos->member.next, typeof(*pos), member))

/**
 * xlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define xlist_for_each_entry_safe(pos, n, head, member) \
    for (pos = xlist_entry((head)->next, typeof(*pos), member), \
        n = xlist_entry(pos->member.next, typeof(*pos), member); \
        &pos->member != (head); \
        pos = n, n = xlist_entry(n->member.next, typeof(*n), member))

/**
 * xlist_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define xlist_first_entry(ptr, type, member) \
    xlist_entry((ptr)->next, type, member)

#endif // _XLIST_H_
