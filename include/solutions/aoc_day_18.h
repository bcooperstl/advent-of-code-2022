#ifndef __AOC_DAY_18__
#define __AOC_DAY_18__

#include <stdint.h>

#include "constants.h"

#define DAY_18_SIZE 22
#define DAY_18_OFFSET 1
#define DAY_18_ALLOC (DAY_18_SIZE + (2 * DAY_18_OFFSET))

struct day_18_cubes
{
    char world[DAY_18_ALLOC][DAY_18_ALLOC][DAY_18_ALLOC];
};

typedef struct day_18_cubes day_18_cubes_t;

void day_18_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_18_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
