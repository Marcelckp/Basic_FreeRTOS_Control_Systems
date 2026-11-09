/* Vibration sensor for industrial facility Question 2.1 implementation */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "logger.h"
#include "traffic_controller.h"

static LightColour NS = GREEN;
static LightColour WE = RED;
static SemaphoreHandle_t traffic_controller_mutex;
static TaskHandle_t traffic_bump = NULL;

static char event[256];

static const char *light_colour_name(LightColour colour)
{
    switch (colour) {
    case GREEN:  return "GREEN";
    case YELLOW: return "YELLOW";
    case RED:    return "RED";
    default:     return "UNKNOWN";
    }
}

void transition_to_red(LightColour *direction)
{
    vTaskDelay(pdMS_TO_TICKS(TRANSITION_DELAY));

    xSemaphoreTake(traffic_controller_mutex, portMAX_DELAY);
    *direction = YELLOW;

    snprintf(event, sizeof event, "NS %s WE %s", 
        light_colour_name(NS), 
        light_colour_name(WE));
    log_event(event);

    vTaskDelay(pdMS_TO_TICKS(TRANSITION_DELAY));

    *direction = RED;

    snprintf(event, sizeof event, "NS %s WE %s", 
        light_colour_name(NS), 
        light_colour_name(WE));
    log_event(event);
    xSemaphoreGive(traffic_controller_mutex);
}

void transition_to_green(LightColour *direction)
{
    vTaskDelay(pdMS_TO_TICKS(TRANSITION_DELAY));

    *direction = GREEN;

    snprintf(event, sizeof event, "NS %s WE %s", 
        light_colour_name(NS), 
        light_colour_name(WE));
    log_event(event);
    xSemaphoreGive(traffic_controller_mutex);
}

static void traffic_mobilisation_task(void *arg)
{
    (void)arg;
    // Orchestrate the changing of light colours to manage traffic
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        transition_to_red(&NS);
        transition_to_green(&WE);
        transition_to_red(&WE); 
        transition_to_green(&NS);
        
        if (traffic_bump != NULL) xTaskNotifyGive(traffic_bump);
    }
}

void traffic_controller_init(void)
{
    traffic_controller_mutex = xSemaphoreCreateMutex();
    configASSERT(traffic_controller_mutex != NULL);
    xTaskCreate(traffic_mobilisation_task, "traffic_mobilisation", configMINIMAL_STACK_SIZE, NULL, 4, &traffic_bump);

    snprintf(event, sizeof event, "NS %s WE %s", 
        light_colour_name(NS), 
        light_colour_name(WE));
    log_event(event);

    xTaskNotifyGive(traffic_bump);
}