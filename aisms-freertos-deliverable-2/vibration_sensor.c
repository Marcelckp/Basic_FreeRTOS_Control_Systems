/* Vibration sensor for industrial facility Question 2.1 implementation */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "logger.h"
#include "monitor.h"
#include "vibration_sensor.h"
#include "randHelper.h"

static float VIBRATION_G = 0;
static SemaphoreHandle_t vibration_mutex;

float get_vibration_sensor_data(void)
{
    float value;
    xSemaphoreTake(vibration_mutex, portMAX_DELAY);
    value = VIBRATION_G;
    xSemaphoreGive(vibration_mutex);
    return value;
}

static void update_vibration_sensor_values_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(vibration_mutex, portMAX_DELAY);
        VIBRATION_G = random_percent();
        xSemaphoreGive(vibration_mutex);
        monitor_notify_update();
        vTaskDelay(pdMS_TO_TICKS(VIBRATION_UPDATE_INTERVAL_MS));
    }
}

void vibration_sensor_init(void)
{
    vibration_mutex = xSemaphoreCreateMutex();
    configASSERT(vibration_mutex != NULL);
    BaseType_t result = xTaskCreate(update_vibration_sensor_values_task, "monitor_vibration", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    configASSERT(result != NULL);
}