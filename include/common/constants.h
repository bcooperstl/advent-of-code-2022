#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// Test file format is a CSV file, using a single quote as the quote character and a # as the comment character
#define TEST_INDEX_DELIM ','
#define TEST_INDEX_QUOTE '\''
#define TEST_INDEX_COMMENT '#'

#define MAX_FILENAME_LENGTH 128
#define MAX_RESULT_LENGTH 32
#define MAX_NUM_EXTRA_ARGS 4
#define MAX_EXTRA_ARG_LENGTH 32

#define TEST_SUMMARY_LENGTH 1024

#define NUM_PARTS 2
#define NUM_DAYS 25

#define TRUE 1
#define FALSE 0

#define SOLUTION_RUN_OK 0
#define SOLUTION_NOT_IMPLEMENTED 1
#define SOLUTION_INVALID_DAY_PART 2

#endif
