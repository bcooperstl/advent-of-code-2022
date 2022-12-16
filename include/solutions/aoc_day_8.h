#ifndef __AOC_DAY_8__
#define __AOC_DAY_8__

#include "constants.h"

struct day_8_forest
{
    int rows;
    int cols;
    char ** data;
};

typedef struct day_8_forest day_8_forest_t;

void day_8_part_1(char * forestname, extra_args_t * extra_args, char * result);
//void day_8_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
