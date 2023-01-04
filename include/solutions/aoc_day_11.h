#ifndef __AOC_DAY_11__
#define __AOC_DAY_11__

#include "constants.h"

#define DAY11_MAX_MONKEYS 8
#define DAY11_MAX_ITEMS 64

#define DAY11_OP_OLD_SQUARED 101
#define DAY11_OP_OLD_MULTIPLY 102
#define DAY11_OP_OLD_ADD 103


struct day_11_monkey
{
    long items[DAY11_MAX_ITEMS];
    int num_items;
    int operation_type;
    long operation_constant;
    long test_modulus;
    int true_destination;
    int false_destination;
    long inspection_count;
    int index_number;
};

typedef struct day_11_monkey day_11_monkey_t;

struct day_11_monkeys
{
    struct day_11_monkey monkeys[DAY11_MAX_MONKEYS];
    int num_monkeys;
    long product_of_mods;
};

typedef struct day_11_monkeys day_11_monkeys_t;

void day_11_part_1(char * forestname, extra_args_t * extra_args, char * result);
void day_11_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
