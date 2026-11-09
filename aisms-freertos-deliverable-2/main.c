/* main.c - ACPMS entry point and FreeRTOS application hooks */
#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"
#include "monitor.h"
#include "vibration_sensor.h"
#include "pressure_sensor.h"

int main( void )
{
    setvbuf(stdout, NULL, _IOLBF, 0); // line buffered output to print the log messages in a single line        
    printf( "Automated Industrial Sensor Monitoring System (SITSC) - FreeRTOS %s\n", tskKERNEL_VERSION_NUMBER );

    logger_init();                          // Priority 6
    monitor_init();                         // Priority 3
    vibration_sensor_init();                // Priority 4
    pressure_sensor_init();                 // Priority 4
    persistent_logging_init();              // Priority 4

    vTaskStartScheduler();

    /* Reached only if scheduler startup failed. */
    printf("FreeRTOS scheduler failed to start\n");
    fflush(stdout);
    return 0;
}