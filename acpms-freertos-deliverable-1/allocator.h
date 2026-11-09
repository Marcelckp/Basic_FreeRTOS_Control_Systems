#pragma once
#include "FreeRTOS.h"
#include "vehicle.h"

void allocator_init(void); // Create the arrival queue and the allocator task

void allocator_kick(void); // Wake up the allocator after freeing a bay

// pdPASS and pdFAIL are FreeRTOS constants for checking in an operation successed
BaseType_t allocator_submit(const Vehicle *v); // pdPASS if queued, pdFAIL if queue is full

UBaseType_t allocator_ahead(const char *plate); // Vehicles in the waiting line returned as a number of items waiting

UBaseType_t allocator_waiting(void); // Vehicles in the waiting line returned as a number of items waiting