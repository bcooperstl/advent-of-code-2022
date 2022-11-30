#include <string.h>
#include <stdio.h>
#include <unistd.h> // getopt
#include <stdlib.h>

#include "aoc_tests.h"
#include "file_utils.h"
#include "aoc_solutions.h"

#define DASH_D 0x01
#define DASH_P 0x02
#define DASH_F 0x04
#define DASH_T 0x08
#define DASH_R 0x10

#define OPTS_RUN_FILE (DASH_D | DASH_P | DASH_F)
#define OPTS_RUN_TEST (DASH_D | DASH_P | DASH_T)
#define OPTS_RUN_REGRESSION (DASH_R)

void usage(char * prog_name)
{
    fprintf(stderr, "Usage for %s\n", prog_name);
    fprintf(stderr, "   Run one file: %s -d day -p part -f filename [extra_args...]\n", prog_name);
    fprintf(stderr, "   Run one day/part tests: %s -d day -p part -t test_index_filename\n", prog_name);
    fprintf(stderr, "   Run full regression test: %s -r test_index_filename\n", prog_name);
}

int main (int argc, char * argv[])
{
    aoc_tests_t all_tests, filtered_tests;
    aoc_solutions_t solutions;
    
    long day = 0;
    long part = 0;
    int regression = FALSE;
    char filename[MAX_FILENAME_LENGTH+1];
    char test_filename[MAX_FILENAME_LENGTH+1];
    char result[MAX_RESULT_LENGTH+1];
    
    filename[0] = '\0';
    test_filename[0] = '\0';
    result[0] = '\0';

    extra_args_t extra_args;
    extra_args.num_extra_args = 0;
    
    for (int i=0; i<MAX_NUM_EXTRA_ARGS; i++)
    {
        extra_args.extra_args[i][0]='\0';
    }
    
    int given_opts = 0;
    int opt;
    
    // getopt parsing of command line parameters
    while ((opt = getopt(argc, argv, "d:p:f:t:r:")) != -1)
    {
        switch (opt)
        {
            case 'd':
                day = strtol(optarg, NULL, 10);
                given_opts |= DASH_D;
                break;
            case 'p':
                part = strtol(optarg, NULL, 10);
                given_opts |= DASH_P;
                break;
            case 'f':
                if (strlen(optarg) > MAX_FILENAME_LENGTH)
                {
                    fprintf(stderr, "Given filename %s is too long - max length is %d\n", optarg, MAX_FILENAME_LENGTH);
                }
                strncpy(filename, optarg, MAX_FILENAME_LENGTH);
                filename[MAX_FILENAME_LENGTH] = '\0';
                given_opts |= DASH_F;
                break;
            case 't':
                if (strlen(optarg) > MAX_FILENAME_LENGTH)
                {
                    fprintf(stderr, "Given filename %s is too long - max length is %d\n", optarg, MAX_FILENAME_LENGTH);
                }
                strncpy(test_filename, optarg, MAX_FILENAME_LENGTH);
                test_filename[MAX_FILENAME_LENGTH] = '\0';
                given_opts |= DASH_T;
                break;
            case 'r':
                if (strlen(optarg) > MAX_FILENAME_LENGTH)
                {
                    fprintf(stderr, "Given filename %s is too long - max length is %d\n", optarg, MAX_FILENAME_LENGTH);
                }
                strncpy(test_filename, optarg, MAX_FILENAME_LENGTH);
                test_filename[MAX_FILENAME_LENGTH] = '\0';
                regression = TRUE;
                given_opts |= DASH_R;
                break;
            default:
                fprintf(stderr, "Invalid option %c given\n", opt);
                usage(argv[0]);
                exit(8);
                break;
        }
    }
    
    // got through the input checks. initialize the solutions
    initialize_solutions(&solutions);
    
    // check for valid option combination and get any extra arguments if running from a file
    if (given_opts == OPTS_RUN_FILE)
    {
        for (int i=optind; i<argc; i++)
        {
            if (extra_args.num_extra_args >= MAX_NUM_EXTRA_ARGS)
            {
                fprintf(stderr, "More than maximum of %d extra arguments given\n", MAX_EXTRA_ARG_LENGTH);
                break;
            }
            if (strlen(argv[i]) > MAX_EXTRA_ARG_LENGTH)
            {
                fprintf(stderr, "Given extra argument %s is too long - max length is %d\n", optarg, MAX_EXTRA_ARG_LENGTH);
            }
            strncpy(extra_args.extra_args[extra_args.num_extra_args], argv[i], MAX_EXTRA_ARG_LENGTH);
            extra_args.extra_args[extra_args.num_extra_args][MAX_EXTRA_ARG_LENGTH] = '\0';
            extra_args.num_extra_args++;
        }
    }
    else if (given_opts != OPTS_RUN_TEST && given_opts != OPTS_RUN_REGRESSION)
    {
        fprintf(stderr, "Invalid set of options given\n");
        usage(argv[0]);
        exit(8);
    }
    
    // Actually do stuff
    // run one file manually
    if (given_opts == OPTS_RUN_FILE)
    {
        int rc;
        printf("Running day %d part %d for filename %s\n", day, part, filename); 
        rc = run_solution(&solutions, day, part, filename, &extra_args, result);
        if (rc == SOLUTION_RUN_OK)
        {
            printf("***Day %d Part %d for file %s has result %s\n", day, part, filename, result);
        }
    }
    else // no filename; assume some type of testing mode
    {
        aoc_tests_init(&all_tests);
        aoc_tests_t * tests_to_run = &all_tests;
        int num_tests = aoc_tests_load_from_file(&all_tests, test_filename);
        if (num_tests == 0)
        {
            fprintf(stderr, "Error loading from test index file %s\n", test_filename);
            exit(8);
        }
        printf("Loaded %d tests from %s\n", num_tests, test_filename);
        if (regression)
        {
            printf("Running full regression test for all days and parts - %d tests!\n", all_tests.num_tests);
            tests_to_run = &all_tests;
        }
        else
        {
            num_tests = aoc_tests_filter_tests(&all_tests, &filtered_tests, day, part);
            
            printf("Running tests for day %d part %d - %d tests\n", day, part, filtered_tests.num_tests);
            tests_to_run = &filtered_tests;
        }
        
        char ** test_summaries = (char **)malloc(num_tests * sizeof(char *));
        for (int i=0; i<num_tests; i++)
        {
            test_summaries[i] = (char *)malloc(TEST_SUMMARY_LENGTH);
        }
        
        for (int i=0; i<tests_to_run->num_tests; i++)
        {
            int rc;
            printf("Running day %d part %d for filename %s\n", tests_to_run->tests[i].day, tests_to_run->tests[i].part, tests_to_run->tests[i].filename); 
            extra_args.num_extra_args = tests_to_run->tests[i].num_extra_args;
            for (int j=0; j<tests_to_run->tests[i].num_extra_args; j++)
            {
                strncpy(extra_args.extra_args[j], tests_to_run->tests[i].extra_args[j], MAX_EXTRA_ARG_LENGTH+1);
                
            }
            rc = run_solution(&solutions, tests_to_run->tests[i].day, tests_to_run->tests[i].part, tests_to_run->tests[i].filename, &extra_args, result);
            if (rc == SOLUTION_RUN_OK)
            {
                printf("***Day %d Part %d for file %s has result %s\n", tests_to_run->tests[i].day, tests_to_run->tests[i].part, tests_to_run->tests[i].filename, result);
                if (strncmp(result, tests_to_run->tests[i].expected_result, MAX_RESULT_LENGTH) == 0)
                {
                    snprintf(test_summaries[i], TEST_SUMMARY_LENGTH, "++Day %d Part %d-%s passed with result %s",
                             tests_to_run->tests[i].day, tests_to_run->tests[i].part, tests_to_run->tests[i].filename, result);
                }
                else
                {
                    snprintf(test_summaries[i], TEST_SUMMARY_LENGTH, "--Day %d Part %d-%s FAILED expected=%s actual=%s",
                             tests_to_run->tests[i].day, tests_to_run->tests[i].part, tests_to_run->tests[i].filename, tests_to_run->tests[i].expected_result, result);
                }
            }
            else
            {
                    snprintf(test_summaries[i], TEST_SUMMARY_LENGTH, "!!Day %d Part %d-%s DID NOT RUN!!",
                             tests_to_run->tests[i].day, tests_to_run->tests[i].part, tests_to_run->tests[i].filename);
            }
        }
        for (int i=0; i<tests_to_run->num_tests; i++)
        {
            printf("%s\n", test_summaries[i]);
            free(test_summaries[i]);
        }
        free(test_summaries);
    }
    return 0;
}
