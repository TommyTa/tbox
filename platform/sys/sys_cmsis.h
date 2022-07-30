/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file sys_cmsis.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef __SYS_CMSIS_H__
#define __SYS_CMSIS_H__

#include "typedefs.h"

/// Timeout value.
/// \note MUST REMAIN UNCHANGED: \b osWaitForever shall be consistent in every CMSIS-RTOS.
#define SYS_WAIT_FOREVER    0xFFFFFFFF  ///< wait forever timeout value

typedef void * sys_thread_id_t;
typedef void * sys_msg_id_t;
typedef void * sys_mutex_id_t;
typedef void * sys_timer_id_t;
typedef void * sys_sem_id_t;

typedef void (*sys_pthread) (void const *argument);
typedef void (*sys_ptimer) (void const *argument);

typedef struct {
  char                      *name;         ///< Thread name
  sys_pthread               pthread;       ///< start address of thread function
  int16_t                   tpriority;     ///< initial thread priority
  uint32_t                  stack_size;    ///< stack size requirements in bytes; 0 is default stack size
} sys_thread_def_t;

typedef struct {
  uint32_t                  queue_sz;      ///< number of elements in the queue
  uint32_t                  item_sz;       ///< size of an item
} sys_msg_qdef_t;

typedef struct {
  uint32_t                  dummy;         ///< dummy value.
} sys_mutex_def_t;

typedef struct {
  sys_ptimer                ptimer;        ///< start address of a timer function
} sys_timer_def_t;

typedef struct {
  uint32_t                  dummy;         ///< dummy value.
} sys_sem_qdef_t;

typedef struct {
  int32_t                   status;        ///< status code: event or error information
  union{
    uint32_t                v;              ///< message as 32-bit value
    void                    *p;             ///< message or mail as void pointer
    int32_t                 signals;        ///< signal flags
  }value;                                   ///< event value
  sys_msg_id_t              message_id;     ///< message id obtained by \ref osMessageCreate
} sys_event_t;

typedef enum {
  SYS_PRIORITY_IDLE = 0,                ///< priority: idle (lowest)
  SYS_PRIORITY_LOW,                     ///< priority: low
  SYS_PRIORITY_BELOW_NORMAL,            ///< priority: below normal
  SYS_PRIORITY_NORMAL,                  ///< priority: normal (default)
  SYS_PRIORITY_ABOVE_NORMAL,            ///< priority: above normal
  SYS_PRIORITY_HIGH,                    ///< priority: high
  SYS_PRIORITY_REALTIME,                ///< priority: realtime (highest)
  SYS_PRIORITY_ERROR                    ///< system cannot determine priority or thread has illegal priority
} sys_priority;

typedef enum {
  SYS_TIMER_ONCE = 0,                   ///< one-shot timer
  SYS_TIMER_PERIODIC                    ///< repeating timer
} sys_timer_t;

/// Status code values returned by CMSIS-RTOS functions.
/// \note MUST REMAIN UNCHANGED: \b osStatus shall be consistent in every CMSIS-RTOS.
typedef enum {
  SYS_OK                     =     0,       ///< function completed; no error or event occurred.
  SYS_EVENT_SIGNAL           =  0x08,       ///< function completed; signal event occurred.
  SYS_EVENT_MESSAGE          =  0x10,       ///< function completed; message event occurred.
  SYS_EVENT_MAIL             =  0x20,       ///< function completed; mail event occurred.
  SYS_EVENT_TIMEOUT          =  0x40,       ///< function completed; timeout occurred.
  SYS_ERROR_PARAMETER        =  0x80,       ///< parameter error: a mandatory parameter was missing or specified an incorrect object.
  SYS_ERROR_RESOURCE         =  0x81,       ///< resource not available: a specified resource was not available.
  SYS_ERROR_TIMEOUTRESOURCE  =  0xC1,       ///< resource not available within given time: a specified resource was not available within the timeout period.
  SYS_ERROR_ISR              =  0x82,       ///< not allowed in ISR context: the function cannot be called from interrupt service routines.
  SYS_ERROR_ISRRECURSIVE     =  0x83,       ///< function called multiple times from ISR with same object.
  SYS_ERROR_PRIORITY         =  0x84,       ///< system cannot determine priority or thread has illegal priority.
  SYS_ERROR_NOMEMORY         =  0x85,       ///< system is out of memory: it was impossible to allocate or reserve memory for the operation.
  SYS_ERROR_VALUE            =  0x86,       ///< value of a parameter is out of range.
  SYS_ERROR_OS               =  0xFF,       ///< unspecified RTOS error: run-time error but no other error message fits.
  SYS_STATUS_RESERVED        =  0x7FFFFFFF  ///< prevent from enum down-size compiler optimization.
} sys_status;

// sys api
void sys_jump_addr(uint32_t addr);
void sys_reset(void);

// tick
void sys_tick_handler(void);

// Operating system boot
int32_t sys_kernel_start(void);

// Time delay function
int32_t sys_delay(uint32_t millisec);

// Critical region
void sys_enter_critical(void);
void sys_exit_critical(void);
uint32_t sys_irq_save(void);
void sys_irq_restore(uint32_t state);
void sys_irq_disable(void);
void sys_irq_enable(void);
void sys_set_nvic(void);

// Mem
void *mem_malloc(xsize_t size);
void mem_free(void *pv);

// Thread
#define SYS_THREAD_DEF(name, priority, stacksz) \
const sys_thread_def_t sys_thread_def_##name = \
{#name, (name), (priority), (stacksz)}

#define SYS_THREAD(name)    (&sys_thread_def_##name)

sys_thread_id_t sys_thread_create(const sys_thread_def_t *thread_def, void *arg);
int32_t sys_thread_resume(sys_thread_id_t thread_id);
int32_t sys_thread_resume_all(void);
int32_t sys_thread_suspend(sys_thread_id_t thread_id);
int32_t sys_thread_suspend_all(void);
int32_t sys_thread_terminate(sys_thread_id_t thread_id);

// Message queue
#define SYS_MSG_QDEF(name, queue_sz, type) \
const sys_msg_qdef_t sys_message_qdef_##name = \
{(queue_sz), sizeof (type)}

#define SYS_MSG_Q(name)    (&sys_message_qdef_##name)

sys_msg_id_t sys_msg_create(const sys_msg_qdef_t *queue_def, sys_thread_id_t thread_id);
int32_t sys_msg_put(sys_msg_id_t queue_id, uint32_t info, uint32_t millisec);
sys_event_t sys_msg_get(sys_msg_id_t queue_id, uint32_t millisec);

// Mutex
#define SYS_MUTEX_DEF(name) \
const sys_mutex_def_t sys_mutex_def_##name = { 0 }

#define SYS_MUTEX(name)    (&sys_mutex_def_##name)

sys_mutex_id_t sys_mutex_create(const sys_mutex_def_t *mutex_def);
int32_t sys_mutex_lock(sys_mutex_id_t mutex_id, uint32_t timeout);
int32_t sys_mutex_unlock(sys_mutex_id_t mutex_id);
int32_t sys_mutex_delete(sys_mutex_id_t mutex_id);

// Timer
#define SYS_TIMER_DEF(name, function) \
const sys_timer_def_t sys_timer_def_##name = \
{(function)}

#define SYS_TIMER(name)    (&sys_timer_def_##name)

sys_timer_id_t sys_timer_create(const sys_timer_def_t *timer_def, sys_timer_t type, void *arg);
int32_t sys_timer_start(sys_timer_id_t timer_id, uint32_t millisec);
int32_t sys_timer_stop(sys_timer_id_t timer_id);
int32_t sys_timer_delete(sys_timer_id_t timer_id);

// Semaphore
#define SYS_SEM_DEF(name)    const sys_sem_qdef_t sys_sem_def_##name = {0}
#define SYS_SEM(name)        (&sys_sem_def_##name)

sys_sem_id_t sys_sem_create(const sys_sem_qdef_t *sem_def, int32_t count);
int32_t sys_sem_wait(sys_sem_id_t sem_id);
int32_t sys_sem_post(sys_sem_id_t sem_id);
int32_t sys_sem_delete(sys_sem_id_t sem_id);

#endif /* __SYS_CMSIS_H__ */
