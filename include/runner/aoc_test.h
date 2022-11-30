#ifndef __AOC_TEST_H__
#define __AOC_TEST_H__

#include "constants.h"

struct aoc_test
{
    int day;
    int part;
    char filename[MAX_FILENAME_LENGTH+1];
	char expected_result[MAX_RESULT_LENGTH+1];
    int num_extra_args;
    char extra_args[MAX_NUM_EXTRA_ARGS][MAX_EXTRA_ARG_LENGTH+1];
};

typedef struct aoc_test aoc_test_t;

void aoc_test_init(aoc_test_t * test);
int aoc_test_matches(aoc_test_t * test, int day, int part);
int aoc_test_compare_result(aoc_test_t * test, char * actual_result);

#endif
