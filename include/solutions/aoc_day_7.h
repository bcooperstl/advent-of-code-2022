#ifndef __AOC_DAY_7__
#define __AOC_DAY_7__

#include "constants.h"

#define DAY_7_NAME_LEN  16

struct day_7_directory;

struct day_7_file
{
    char filename[DAY_7_NAME_LEN+1];
    long size;
    struct day_7_directory * parent;
};

typedef struct day_7_file day_7_file_t;

struct day_7_directory
{
    char dirname[DAY_7_NAME_LEN+1];
    long total_size;
    struct day_7_file * files;
    int total_files;
    int used_files;
    struct day_7_directory * dirs;
    int total_dirs;
    int used_dirs;
    struct day_7_directory * parent;
};

typedef struct day_7_directory day_7_directory_t;

void day_7_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_7_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
