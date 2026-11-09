/* Monitoring parking bay availability: wakes up every 1 second to snapshot the bays and returns the status */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "bays.h"
#include "monitor.h"
#include "allocator.h"

#define MONITOR_PERIOD_MS 1000

static void monitor_task(void *arg)
{
    (void)arg;
    TickType_t last = xTaskGetTickCount();
    for (;;) {
        Bay snap[BAYS_COUNT];
        int free_count = bays_snapshot(snap);

        // e.g. "free=3/10 Bay 1000, 1001, 1002, 1003, ..."
        char status[64];
        int len = snprintf(status, sizeof status, "free=%d/%d", free_count, BAYS_COUNT);
        for (int i = 0; i < BAYS_COUNT && len < (int)sizeof status; i++) {
            len += snprintf(status + len, sizeof status -(size_t)len, " B%02d:%c", i + 1, snap[i].is_occupied ? 'X' : '-');
        }
        char message[512];
        snprintf(message, sizeof message, "PARKING_MONITORING - %s", status);
        log_event_simple(message);
        snprintf(message, sizeof message, "PARKING_MONITORING - %u cars are waiting in line at the gate to accessible the parking lot.", (unsigned)allocator_waiting());
        log_event_simple(message);
        xTaskDelayUntil(&last, pdMS_TO_TICKS(MONITOR_PERIOD_MS));
    }
}

void monitor_init(void)
{
    xTaskCreate(monitor_task, "monitor", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}