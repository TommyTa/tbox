/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdio.h>

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
#endif

#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)

#define configUSE_PREEMPTION                    1                       // 1:Preemptive kernel 0:Cooperative kernel
#define configUSE_TIME_SLICING                  1                       // 1:Enable the time slicing.
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1                       // 1:Enable the special method to select next task, if the hardware
                                                                        // did not support the LeadingZeros instruction, disable it.
#define configUSE_TICKLESS_IDLE                 0                       // 1:Enable the tickless low power mode.
#define configUSE_QUEUE_SETS                    1                       // 1:Enable the queue sets.
#define configCPU_CLOCK_HZ                      (SystemCoreClock)       // System frequency
#define configTICK_RATE_HZ                      (1000)                  // System tick
#define configMAX_PRIORITIES                    (32)                    // Max priorities
#define configMINIMAL_STACK_SIZE                ((unsigned short)128)   // Be used for idle task
#define configMAX_TASK_NAME_LEN                 (16)                    // The length of the task name

#define configUSE_16_BIT_TICKS                  0                       // 1:16bit 0:32bit
#define configIDLE_SHOULD_YIELD                 1                       // When idle task is scheduled, yield to the same priority task.
#define configUSE_TASK_NOTIFICATIONS            1                       // 1:Use notification mechanism
#define configUSE_MUTEXES                       1                       // 1:Use mutex
#define configUSE_RECURSIVE_MUTEXES             1                       // 1:Use the recursive mutexes
#define configQUEUE_REGISTRY_SIZE               8                       // If the macro not equal to zero, the max size of REGISTRY.
#define configCHECK_FOR_STACK_OVERFLOW          0                       // If the macro greater than zero, the macro shoule be set to 1 or 2.
#define configUSE_MALLOC_FAILED_HOOK            0                       // 1:If enbale, must set the hook.
#define configUSE_APPLICATION_TASK_TAG          0                       // Task tag
#define configUSE_COUNTING_SEMAPHORES           1                       // 1:Use the counting semaphores.

#define configSUPPORT_DYNAMIC_ALLOCATION        1                       // Support the dynamic allocation
#define configTOTAL_HEAP_SIZE                   ((size_t)(40*1024))     // The total size of the heap.

#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0

#define configGENERATE_RUN_TIME_STATS           0                       // Track the total amount of time that the task has been run.
#define configUSE_TRACE_FACILITY                1                       // Visual tracking
#define configUSE_STATS_FORMATTING_FUNCTIONS    1                       // And configUSE_TRACE_FACILITY is set to 1, the following 3 functions is compiled.
                                                                        // prvWriteNameToBuffer(), vTaskList(), vTaskGetRunTimeStats()

#define configUSE_CO_ROUTINES                   0                       // Enable routines, must add the croutine.c file.
#define configMAX_CO_ROUTINE_PRIORITIES         (2)                     // The number of the valid priority.

#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (3)
#define configTIMER_QUEUE_LENGTH                12
#define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE*20)

#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1

#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS                     __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS                     4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15              // The lowest interrut priority
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    1               // The max syscall interrut priority
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

#define xPortPendSVHandler                      PendSV_Handler
#define vPortSVCHandler                         SVC_Handler

#endif /* FREERTOS_CONFIG_H */
