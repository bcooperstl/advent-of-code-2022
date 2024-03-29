#ifndef __AOC_DAY_24__
#define __AOC_DAY_24__

#include "constants.h"

#define DAY_24_MAP_MAX_ROWS 32
#define DAY_24_MAP_MAX_COLS 127

struct day_24_map
{
    char data[DAY_24_MAP_MAX_ROWS][DAY_24_MAP_MAX_COLS+1]; // +1 for null terminator
    int num_rows;
    int num_cols;
};

typedef struct day_24_map day_24_map_t;

struct day_24_map_component_permutations
{
    day_24_map_t blank_map;
    day_24_map_t up_down_permutations[DAY_24_MAP_MAX_ROWS];
    day_24_map_t left_right_permutations[DAY_24_MAP_MAX_COLS];
    int num_up_down_permutations;
    int num_left_right_permutations;
};

typedef struct day_24_map_component_permutations day_24_map_component_permutations_t;

void day_24_part_1(char * forestname, extra_args_t * extra_args, char * result);
void day_24_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
