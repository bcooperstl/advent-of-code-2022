#ifndef __AOC_DAY_0__
#define __AOC_DAY_0__

#include "constants.h"

#define MAX_DAY0_VALUES 2048

struct day0_values
{
    long values[2048];
    int num_values;
};

typedef struct day0_values day0_values_t;

int read_input(char * filename, day0_values_t * values);

void day0_part1(char * filename, extra_args_t * extra_args, char * result);
void day0_part2(char * filename, extra_args_t * extra_args, char * result);

#endif
