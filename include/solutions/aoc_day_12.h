#ifndef __AOC_DAY_12__
#define __AOC_DAY_12__

#include "constants.h"

#define DAY_12_MAX_ROWS 41
#define DAY_12_MAX_COLS 80

struct day_12_node
{
    char elevation;
    int steps_to_here;
    int evaluated;
};

typedef struct day_12_node day_12_node_5;

struct day_12_map
{
    struct day_12_node nodes[DAY_12_MAX_ROWS][DAY_12_MAX_COLS];
    int num_rows;
    int num_cols;
};

typedef struct day_12_map day_12_map_t;

void day_12_part_1(char * forestname, extra_args_t * extra_args, char * result);
//void day_12_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
