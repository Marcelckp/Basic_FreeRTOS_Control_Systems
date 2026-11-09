/* Vibration sensor for industrial facility Question 2.2 */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    GREEN,
    YELLOW,
    RED
} LightColour;

#define TRANSITION_DELAY 2000

static void transition_traffic(LightColour *direction);
static void traffic_mobilisation_task(void *arg);
void traffic_controller_init(void);