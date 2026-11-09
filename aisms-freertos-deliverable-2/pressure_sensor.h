/* Pressure sensor for industrial facility Question 2.1 */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "monitor.h"

#define PRESSURE_UPDATE_INTERVAL_MS 5000

float get_pressure_sensor_data(void);
static void update_pressure_sensor_values_task(void *arg);
void pressure_sensor_init(void);