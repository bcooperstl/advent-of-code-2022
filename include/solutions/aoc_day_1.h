#ifndef __AOC_DAY_1__
#define __AOC_DAY_1__

#include "constants.h"

#define MAX_DAY_1_CALORIES  16
#define MAX_DAY_1_ELVES     256

struct day_1_elf
{
    long calories[MAX_DAY_1_CALORIES];
    int num_calories;
};

typedef struct day_1_elf day_1_elf_t;

struct day_1_elves
{
    day_1_elf_t elves[MAX_DAY_1_ELVES];
    int num_elves;
};

typedef struct day_1_elves day_1_elves_t;

//static int read_and_parse_input(char * filename, day_1_elves_t * elves);

void day_1_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_1_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
