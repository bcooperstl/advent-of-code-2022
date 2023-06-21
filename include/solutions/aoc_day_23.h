#ifndef __AOC_DAY_23__
#define __AOC_DAY_23__

#include "constants.h"

#define DAY_23_ELF '#'
#define DAY_23_EMPTY '.'

#define DAY_23_MAX_ELVES 4096

#define DAY_23_DIRECTION_NORTH 0
#define DAY_23_DIRECTION_SOUTH 1
#define DAY_23_DIRECTION_WEST 2
#define DAY_23_DIRECTION_EAST 3
#define DAY_23_DIRECTION_STAY 4

struct day_23_elf
{
    int pos_y;
    int pos_x;
    int elf_neighbors[8];
    int proposed_direction;
    int proposed_x;
    int proposed_y;
    int can_move;
};

typedef struct day_23_elf day_23_elf_t;

struct day_23_elves
{
    day_23_elf_t elves[DAY_23_MAX_ELVES];
    int num_elves;
};

typedef struct day_23_elves day_23_elves_t;

struct day_23_direction_list
{
    int directions[4];
};

typedef struct day_23_direction_list day_23_direction_list_t;

void day_23_part_1(char * forestname, extra_args_t * extra_args, char * result);
void day_23_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
