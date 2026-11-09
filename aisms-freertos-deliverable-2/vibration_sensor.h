/* Vibration sensor for industrial facility Question 2.2 */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "monitor.h"

#define VIBRATION_UPDATE_INTERVAL_MS 7000

float get_vibration_sensor_data(void);
static void update_vibration_sensor_values_task(void *arg);
void vibration_sensor_init(void);