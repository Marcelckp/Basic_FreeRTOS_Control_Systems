#pragma once
#include "FreeRTOS.h"

#define LOGGING_INTERVAL_MS 1000

typedef enum {
    SENSOR,
    LOG
} LogType;

typedef struct
{
    TickType_t tick; // timestamp as set by the log sender (i.e. the tick count when the event occurred)
    LogType type;
    char event[512]; // event that occurred (e.g. "SIMULATION_COMPLETE")
} LogMessage;

void logger_init(void); // create the log buffer and the logger task
void log_event(LogType type, const char *event); // log an event (event)