#pragma once
#include "FreeRTOS.h"

typedef struct {
    char plate[12]; 
    TickType_t arrived;
    uint32_t stay_ms;
} Vehicle;