#ifndef __AOC_DAY_3__
#define __AOC_DAY_3__

#include "constants.h"

#define MAX_DAY_3_COMPARTMENT_LENGTH  32

struct day_3_rucksack
{
    int compartment_length;
    char compartments[2][MAX_DAY_3_COMPARTMENT_LENGTH+1];
};

typedef struct day_3_rucksack day_3_rucksack_t;

//static int read_and_parse_input(char * filename, day_3_elves_t * elves);

void day_3_part_1(char * filename, extra_args_t * extra_args, char * result);
//void day_3_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
