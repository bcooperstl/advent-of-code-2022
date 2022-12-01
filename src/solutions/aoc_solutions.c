#include <stdio.h>
#include <string.h>

#include "aoc_solutions.h"
// includes for each day will go here
#include "aoc_day_1.h"

#define DAY_INDEX(day) (day -1)
#define PART_INDEX(part) (part-1)

int valid_day_part(int day, int part)
{
    if (day < 1 || day > NUM_DAYS)
    {
        return FALSE;
    }
    if (part < 1 || part > NUM_PARTS)
    {
        return FALSE;
    }
    return TRUE;
}

void initialize_solutions(aoc_solutions_t * solutions)
{
    memset(solutions, '\0', sizeof(aoc_solutions_t));
    // assign solution functions here. indexes are day, part
    solutions->solutions[DAY_INDEX(1)][PART_INDEX(1)] = day_1_part_1;
    solutions->solutions[DAY_INDEX(1)][PART_INDEX(2)] = day_1_part_2;
}

int run_solution(aoc_solutions_t * solutions, int day, int part, char * filename, extra_args_t * extra_args, char * result)
{
    if (valid_day_part(day, part) == FALSE)
    {
        fprintf(stderr, "Invalid day %d (valid ranges %d-%d) or part %d (valid ranges %d-%d)\n", day, 1, NUM_DAYS, part, 1, NUM_PARTS);
        return SOLUTION_INVALID_DAY_PART;
    }
    if (solutions->solutions[DAY_INDEX(day)][PART_INDEX(part)] == NULL)
    {
        fprintf(stderr, "Solution for day %d part %d is not inplemented\n", day, part);
        return SOLUTION_NOT_IMPLEMENTED;
    }
    (*solutions->solutions[DAY_INDEX(day)][PART_INDEX(part)]) (filename, extra_args, result);
    return SOLUTION_RUN_OK;
}

