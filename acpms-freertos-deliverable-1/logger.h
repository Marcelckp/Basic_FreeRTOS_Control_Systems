#pragma once
#include "FreeRTOS.h"

#define NO_BAY (-1) // event that has no bay yet assigned (a vehicle that is waiting to be parked)

typedef enum {
    LOG_EVENT,
    LOG_EVENT_SIMPLE
} LogKind;

typedef struct
{
    TickType_t tick;
    int bay;
    char vehicle[12];
    char action[512];
    char event[512];
} LogEventMessage;

typedef struct
{
    TickType_t tick; // timestamp as set by the log sender (i.e. the tick count when the event occurred)
    char event[512]; // event that occurred (e.g. "SIMULATION_COMPLETE")
} LogEventMessageSimple;

// One queue item. kind selects which union member holds the message.
typedef struct
{
    LogKind kind;
    union {
        LogEventMessage event;
        LogEventMessageSimple eventSimple;
    } body;
} LogMessage;

void logger_init(void); // create the log buffer and the logger task
void log_event(const char *event, const char *vehicle, const int bay, const char *action); // log an entrance/exit event (event, vehicle, bay, action)
void log_event_simple(const char *event); // log an event (event)
