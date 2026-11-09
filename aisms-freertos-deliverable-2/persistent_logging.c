#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"
#include "monitor.h"
#include "vibration_sensor.h"
#include "pressure_sensor.h"

static void persistent_logging_task(void *arg)
{
    (void)arg;

    for (;;) {
        float pressure = get_pressure_sensor_data();
        float vibration = get_vibration_sensor_data();
        
        char event[256];
        snprintf(event, sizeof event, "P: %f V: %f", pressure, vibration);

        log_event(LOG, event);
        vTaskDelay(pdMS_TO_TICKS(LOGGING_INTERVAL_MS));
    }
}

void persistent_logging_init(void)
{
    xTaskCreate(persistent_logging_task, "persistent_logger", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
}