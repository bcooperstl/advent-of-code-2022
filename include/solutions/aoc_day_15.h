#ifndef __AOC_DAY_15__
#define __AOC_DAY_15__

#include "constants.h"

#define DAY_15_MAX_SENSORS 32
#define DAY_15_MAX_SENSOR_GROUPS 64

struct day_15_sensor
{
    long sensor_x;
    long sensor_y;
    long beacon_x;
    long beacon_y;
    long distance_apart;
    int covers_target_row;
    long min_col_in_taget_row_covered;
    long max_col_in_taget_row_covered;
    long min_row_covered;
    long max_row_covered;
};

typedef struct day_15_sensor day_15_sensor_t;

struct day_15_sensors
{
    struct day_15_sensor sensors[DAY_15_MAX_SENSORS];
    int num_sensors;
};

typedef struct day_15_sensors day_15_sensors_t;

struct day_15_sensor_group
{
    long min_row;
    long max_row;
    struct day_15_sensor * sensors[DAY_15_MAX_SENSORS];
    int num_sensors;
};

typedef struct day_15_sensor_group day_15_sensor_group_t;

struct day_15_sensor_groups
{
    struct day_15_sensor_group sensor_groups[DAY_15_MAX_SENSOR_GROUPS];
    int num_sensor_groups;
};

typedef struct day_15_sensor_groups day_15_sensor_groups_t;

void day_15_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_15_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
