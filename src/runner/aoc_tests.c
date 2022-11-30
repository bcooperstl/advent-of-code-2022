#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc_tests.h"
#include "aoc_test.h"
#include "constants.h"
#include "file_utils.h"

void aoc_tests_init(aoc_tests_t * tests)
{
    tests->num_tests = 0;
}

void aoc_tests_insert_test(aoc_tests_t * tests, aoc_test_t test)
{
    if (tests->num_tests + 1 >= MAX_TESTS)
    {
        fprintf(stderr, "***ERROR*** Exceeded maximum number of %d tests; Increase limit!\n", MAX_TESTS);
        return;
    }
    memcpy(&tests->tests[tests->num_tests], &test, sizeof(aoc_test_t));
    printf("copied %s to %s\n", test.extra_args[0], tests->tests[tests->num_tests].extra_args[0]);
    tests->num_tests++;
}

void get_base_directory(char * filename, char * base_directory)
{
#ifdef DEBUG_RUNNER
    printf("calculating base direcotry of [%s]\n", filename);
#endif
    char * found[2];
    found[0] = strrchr(filename, '/');
    found[1] = strrchr(filename, '\\');
    char * last_found = NULL;
    
    if (found[0] == NULL && found[1] == NULL) // no base directory; use an empty string
    {
        base_directory[0] = '\0';
#ifdef DEBUG_RUNNER
        printf("  no slashes, so base directory is empty\n");
#endif
        return;
    }
    
    if (found[0] != NULL)
    {
        last_found = found[0];
    }
    if (found[1] != NULL)
    {
        if (last_found == NULL)
        {
            last_found = found[1];
        }
        else
        {
            if (found[1] > found[0])
            {
                last_found = found[1];
            }
        }
    }
    
    strncpy(base_directory, filename, last_found - filename + 1);
    base_directory[last_found-filename+1] = '\0';
#ifdef DEBUG_RUNNER
    printf("  extracted base_directory of [%s]\n", base_directory);
#endif
    return;
}

int aoc_tests_load_from_file(aoc_tests_t * tests, char * filename)
{
    file_data_t fd;
    char basedir[MAX_FILENAME_LENGTH];
    get_base_directory(filename, basedir);
#ifdef DEBUG_RUNNER
    printf("The base dicectory for test files is %s\n", basedir);
#endif
    
    int num_tests = 0;
    
    char delimiters[1];
    delimiters[0]=TEST_INDEX_DELIM;
    
    file_data_init(&fd);
    file_data_read_file(&fd, filename, delimiters, 1, TEST_INDEX_QUOTE, TEST_INDEX_COMMENT);
    
    line_data_t * current = fd.head_line;
    while (current != NULL)
    {
        aoc_test_t test;
        aoc_test_init(&test);
        
        // skip a blank line
        if (current->num_tokens == 0)
        {
            current = current->next;
            continue;
        }
        
        if (current->num_tokens < 4)
        {
            current = current->next;
            fprintf(stderr, "Not enough tokens present for a test case\n");
            continue;
        }
        
        token_data_t * td = current->head_token;
        
        // format is day,part,filename,expected[,...extra args]
        test.day = strtol(td->token, NULL, 10);
        td = td->next;
        test.part = strtol(td->token, NULL, 10);
        td = td->next;
        snprintf(test.filename, sizeof(test.filename), "%s%s", basedir, td->token);
#ifdef DEBUG_RUNNER
        printf("The full path to %s is %s\n", td->token, test.filename);
#endif
        td = td->next;
        strncpy(test.expected_result, td->token, sizeof(test.expected_result));
        td = td->next;
        
        test.num_extra_args = current->num_tokens - 4;
        if (test.num_extra_args  > MAX_NUM_EXTRA_ARGS)
        {
            fprintf(stderr, "Too many extra args (%d) given for test case\n", test.num_extra_args);
            continue;
        }
        
        for (int i=0; i<test.num_extra_args; i++)
        {
            strncpy(test.extra_args[i], td->token, sizeof(test.extra_args[i]));
            td = td->next;
        }
        
#ifdef DEBUG_RUNNER
        printf("Generated test case for day %d part %d file %s with expected result %s\n", test.day, test.part, test.filename, test.expected_result);
        for (int i=0; i<test.num_extra_args; i++)
        {
            printf("Extra argument %d is %s\n", i, test.extra_args[i]);
        }
#endif
        
        aoc_tests_insert_test(tests, test);
        num_tests++;
        
        current = current->next;
    }
    file_data_cleanup(&fd);
    return num_tests;
}

int aoc_tests_filter_tests(aoc_tests_t * all_tests, aoc_tests_t * filtered_tests, int day, int part)
{
    // clear the existing list of tests
    aoc_tests_init(filtered_tests);
    
    int num_tests = 0;
    
    for (int i=0; i<all_tests->num_tests; i++)
    {
        if (all_tests->tests[i].day == day && all_tests->tests[i].part == part)
        {
            aoc_tests_insert_test(filtered_tests, all_tests->tests[i]);
            num_tests++;
        }
    }
    return num_tests;
}
