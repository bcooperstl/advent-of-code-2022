#ifndef __AOC_DAY_15__
#define __AOC_DAY_15__

#include "constants.h"

#define DAY_15_MAX_SENSORS 32

struct day_15_sensor
{
    long sensor_x;
    long sensor_y;
    long beacon_x;
    long beacon_y;
    long distance_apart;
    int covers_target_row;
    long min_target_row_covered;
    long max_target_row_covered;
};

typedef struct day_15_sensor day_15_sensor_t;

struct day_15_sensors
{
    struct day_15_sensor sensors[DAY_15_MAX_SENSORS];
    int num_sensors;
};

typedef struct day_15_sensors day_15_sensors_t;

void day_15_part_1(char * filename, extra_args_t * extra_args, char * result);
//void day_15_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
