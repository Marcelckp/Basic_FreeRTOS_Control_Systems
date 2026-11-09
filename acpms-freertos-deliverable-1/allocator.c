/* Parking bay allocator.*/
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bays.h"
#include "allocator.h"
#include "release.h"

#define QUEUE_LEN   16 // Waiting line for parking lot
#define RETRY_MS            200 // How often we recheck bay allocation

static QueueHandle_t vip_queue;
static QueueHandle_t normal_queue;
static TaskHandle_t alloc_handle; // Handler to bump the queue

static int is_vip(const char *plate)
{
    return strncmp(plate, "VIP", 3) == 0;
}

BaseType_t allocator_submit(const Vehicle *v)
{
    BaseType_t ok;
    // Based on if the vehicle has a VIP number plate queue them in the VIP queue
    if (is_vip(v->plate)) {
        log_event("VIP_DETECTED", v->plate, NO_BAY, "VIP permit detected, joined VIP line");
        ok = xQueueSend(vip_queue, v, portMAX_DELAY);
    } else {
        return xQueueSend(normal_queue, v, portMAX_DELAY);
    }
    xTaskNotifyGive(alloc_handle);
    return ok;
}

UBaseType_t allocator_ahead(const char *plate)
{
    UBaseType_t vips = uxQueueMessagesWaiting(vip_queue); 
    return is_vip(plate) ? vips : vips + uxQueueMessagesWaiting(normal_queue);
}

void allocator_kick(void)
{
    xTaskNotifyGive(alloc_handle); // Something change... Potential free bay opening.
}

UBaseType_t allocator_waiting(void)
{
    return uxQueueMessagesWaiting(vip_queue); + uxQueueMessagesWaiting(normal_queue);
}

static void allocator_task(void *arg)
{
    (void)arg;
    char waiting_logged[12] = ""; // plate already logged as WAITING so we log it once

    for (;;) {
        // Sleep until the handler event callback is invoked to notify the listener here to wakup and run for all vehicles that have arrived.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // alloc_handle is the trigger function
        
        // Serve as many waiting cars as there are free bays
        for (;;) {
            Vehicle v;
            QueueHandle_t from;

            if (xQueuePeek(vip_queue, &v, 0) == pdPASS) from = vip_queue;
            else if (xQueuePeek(normal_queue, &v, 0) == pdPASS) from = normal_queue;
            else break;

            int bay = bays_occupy_free(v.plate);
            if (bay == NO_BAY) {
                // Log that the car in the front of the queue is waiting. We wake up on notification event and we can only assign new bays on that event as it will be set when a car arrives or one leaves the parking lot.
                if (strcmp(waiting_logged, v.plate) != 0) {
                    log_event("WAITING", v.plate, NO_BAY, "There are no available parking bays, waiting on assigned vehicles to exit");
                    snprintf(waiting_logged, sizeof waiting_logged, "%s", v.plate);
                }
                break;
            }
            
            // Bay found and secured for vehicle allowing them entry
            xQueueReceive(from, &v, 0);
            
            // Tracks how long the car waited from arriving to be allocated a parking spot
            unsigned long waited = (unsigned long)((xTaskGetTickCount() - v.arrived) * portTICK_PERIOD_MS);
            
            char action[512];
            if (from == vip_queue) 
                snprintf(action, sizeof action, "VIP PRIORITY - Allocated after %lu ms in queue, and ahead of %u normal cars waiting", waited, (unsigned)uxQueueMessagesWaiting(normal_queue));
            else
                snprintf(action, sizeof action, "Allocated after %lu ms in queue", waited);
            log_event("BAY_ASSIGNED", v.plate, bay, action);
            release_schedule(bay, v.stay_ms); // Starting bay parking time.
        }
    }
}

void allocator_init(void)
{
    vip_queue = xQueueCreate(QUEUE_LEN, sizeof(Vehicle));
    normal_queue = xQueueCreate(QUEUE_LEN, sizeof(Vehicle));
    configASSERT(normal_queue != NULL && vip_queue != NULL);
    // alloc_handle is a handler that pokes this task to run again.
    xTaskCreate(allocator_task, "alloc", configMINIMAL_STACK_SIZE, NULL, 4, &alloc_handle);
}