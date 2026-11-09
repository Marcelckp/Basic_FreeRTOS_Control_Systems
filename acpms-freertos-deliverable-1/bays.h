#pragma once

#include "logger.h" // for log_event
#include "stdbool.h" // for bool
#include <stddef.h> 

#define BAYS_COUNT 10

// Bay structure for a single bay
typedef struct {
    bool is_occupied; // true if the bay is occupied, false if the bay is free
    char vehicle[12]; // the vehicle number plate that is parked in the bay (empty string if the bay is free)
} Bay;

void bays_init(void);
int bays_occupy_free(const char *vehicle); // Take the first free bay: return the bay number or NO_BAY if no free bay is available
int bays_release(int bay, char *vehicle_out, size_t n); // Release the bay and copy the memory of the car that was released in the memory location pushed into vehicle_out.
int bays_snapshot(Bay out[BAYS_COUNT]); // Snapshot the bays: return the number of bays occupied