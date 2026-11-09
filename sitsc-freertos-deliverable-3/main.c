/* main.c - ACPMS entry point and FreeRTOS application hooks */
#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "logger.h"
#include "traffic_controller.h"

int main( void )
{
    setvbuf(stdout, NULL, _IOLBF, 0); // line buffered output to print the log messages in a single line        
    printf( "Smart Intersection Traffic Signal Controller (SITSC) - FreeRTOS %s\n", tskKERNEL_VERSION_NUMBER );

    logger_init();                          // Priority 6
    traffic_controller_init();              // Initialize mutex groups

    vTaskStartScheduler();

    /* Reached only if scheduler startup failed. */
    printf("FreeRTOS scheduler failed to start\n");
    fflush(stdout);
    return 0;
}