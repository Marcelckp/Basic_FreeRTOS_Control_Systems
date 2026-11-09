#pragma once
#include <stdint.h>

void release_init(void);
void release_schedule(int bay, uint32_t stay_ms);