/* Question 1.5 Implementation of the logger.h interface - Event logging every tasks sends LogMsg to one buffer/queue which prints the events to the console */
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "logger.h"
#include "vibration_sensor.h"
#include "pressure_sensor.h"

#define LOG_BUFFER_SIZE 16

static QueueHandle_t log_queue;

// We use char * to notate strings because there is no string type in C.
void log_event(LogType type, const char *event)
{
    LogMessage m = {0};
    m.tick = xTaskGetTickCount();
    m.type = type;
    snprintf(m.event, sizeof(m.event), "%s", event);
    xQueueSend(log_queue, &m, portMAX_DELAY); // block until the message is sent
}

static void logger_task(void *arg)
{
    (void)arg;

    // For loop to keep the logger task running until the program exits
    // The task does not return and pulls messages from the queue and prints them to the console
    for (;;) {
        LogMessage m;
        xQueueReceive(log_queue, &m, portMAX_DELAY); // block until a message is received

        TickType_t tick = 0;
        const char *event = "";
        LogType type;

        tick = m.tick;
        event = m.event;
        type = m.type;

        unsigned long ms = (unsigned long)(tick * portTICK_PERIOD_MS);
        int hours = ms / 3600000;
        int minutes = (ms % 3600000) / 60000;
        int seconds = (ms % 60000) / 1000;
        int milliseconds = ms % 1000;
        
        const char *type_name = (type == LOG) ? "LOG" : "SENSOR";

        if (type == LOG) printf("\033[30m [%s] [%02d:%02d:%02d:%03d] %-9s \033[0m", type_name, hours, minutes, seconds, milliseconds, event);
        if (type == SENSOR) printf("\033[32m [%s] %-9s \033[0m", type_name, event);
        
        printf("\n");
    }
}

void logger_init(void)
{
    // create the log queue/buffer. every item is one LogMessage, whichever kind it carries
    log_queue = xQueueCreate(LOG_BUFFER_SIZE, sizeof(LogMessage));
    // check if the queue was created successfully else panic
    configASSERT(log_queue != NULL);
    xTaskCreate(logger_task, "Logger", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
}
