#pragma once
#define configUSE_PREEMPTION                    1 // Preemption is enabled
#define configTICK_RATE_HZ                      1000 // 1000 Hz = 1 ms
#define configMAX_PRIORITIES                    7
#define configUSE_TASK_PRIORITIES               1 // Task priorities are used
#define configMINIMAL_STACK_SIZE                4096 // 4KB is the minimal stack size
#define configTOTAL_HEAP_SIZE                   (64*1024) // 64KB is the total heap size
#define configMAX_TASK_NAME_LEN                 10 // 10 characters is the maximum task name length
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS // 32 bits is the width of the tick type
#define configUSE_IDLE_HOOK                     0 // No idle hook
#define configUSE_TICK_HOOK                     0 // No tick hook
#define configUSE_TIMERS                        1 
#define configUSE_MUTEXES                       1 // Mutexes are used
#define configSUPPORT_DYNAMIC_ALLOCATION        1 // Dynamic allocation is supported
#define INCLUDE_vTaskDelay                      1 // vTaskDelay is included
#define INCLUDE_vTaskDelayUntil                 1 // vTaskDelayUntil is included
#define INCLUDE_vTaskSuspend                    1 // vTaskSuspend is included
#define INCLUDE_vTaskDelete                     1 // vTaskDelete is included
#define INCLUDE_vSemaphoreGive                  1 
#define INCLUDE_vSemaphoreTake                  1
#define INCLUDE_vSemaphoreCreateMutex           1

// Software timers
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE