#include <stdio.h>
#include <string.h>

#include "aoc_solutions.h"
// includes for each day will go here
#include "aoc_day_1.h"
#include "aoc_day_2.h"
#include "aoc_day_3.h"
#include "aoc_day_4.h"
#include "aoc_day_5.h"
#include "aoc_day_6.h"
#include "aoc_day_7.h"
#include "aoc_day_8.h"
#include "aoc_day_9.h"
#include "aoc_day_10.h"
#include "aoc_day_11.h"
#include "aoc_day_12.h"
#include "aoc_day_13.h"
#include "aoc_day_14.h"
#include "aoc_day_15.h"
#include "aoc_day_16.h"
#include "aoc_day_17.h"
#include "aoc_day_18.h"
#include "aoc_day_19.h"
#include "aoc_day_20.h"
#include "aoc_day_21.h"
#include "aoc_day_22.h"

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
    solutions->solutions[DAY_INDEX(2)][PART_INDEX(1)] = day_2_part_1;
    solutions->solutions[DAY_INDEX(2)][PART_INDEX(2)] = day_2_part_2;
    solutions->solutions[DAY_INDEX(3)][PART_INDEX(1)] = day_3_part_1;
    solutions->solutions[DAY_INDEX(3)][PART_INDEX(2)] = day_3_part_2;
    solutions->solutions[DAY_INDEX(4)][PART_INDEX(1)] = day_4_part_1;
    solutions->solutions[DAY_INDEX(4)][PART_INDEX(2)] = day_4_part_2;
    solutions->solutions[DAY_INDEX(5)][PART_INDEX(1)] = day_5_part_1;
    solutions->solutions[DAY_INDEX(5)][PART_INDEX(2)] = day_5_part_2;
    solutions->solutions[DAY_INDEX(6)][PART_INDEX(1)] = day_6_part_1;
    solutions->solutions[DAY_INDEX(6)][PART_INDEX(2)] = day_6_part_2;
    solutions->solutions[DAY_INDEX(7)][PART_INDEX(1)] = day_7_part_1;
    solutions->solutions[DAY_INDEX(7)][PART_INDEX(2)] = day_7_part_2;
    solutions->solutions[DAY_INDEX(8)][PART_INDEX(1)] = day_8_part_1;
    solutions->solutions[DAY_INDEX(8)][PART_INDEX(2)] = day_8_part_2;
    solutions->solutions[DAY_INDEX(9)][PART_INDEX(1)] = day_9_part_1;
    solutions->solutions[DAY_INDEX(9)][PART_INDEX(2)] = day_9_part_2;
    solutions->solutions[DAY_INDEX(10)][PART_INDEX(1)] = day_10_part_1;
    solutions->solutions[DAY_INDEX(10)][PART_INDEX(2)] = day_10_part_2;
    solutions->solutions[DAY_INDEX(11)][PART_INDEX(1)] = day_11_part_1;
    solutions->solutions[DAY_INDEX(11)][PART_INDEX(2)] = day_11_part_2;
    solutions->solutions[DAY_INDEX(12)][PART_INDEX(1)] = day_12_part_1;
    solutions->solutions[DAY_INDEX(12)][PART_INDEX(2)] = day_12_part_2;
    solutions->solutions[DAY_INDEX(13)][PART_INDEX(1)] = day_13_part_1;
    solutions->solutions[DAY_INDEX(13)][PART_INDEX(2)] = day_13_part_2;
    solutions->solutions[DAY_INDEX(14)][PART_INDEX(1)] = day_14_part_1;
    solutions->solutions[DAY_INDEX(14)][PART_INDEX(2)] = day_14_part_2;
    solutions->solutions[DAY_INDEX(15)][PART_INDEX(1)] = day_15_part_1;
    solutions->solutions[DAY_INDEX(15)][PART_INDEX(2)] = day_15_part_2;
    solutions->solutions[DAY_INDEX(16)][PART_INDEX(1)] = day_16_part_1;
    solutions->solutions[DAY_INDEX(16)][PART_INDEX(2)] = day_16_part_2;
    solutions->solutions[DAY_INDEX(17)][PART_INDEX(1)] = day_17_part_1;
    solutions->solutions[DAY_INDEX(17)][PART_INDEX(2)] = day_17_part_2;
    solutions->solutions[DAY_INDEX(18)][PART_INDEX(1)] = day_18_part_1;
    solutions->solutions[DAY_INDEX(18)][PART_INDEX(2)] = day_18_part_2;
    solutions->solutions[DAY_INDEX(19)][PART_INDEX(1)] = day_19_part_1;
    solutions->solutions[DAY_INDEX(19)][PART_INDEX(2)] = day_19_part_2;
    solutions->solutions[DAY_INDEX(20)][PART_INDEX(1)] = day_20_part_1;
    solutions->solutions[DAY_INDEX(20)][PART_INDEX(2)] = day_20_part_2;
    solutions->solutions[DAY_INDEX(21)][PART_INDEX(1)] = day_21_part_1;
    solutions->solutions[DAY_INDEX(21)][PART_INDEX(2)] = day_21_part_2;
    solutions->solutions[DAY_INDEX(22)][PART_INDEX(1)] = day_22_part_1;
    solutions->solutions[DAY_INDEX(22)][PART_INDEX(2)] = day_22_part_2;
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

