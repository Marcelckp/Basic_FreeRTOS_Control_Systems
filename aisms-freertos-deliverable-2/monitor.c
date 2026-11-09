/* Monitoring parking bay availability: wakes up every 1 second to snapshot the bays and returns the status */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "monitor.h"
#include "logger.h"
#include "vibration_sensor.h"
#include "pressure_sensor.h"

#define MONITOR_PERIOD_MS 1000

static TaskHandle_t monitor_task_handle = NULL;

// Sensor console output for real-time sensor data
static void monitor_task(void *arg)
{
    (void)arg;

    for (;;) {
        // Sleep indefinitely until notified
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        float pressure = get_pressure_sensor_data();
        float vibration = get_vibration_sensor_data();

        char event[256];
        snprintf(event, sizeof event, "Pressure: %.2f kPa | Vibration: %.2f g", pressure, vibration);
        log_event(SENSOR, event);
    }
}

void monitor_init(void)
{
    xTaskCreate(monitor_task, "monitor", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, &monitor_task_handle);
}

void monitor_notify_update(void)
{
    configASSERT(monitor_task_handle != NULL);
    xTaskNotifyGive(monitor_task_handle);
}