#ifndef __AOC_DAY_21__
#define __AOC_DAY_21__

#include <stdint.h>

#include "constants.h"

#define DAY_21_MAX_MONKEYS 2600

#define DAY_21_MONKEY_IS_VALUE 1
#define DAY_21_MONKEY_IS_OPERATION 2
#define DAY_21_MONKEY_NAME_LENGTH 4

struct day_21_monkey;

struct day_21_monkey
{
    int type;
    int is_evaluated;
    long long value;
    char own_name[DAY_21_MONKEY_NAME_LENGTH+1];
    char operation;
    char operand_names[2][DAY_21_MONKEY_NAME_LENGTH+1];
    int operand_indices[2];
    int is_yell_dependant;
};

typedef struct day_21_monkey day_21_monkey_t;

struct day_21_monkeys
{
    day_21_monkey_t monkeys[DAY_21_MAX_MONKEYS];
    int num_monkeys;
};

typedef struct day_21_monkeys day_21_monkeys_t;

void day_21_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_21_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
