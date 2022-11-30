#ifndef __AOC_TESTS_H__
#define __AOC_TESTS_H__

#include "aoc_test.h"

#define MAX_TESTS 256

struct aoc_tests
{
    aoc_test_t tests[MAX_TESTS];
    int num_tests;
};

typedef struct aoc_tests aoc_tests_t;

void aoc_tests_init(aoc_tests_t * tests);
void aoc_tests_insert_test(aoc_tests_t * tests, aoc_test_t test);
int aoc_tests_load_from_file(aoc_tests_t * tests, char * filename);
int aoc_tests_filter_tests(aoc_tests_t * all_tests, aoc_tests_t * filtered_tests, int day, int part);

#endif
