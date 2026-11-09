#include <stdlib.h>

// Utility handlers
float random_percent(void)
{
    return 100.0f * ((float)rand() / (float)RAND_MAX);
}