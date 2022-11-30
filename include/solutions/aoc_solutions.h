#ifndef __AOC_SOLUTIONS_H__
#define __AOC_SOLUTIONS_H__

#include "constants.h"

struct extra_args
{
    char extra_args[MAX_NUM_EXTRA_ARGS][MAX_EXTRA_ARG_LENGTH+1];
    int num_extra_args;
};

typedef struct extra_args extra_args_t;

struct aoc_solutions
{
    void (*solutions[NUM_DAYS][NUM_PARTS])(char * filename, extra_args_t * extra_args, char * result);
};

typedef struct aoc_solutions aoc_solutions_t;

void initialize_solutions(aoc_solutions_t * solutions);
int run_solution(aoc_solutions_t * solutions, int day, int part, char * filename, extra_args_t * extra_args, char * result);

#endif
