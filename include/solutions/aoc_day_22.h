#ifndef __AOC_DAY_22__
#define __AOC_DAY_22__

#include <stdint.h>

#include "constants.h"

#define DAY_22_CLOCKWISE 'R'
#define DAY_22_COUNTER_CLOCKWISE 'L'

#define DAY_22_WALL '#'
#define DAY_22_OPEN '.'
#define DAY_22_VOID ' '

#define DAY_22_RIGHT 0
#define DAY_22_DOWN 1
#define DAY_22_LEFT 2
#define DAY_22_UP 3

#define DAY_22_MOVE_RIGHT '>'
#define DAY_22_MOVE_DOWN 'v'
#define DAY_22_MOVE_LEFT '>'
#define DAY_22_MOVE_UP '^'

#define DAY_22_INSTRUCTION_GO       100
#define DAY_22_INSTRUCTION_TURN     101

#define DAY_22_MAX_INSTRUCTIONS 4096

#define DAY_22_MAX_DIMENSION 200

#define PART_2_MAX_EDGE_LENGTH 50

#define DAY_22_FACE_FRONT 0
#define DAY_22_FACE_RIGHT 1
#define DAY_22_FACE_BACK 2
#define DAY_22_FACE_LEFT 3

#define DAY_22_FACE_BOTTOM 1
#define DAY_22_FACE_TOP 3


struct day_22_instruction
{
    int type;
    int num_to_go;
    char direction_to_turn;
};

typedef struct day_22_instruction day_22_instruction_t;

struct day_22_instructions
{
    day_22_instruction_t instructions[DAY_22_MAX_INSTRUCTIONS];
    int num_instructions;
};

typedef struct day_22_instructions day_22_instructions_t;

struct day_22_board
{
    char layout[DAY_22_MAX_DIMENSION+2][DAY_22_MAX_DIMENSION+2+1]; // the +2 are for spaces on each side; the final +1 is for null terminators
    int num_rows; // includes leading and trailing space rows
    int num_cols; // includes leading and trailing space columns
    int row_min_non_void[DAY_22_MAX_DIMENSION+2];
    int row_max_non_void[DAY_22_MAX_DIMENSION+2];
    int col_min_non_void[DAY_22_MAX_DIMENSION+2];
    int col_max_non_void[DAY_22_MAX_DIMENSION+2];
};

typedef struct day_22_board day_22_board_t;

struct day_22_game
{
    day_22_board_t layout_board;
    day_22_board_t tracking_board;
    int pos_x;
    int pos_y;
    char direction;
    day_22_instructions_t instructions;
    int instructions_performed;
};

typedef struct day_22_game day_22_game_t;

struct day_22_cell
{
    int input_row;
    int input_col;
    char value;
};

typedef struct day_22_cell day_22_cell_t;

struct day_22_face
{
    int loaded;
    int edge_length;
    int map_up_direction;
    day_22_cell_t cells[PART_2_MAX_EDGE_LENGTH][PART_2_MAX_EDGE_LENGTH];
};

typedef struct day_22_face day_22_face_t;

// a normal cube has 6 faces. mine has 8 to handle the back face for left/right and up/down rotations

struct day_22_cube
{
    int edge_length;
    day_22_face_t left_right_rotation_faces[4]; // front, right, back, left
    day_22_face_t up_down_rotation_faces[4]; // front, bottom, back, top
};

typedef struct day_22_cube day_22_cube_t;

void day_22_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_22_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
