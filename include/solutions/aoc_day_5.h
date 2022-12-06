#ifndef __AOC_DAY_5__
#define __AOC_DAY_5__

#include "constants.h"

#define MAX_DAY_5_STACKS 9
#define MAX_DAY_5_STACK_DEPTH 64
#define MAX_DAY_5_MOVES 512

struct day_5_stacks
{
    int num_stacks;
    char crates[MAX_DAY_5_STACKS][MAX_DAY_5_STACK_DEPTH+1];
};

typedef struct day_5_stacks day_5_stacks_t;

struct day_5_move
{
    int count;
    int from;
    int to;
};

typedef struct day_5_move day_5_move_t;

struct day_5_moves
{
    int num_moves;
    day_5_move_t moves[MAX_DAY_5_MOVES];
};

typedef struct day_5_moves day_5_moves_t;

void day_5_part_1(char * filename, extra_args_t * extra_args, char * result);
//void day_5_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
