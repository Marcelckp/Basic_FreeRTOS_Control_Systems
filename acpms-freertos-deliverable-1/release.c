/* Automatic release fires when release stay_ms time has been reached, the exit task frees the bays for new cars to enter */
#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "bays.h"
#include "release.h"
#include "allocator.h"

static TimerHandle_t bay_timer[BAYS_COUNT];
static TickType_t parked_at[BAYS_COUNT];
static QueueHandle_t exit_queue; // Bay number whose time is up and will be vacated

// Function called when the timer expires
static void bay_timer_cb(TimerHandle_t t)
{
    int bay = (int)(intptr_t)pvTimerGetTimerID(t); // Each timer stores it's own bay number
    xQueueSend(exit_queue, &bay, 0);
}

// Called by the allocator right after assiging a bay. Bay will be the index of the currently assigned bay that needs its stay scheduled.
void release_schedule(int bay, uint32_t stay_ms)
{
    parked_at[bay - 1] = xTaskGetTickCount();
    // Assigns a period to the time and starts it if it was inactive. The timer will then expire after pdMS_TO_TICKS(stay_ms).
    xTimerChangePeriod(bay_timer[bay - 1], pdMS_TO_TICKS(stay_ms), portMAX_DELAY);
}


static void exit_task(void *arg)
{
    (void)arg;
    for(;;) {
        int bay;
        xQueueReceive(exit_queue, &bay, portMAX_DELAY);
        
        char plate[12];
        if (!bays_release(bay, plate, sizeof plate)) continue; // Bay we already free so returned 0

        unsigned long ms = (unsigned long)((xTaskGetTickCount() - parked_at[bay - 1]) * portTICK_PERIOD_MS);

        char action[64];
        snprintf(action, sizeof action, "vehicle exited after %lu ms, bay freed %u", ms, bay);

        log_event("RELEASED", plate, bay, action);
        allocator_kick(); // Since a bay is being freed the allocator can tend to any cars waiting to be awarded entry
    }
}

void release_init(void)
{
    exit_queue = xQueueCreate(BAYS_COUNT, sizeof(int));
    configASSERT(exit_queue != NULL);
    for (int i = 0; i < BAYS_COUNT; i++) {
        // Creates the timers, and for each bay. These timers are inactive until given a timer period in the release_schedule function.
        bay_timer[i] = xTimerCreate("bay", 1, pdFALSE, (void *)(intptr_t)(i + 1), bay_timer_cb);
        configASSERT(bay_timer[i] != NULL);
    }
    xTaskCreate(exit_task, "exit", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
