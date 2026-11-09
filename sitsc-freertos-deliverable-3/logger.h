#pragma once
#include "FreeRTOS.h"

typedef struct
{
    TickType_t tick; // timestamp as set by the log sender (i.e. the tick count when the event occurred)
    char event[512]; // event that occurred (e.g. "SIMULATION_COMPLETE")
} LogMessage;

void log_event(const char *event); // log an event (event)
void logger_init(void); // create the log buffer and the logger task