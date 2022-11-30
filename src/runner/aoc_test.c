#include <string.h>

#include "aoc_test.h"
#include "constants.h"

void aoc_test_init(aoc_test_t * test)
{
    test->day = 0;
    test->part = 0;
    test->filename[0] = '\0';
    test->expected_result[0] = '\0';
    test->num_extra_args = 0;
    for (int i=0; i<MAX_NUM_EXTRA_ARGS; i++)
    {
        test->extra_args[i][0] = '\0';
    }
}

int aoc_test_matches(aoc_test_t * test, int day, int part)
{
    return (test->day == day) && (test->part == part) ? TRUE : FALSE;
}

int aoc_test_compare_result(aoc_test_t * test, char * actual_result)
{
    if (strncmp(test->expected_result, actual_result, MAX_RESULT_LENGTH) == 0)
    {
        return TRUE;
    }
    return FALSE;
}
