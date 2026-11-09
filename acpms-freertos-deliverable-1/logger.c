/* Question 1.5 Implementation of the logger.h interface - Event logging every tasks sends LogMsg to one buffer/queue which prints the events to the console */
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "logger.h"

#define LOG_BUFFER_SIZE 16

static QueueHandle_t log_queue;

void log_event(const char *event, const char *vehicle, const int bay, const char *action)
{
    LogMessage m = {0};
    m.kind = LOG_EVENT;
    m.body.event.tick = xTaskGetTickCount();
    m.body.event.bay = bay;
    snprintf(m.body.event.vehicle, sizeof(m.body.event.vehicle), "%s", vehicle);
    snprintf(m.body.event.action,  sizeof(m.body.event.action),  "%s", action);
    snprintf(m.body.event.event,   sizeof(m.body.event.event),   "%s", event);
    xQueueSend(log_queue, &m, portMAX_DELAY); // block until the message is sent
}

void log_event_simple(const char *event)
{
    LogMessage m = {0};
    m.kind = LOG_EVENT_SIMPLE;
    m.body.eventSimple.tick = xTaskGetTickCount();
    snprintf(m.body.eventSimple.event, sizeof(m.body.eventSimple.event), "%s", event);
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
        switch (m.kind) {
        case LOG_EVENT:
            tick = m.body.event.tick;
            event = m.body.event.event;
            break;
        case LOG_EVENT_SIMPLE:
            tick = m.body.eventSimple.tick;
            event = m.body.eventSimple.event;
            break;
        }

        unsigned long ms = (unsigned long)(tick * portTICK_PERIOD_MS);
        int hours = ms / 3600000;
        int minutes = (ms % 3600000) / 60000;
        int seconds = (ms % 60000) / 1000;
        int milliseconds = ms % 1000;
        printf("[%02d:%02d:%02d:%03d] %-9s", hours, minutes, seconds, milliseconds, event);

        switch (m.kind) {
        case LOG_EVENT:
            printf(" vehicle=%-8s bay=%3d action=%s\n",
                m.body.event.vehicle, m.body.event.bay, m.body.event.action);
            break;
        case LOG_EVENT_SIMPLE:
            printf("\n");
            break;
        }
    }
}

void logger_init(void)
{
    // create the log queue/buffer. every item is one LogMessage, whichever kind it carries
    log_queue = xQueueCreate(LOG_BUFFER_SIZE, sizeof(LogMessage));
    // check if the queue was created successfully else panic
    configASSERT(log_queue != NULL);
    // create the logger task with the lowest priority (configMAX_PRIORITIES - 1)
    xTaskCreate(logger_task, "Logger", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
}
