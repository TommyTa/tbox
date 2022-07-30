/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file sys_cmsis.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include "errorno.h"
#include "cmsis_os.h"

#include "sys_cmsis.h"

#define PRIORITY_CONVERSION(N)           (N-3)


typedef void (*app_func_t)(void);

void sys_jump_addr(uint32_t addr)
{
    app_func_t jump2app;

    if ((*(uint32_t *)addr) & 0x0FFFFFFF)
    {
        jump2app = (app_func_t) * (uint32_t *)(addr + 4);
        jump2app();
    }
}

void sys_tick_handler(void)
{
    osSystickHandler();
}

// Operating system boot
int32_t sys_kernel_start(void)
{
    osKernelStart();
    return RETVAL(E_OK);
}

// Time delay function
int32_t sys_delay(uint32_t millisec)
{
    osDelay(millisec);
    return RETVAL(E_OK);
}

// Critical region
void sys_enter_critical(void)
{
    taskENTER_CRITICAL();
}

void sys_exit_critical(void)
{
    taskEXIT_CRITICAL();
}

uint32_t sys_irq_save(void)
{
    /* 1. Read PRIMASK to R0, where R0 is the return value
       2. PRIMASK=1, off interrupt (NMI and hardware FAULT can respond)
       3. return
    */
    __asm volatile
    (
        "MRS r0, primask;\n"
        "cpsid I;\n"
        "bx lr"
    );
    return RETVAL(E_OK);
}

void sys_irq_restore(uint32_t state)
{
    /* 1. Read R0 into PRIMASK with R0 as a parameter
       2. return
    */
	__asm volatile
    (
        "MSR primask, r0;\n" /* The FPU enable bits are in the CPACR. */
        "bx lr"
    );
}

// MEN MALLOC
void *mem_malloc(xsize_t size)
{
    return pvPortMalloc(size);
}

void mem_free(void *pv)
{
    if (pv == NULL)
    {
        return;
    }
    vPortFree(pv);
}

// Thread
sys_thread_id_t sys_thread_create(const sys_thread_def_t *thread_def, void *arg)
{
    osThreadDef_t thread_def_t;

    thread_def_t.name       = thread_def->name;
    thread_def_t.pthread    = thread_def->pthread;
    thread_def_t.tpriority  = PRIORITY_CONVERSION(thread_def->tpriority);
    thread_def_t.stacksize  = thread_def->stack_size;
    return osThreadCreate(&thread_def_t, arg);
}

int32_t sys_thread_resume(sys_thread_id_t thread_id)
{
    int32_t ret;

    ret = osThreadResume(thread_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_thread_resume_all(void)
{
    int32_t ret;

    ret = osThreadResumeAll();
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_thread_suspend(sys_thread_id_t thread_id)
{
    int32_t ret;

    ret = osThreadSuspend(thread_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_thread_suspend_all(void)
{
    int32_t ret;

    ret = osThreadSuspendAll();
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_thread_terminate(sys_thread_id_t thread_id)
{
    int32_t ret;

    ret = osThreadTerminate(thread_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

// Message queue
sys_msg_id_t sys_msg_create(const sys_msg_qdef_t *queue_def, sys_thread_id_t thread_id)
{
    osMessageQDef_t queue_def_t;

    queue_def_t.item_sz  = queue_def->item_sz;
    queue_def_t.queue_sz = queue_def->queue_sz;
    return osMessageCreate (&queue_def_t, thread_id);
}

int32_t sys_msg_put(sys_msg_id_t queue_id, uint32_t info, uint32_t millisec)
{
    int32_t ret;

    if (osQueueSpacesAvailable(queue_id) < 1)
    {
        return RETVAL(E_FULL);
    }
    ret = osMessagePut(queue_id, info, millisec);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

sys_event_t sys_msg_get(sys_msg_id_t queue_id, uint32_t millisec)
{
    sys_event_t event_t;
    osEvent event;

    event = osMessageGet(queue_id, millisec);
    event_t.message_id = event.def.message_id;
    event_t.status     = event.status;
    event_t.value.v    = event.value.v;
    return event_t;
}

// Timer
sys_timer_id_t sys_timer_create(const sys_timer_def_t *timer_def, sys_timer_t type, void *arg)
{
    osTimerDef_t timer_def_t;

    timer_def_t.ptimer = timer_def->ptimer;
    return osTimerCreate(&timer_def_t, type, arg);
}

int32_t sys_timer_start(sys_timer_id_t timer_id, uint32_t millisec)
{
    int32_t ret;

    ret = osTimerStart(timer_id, millisec);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_timer_stop(sys_timer_id_t timer_id)
{
    int32_t ret;

    ret = osTimerStop(timer_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_timer_delete(sys_timer_id_t timer_id)
{
    int32_t ret;

    ret = osTimerDelete(timer_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

// Semaphore
sys_sem_id_t sys_sem_create(const sys_sem_qdef_t *sem_def, int32_t count)
{
    osSemaphoreDef_t semaphore_def;

    semaphore_def.dummy = sem_def->dummy;
    return osSemaphoreCreate(&semaphore_def, count);
}

int32_t sys_sem_wait(sys_sem_id_t sem_id)
{
    int32_t ret;

    ret = osSemaphoreWait(sem_id, 0);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_sem_post(sys_sem_id_t sem_id)
{
    int32_t ret;

    ret = osSemaphoreRelease(sem_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_sem_delete(sys_sem_id_t sem_id)
{
    int32_t ret;

    ret = osSemaphoreDelete(sem_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}


sys_mutex_id_t sys_mutex_create(const sys_mutex_def_t *mutex_def)
{
    osMutexDef_t mutex_def_t;

    mutex_def_t.dummy = mutex_def->dummy;
    return osMutexCreate(&mutex_def_t);
}

int32_t sys_mutex_lock(sys_mutex_id_t mutex_id, uint32_t timeout)
{
    int32_t ret;

    ret = osMutexWait(mutex_id, timeout);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_mutex_unlock(sys_mutex_id_t mutex_id)
{
    int32_t ret;

    ret = osMutexRelease(mutex_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}

int32_t sys_mutex_delete(sys_mutex_id_t mutex_id)
{
    int32_t ret;

    ret = osMutexDelete(mutex_id);
    if (ret != RETVAL(E_OK))
    {
        ret = RETVAL(E_FAIL);
    }
    return ret;
}
