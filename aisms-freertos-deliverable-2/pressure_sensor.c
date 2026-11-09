/* Pressure sensor for industrial facility Question 2.2 implementation */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "logger.h"
#include "monitor.h"
#include "pressure_sensor.h"
#include "randHelper.h"

static float PRESSURE_KPA = 0;
static SemaphoreHandle_t pressure_mutex;

float get_pressure_sensor_data(void)
{
    float value;
    // portMAX_DELAY is the timeout this semaphore lock take will be active for if task is not complete.
    // In the necessary time it will not continue to block as it will be causing other threads/tasks waiting
    // On the lock to continue to pause and remain frozen indefinitely. portMAX_DELAY means that this will wait
    // indefinitely for the lock
    xSemaphoreTake(pressure_mutex, portMAX_DELAY);
    value = PRESSURE_KPA;
    xSemaphoreGive(pressure_mutex);
    return value;
}

static void update_pressure_sensor_values_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(pressure_mutex, portMAX_DELAY);
        PRESSURE_KPA = random_percent();
        xSemaphoreGive(pressure_mutex);
        monitor_notify_update();
        vTaskDelay(pdMS_TO_TICKS(PRESSURE_UPDATE_INTERVAL_MS));
    }
}

void pressure_sensor_init(void)
{
    pressure_mutex = xSemaphoreCreateMutex();
    configASSERT(pressure_mutex != NULL);
    BaseType_t result = xTaskCreate(update_pressure_sensor_values_task, "monitor_pressure", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    configASSERT(result == pdPASS);
}