/* main.c - ACPMS entry point and FreeRTOS application hooks */
#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"
#include "bays.h"
#include "monitor.h"
#include "allocator.h"
#include "release.h"

/* Number of arrivals - simulated arrivals to the parking lot system to test the system */
static const struct { const char *plate; uint32_t stay_ms } arrivals[] = {
    { "ABC123",     6000 },
    { "DEF456",     3000 },
    { "GHI789",     10000 },
    { "VIP002",     3000 },
    { "JKL012",     5000 },
    { "MNO345",     1000 },
    { "PQR678",     9000 },
    { "VIP007",     9000 },
    { "STU901",     4500 },
    { "TTY909",     6000 },
    { "SIS619",     8000 },
    { "LIF333",     10000 },
    // Overflow in the simulation to map out what happens when all bays are occupied
    // We expect a back up until the cars inside the parking lot exit
    { "AIR902",     2000 },
    { "PLA444",     9000 },
    { "CAR007",     12000 },
    { "VIP001",     3000 }
};

#define N_ARRIVALS (sizeof arrivals / sizeof arrivals[0])

// A example task: Never returns and blocks every 1 second to let the tick count increment by 1
static void sim_task(void *arg)
{
    (void)arg;
    for (size_t i = 0; i < N_ARRIVALS; i++) {
        // Send logging event to the logger
        Vehicle v;
        snprintf(v.plate, sizeof v.plate, "%s", arrivals[i].plate);
        v.arrived = xTaskGetTickCount();
        v.stay_ms = arrivals[i].stay_ms;

        char action[64];
        snprintf(action, sizeof action, "Vehicle arrived at the gate, %u ahead in line", (unsigned)allocator_ahead(v.plate));
        log_event("ENTRY", v.plate, NO_BAY, action);

        allocator_submit(&v); // Always allows new vehicles we just queue them and move on. These cars will wait until there is an open bay available. Logic local to the allocator.

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Send logging event to the logger to indicate that the simulation is complete
    log_event_simple("SIMULATION_COMPLETE");
    vTaskSuspend(NULL); // delete the task as there is nothing left to do
}

int main( void )
{
    setvbuf(stdout, NULL, _IOLBF, 0); // line buffered output to print the log messages in a single line        
    printf( "Autonomous Campus Parking Management System (ACPMS) - FreeRTOS %s\n", tskKERNEL_VERSION_NUMBER );
    fflush( stdout );

    // name, stack size, argument, priority, task handle
    bays_init();                        // Mutex must exist before nay task uses the bays
    logger_init();                      // Priority 1
    monitor_init();                     // Priority 3
    allocator_init();                   // Priority 4
    release_init();                     // Priority 5
    xTaskCreate(sim_task, "simulation", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    vTaskStartScheduler(); /* returns after the logger calls vTaskEndScheduler() */
    return 0;
}