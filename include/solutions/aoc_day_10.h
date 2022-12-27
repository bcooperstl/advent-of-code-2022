#ifndef __AOC_DAY_10__
#define __AOC_DAY_10__

#include "constants.h"

#define DAY_10_TYPE_NOOP 0
#define DAY_10_TYPE_ADDX 1

#define DAY_10_MAX_INSTRUCTIONS 256

struct day_10_instruction
{
    int type;
    int value;
};

typedef struct day_10_instruction day_10_instruction_t;

struct day_10_instructions
{
    struct day_10_instruction instructions[DAY_10_MAX_INSTRUCTIONS];
    int num_instructions;
};

typedef struct day_10_instructions day_10_instructions_t;

struct day_10_key_cycle
{
    int cycle_number;
    int value;
};

typedef struct day_10_key_cycle day_10_key_cycle_t;


void day_10_part_1(char * forestname, extra_args_t * extra_args, char * result);
void day_10_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
