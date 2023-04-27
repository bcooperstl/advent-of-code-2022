#ifndef __AOC_DAY_17__
#define __AOC_DAY_17__

#include <stdint.h>

#include "constants.h"

#define DAY_17_COLUMN_HEIGHT 256
#define DAY_17_COLUMN_WIDTH 7

#define DAY_17_MAX_JET_PATTERN_LENGTH 12000

struct day_17_column
{
    char map[DAY_17_COLUMN_HEIGHT][DAY_17_COLUMN_WIDTH+1];
    int high_row;
    int jet_pos;
    int piece_pos;
    int piece_bottom_row;
    int piece_top_row;
    long long rows_purged;
};

typedef struct day_17_column day_17_column_t;

struct day_17_state;

struct day_17_state
{
    char map[DAY_17_COLUMN_HEIGHT][DAY_17_COLUMN_WIDTH+1];
    int high_row;
    long long rows_purged;
    long long pieces_placed;
    struct day_17_state * next;
};

typedef struct day_17_state day_17_state_t;

struct day_17_state_history
{
    day_17_state_t * states_by_jet_pos[DAY_17_MAX_JET_PATTERN_LENGTH];
};

typedef struct day_17_state_history day_17_state_history_t;

void day_17_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_17_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
