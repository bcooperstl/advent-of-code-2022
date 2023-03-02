#ifndef __AOC_DAY_17__
#define __AOC_DAY_17__

#include <stdint.h>

#include "constants.h"

#define DAY_17_COLUMN_HEIGHT 10000
#define DAY_17_COLUMN_WIDTH 7

struct day_17_column
{
    char map[DAY_17_COLUMN_HEIGHT][DAY_17_COLUMN_WIDTH+1];
    int high_row;
    int jet_pos;
    int piece_pos;
    int piece_bottom_row;
    int piece_top_row;
};

typedef struct day_17_column day_17_column_t;

void day_17_part_1(char * filename, extra_args_t * extra_args, char * result);
//void day_17_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
