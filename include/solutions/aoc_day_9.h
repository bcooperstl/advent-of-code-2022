#ifndef __AOC_DAY_9__
#define __AOC_DAY_9__

#include "constants.h"

#define DAY9_MAX_INSTRUCTIONS 2048

#define NUM_STATES 9
#define NUM_DIRECTIONS 4

struct day_9_move
{
    int head_delta_x;
    int head_delta_y;
    int tail_delta_x;
    int tail_delta_y;
    int next_state;
};

typedef struct day_9_move day_9_move_t;

struct day_9_move_table
{
    day_9_move_t moves[NUM_STATES][NUM_DIRECTIONS];
};

typedef struct day_9_move_table day_9_move_table_t;

struct day_9_instruction
{
    int direction;
    int num_steps;
};

typedef struct day_9_instruction day_9_instruction_t;

struct day_9_instructions
{
    day_9_instruction_t instructions[DAY9_MAX_INSTRUCTIONS];
    int num_instructions;
};

typedef struct day_9_instructions day_9_instructions_t;

void day_9_part_1(char * forestname, extra_args_t * extra_args, char * result);
//void day_9_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
