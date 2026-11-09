/* Shared bay state: every read or write happens while holding bay_mutex */
#include <string.h>
#include <stdio.h>
#include "logger.h" // for log_event
#include "bays.h"
#include "FreeRTOS.h"
#include "semphr.h"

static Bay bays[BAYS_COUNT]; 
static unsigned long next_id = 1000;
static SemaphoreHandle_t bay_mutex;

void bays_init(void)
{
    bay_mutex = xSemaphoreCreateMutex();
    configASSERT(bay_mutex != NULL); 
}

int bays_occupy_free(const char *vehicle)
{
    int bay = NO_BAY;
    xSemaphoreTake(bay_mutex, portMAX_DELAY);
    for (int i = 0; i < BAYS_COUNT; i++ ) {
        if (!bays[i].is_occupied) {
            bays[i].is_occupied = true;
            snprintf(bays[i].vehicle, sizeof bays[i].vehicle, "%s", vehicle);
            bay = i + 1;
            break;
        }
    }
    xSemaphoreGive(bay_mutex);
    return bay;
}

int bays_release(int bay, char *vehicle_out, size_t n)
{
    int was_taken = 0;
    if (bay < 1 || bay > BAYS_COUNT) return 0;
    xSemaphoreTake(bay_mutex, portMAX_DELAY);
    if (bays[bay - 1].is_occupied) {
        was_taken = 1;
        snprintf(vehicle_out, n, "%s", bays[bay - 1].vehicle);
        bays[bay - 1].is_occupied = false;
        bays[bay - 1].vehicle[0] = '\0';
    }
    xSemaphoreGive(bay_mutex);
    return was_taken;
}

int bays_snapshot(Bay out[BAYS_COUNT]) 
{
    int free_count = 0;
    xSemaphoreTake(bay_mutex, portMAX_DELAY);
    memcpy(out, bays, sizeof bays);
    xSemaphoreGive(bay_mutex);

    for (int i = 0; i < BAYS_COUNT; i++) {
        if (out[i].is_occupied == false) free_count++;
    }
    return free_count;
}