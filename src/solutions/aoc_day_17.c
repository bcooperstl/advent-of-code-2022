#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_17.h"
#include "file_utils.h"

#define EMPTY '.'
#define RESTING '#'
#define DROPPING '@'

#define JET_LEFT '<'
#define JET_RIGHT '>'

#define PART_1_PIECES 2022

static void init_column(day_17_column_t * column)
{
    for (int i=0; i<DAY_17_COLUMN_HEIGHT; i++)
    {
        for (int j=0; j<DAY_17_COLUMN_WIDTH; j++)
        {
            column->map[i][j] = EMPTY;
        }
        column->map[i][DAY_17_COLUMN_WIDTH] = '\0';
    }
    column->high_row = DAY_17_COLUMN_HEIGHT;
    column->piece_pos = 0;
    column->jet_pos = 0;
    column->piece_bottom_row = 0;
    column->piece_top_row = 0;
}

static void display_column(day_17_column_t * column)
{
    for (int i=column->piece_top_row; i<DAY_17_COLUMN_HEIGHT; i++)
    {
        printf("|%s|\n", column->map[i]);
    }
    printf("+-------+\n");
}

static void init_piece(day_17_column_t * column)
{
    column->piece_bottom_row = column->high_row - 4;
#ifdef DEBUG_DAY_17
    printf(" piece_bottom_row starts at %d\n", column->piece_bottom_row);
#endif
    switch (column->piece_pos)
    {
        case 0: // horizontal line
            column->piece_top_row = column->piece_bottom_row;
            column->map[column->piece_bottom_row][2] = DROPPING;
            column->map[column->piece_bottom_row][3] = DROPPING;
            column->map[column->piece_bottom_row][4] = DROPPING;
            column->map[column->piece_bottom_row][5] = DROPPING;
#ifdef DEBUG_DAY_17
            printf(" piece is horizontal line; piece_top_row is %d, initial column with dropping piece:\n", column->piece_top_row);
            display_column(column);
#endif
            break;
        case 1: // plus symbol
            column->piece_top_row = column->piece_bottom_row - 2;
            column->map[column->piece_bottom_row][3] = DROPPING;
            column->map[column->piece_bottom_row-1][2] = DROPPING;
            column->map[column->piece_bottom_row-1][3] = DROPPING;
            column->map[column->piece_bottom_row-1][4] = DROPPING;
            column->map[column->piece_bottom_row-2][3] = DROPPING;
#ifdef DEBUG_DAY_17
            printf(" piece is plus; piece_top_row is %d, initial column with dropping piece:\n", column->piece_top_row);
            display_column(column);
#endif
            break;
        case 2: // right angle
            column->piece_top_row = column->piece_bottom_row - 2;
            column->map[column->piece_bottom_row][2] = DROPPING;
            column->map[column->piece_bottom_row][3] = DROPPING;
            column->map[column->piece_bottom_row][4] = DROPPING;
            column->map[column->piece_bottom_row-1][4] = DROPPING;
            column->map[column->piece_bottom_row-2][4] = DROPPING;
#ifdef DEBUG_DAY_17
            printf(" piece is right angle; piece_top_row is %d, initial column with dropping piece:\n", column->piece_top_row);
            display_column(column);
#endif
            break;
        case 3: // vertical line
            column->piece_top_row = column->piece_bottom_row - 3;
            column->map[column->piece_bottom_row][2] = DROPPING;
            column->map[column->piece_bottom_row-1][2] = DROPPING;
            column->map[column->piece_bottom_row-2][2] = DROPPING;
            column->map[column->piece_bottom_row-3][2] = DROPPING;
#ifdef DEBUG_DAY_17
            printf(" piece is vertical line; piece_top_row is %d, initial column with dropping piece:\n", column->piece_top_row);
            display_column(column);
#endif
            break;
        case 4: // square
            column->piece_top_row = column->piece_bottom_row - 1;
            column->map[column->piece_bottom_row][2] = DROPPING;
            column->map[column->piece_bottom_row][3] = DROPPING;
            column->map[column->piece_bottom_row-1][2] = DROPPING;
            column->map[column->piece_bottom_row-1][3] = DROPPING;
#ifdef DEBUG_DAY_17
            printf(" piece is square; piece_top_row is %d, initial column with dropping piece:\n", column->piece_top_row);
            display_column(column);
#endif
            break;
    }
    column->piece_pos = (column->piece_pos + 1) % 5;
    return;
}

static void jet_left_piece(day_17_column_t * column)
{
#ifdef DEBUG_DAY_17
    printf(" applying jet to the left\n");
#endif
    int blocked = FALSE;
    for (int y=column->piece_top_row; y<=column->piece_bottom_row; y++)
    {
        if (column->map[y][0] == DROPPING)
        {
#ifdef DEBUG_DAY_17
            printf("  dropping cell on left edge at row %d column 0; blocked from moving left\n", y);
#endif
            blocked = TRUE;
        }
        for (int x=1; x<DAY_17_COLUMN_WIDTH; x++)
        {
            if (column->map[y][x] == DROPPING && column->map[y][x-1] == RESTING)
            {
#ifdef DEBUG_DAY_17
                printf("  dropping cell at row %d column %d has resting to its left; blocked from moving left\n", y, x);
#endif
                blocked = TRUE;
            }
        }
    }
    
    if (blocked == FALSE)
    {
#ifdef DEBUG_DAY_17
        printf("  able to move left\n");
#endif
        for (int y=column->piece_top_row; y<=column->piece_bottom_row; y++)
        {
            for (int x=1; x<DAY_17_COLUMN_WIDTH; x++)
            {
                if (column->map[y][x] == DROPPING)
                {
                    column->map[y][x-1] = DROPPING;
                    column->map[y][x] = EMPTY;
                }
            }
        }
#ifdef DEBUG_DAY_17
        printf(" column after moving piece to left:\n", column->piece_top_row);
        display_column(column);
#endif
    }
    return;
}

static void jet_right_piece(day_17_column_t * column)
{
#ifdef DEBUG_DAY_17
    printf(" applying jet to the right\n");
#endif
    int blocked = FALSE;
    for (int y=column->piece_top_row; y<=column->piece_bottom_row; y++)
    {
        if (column->map[y][DAY_17_COLUMN_WIDTH-1] == DROPPING)
        {
#ifdef DEBUG_DAY_17
            printf("  dropping cell on right edge at row %d column %d; blocked from moving right\n", y, DAY_17_COLUMN_WIDTH-1);
#endif
            blocked = TRUE;
        }
        for (int x=0; x<DAY_17_COLUMN_WIDTH - 1; x++)
        {
            if (column->map[y][x] == DROPPING && column->map[y][x+1] == RESTING)
            {
#ifdef DEBUG_DAY_17
                printf("  dropping cell at row %d column %d has resting to its right; blocked from moving right\n", y, x);
#endif
                blocked = TRUE;
            }
        }
    }
    
    if (blocked == FALSE)
    {
#ifdef DEBUG_DAY_17
        printf("  able to move right\n");
#endif
        for (int y=column->piece_top_row; y<=column->piece_bottom_row; y++)
        {
            for (int x=DAY_17_COLUMN_WIDTH - 1; x>=0; x--)
            {
                if (column->map[y][x] == DROPPING)
                {
                    column->map[y][x+1] = DROPPING;
                    column->map[y][x] = EMPTY;
                }
            }
        }
#ifdef DEBUG_DAY_17
        printf(" column after moving piece to right:\n", column->piece_top_row);
        display_column(column);
#endif
    }
    return;
}

// return true if able to drop the piece or false if not able
static int drop_piece_one_row(day_17_column_t * column)
{
#ifdef DEBUG_DAY_17
    printf(" trying to drop piece one row\n");
#endif
    int blocked = FALSE;
    for (int y=column->piece_bottom_row; y>=column->piece_top_row; y--)
    {
        for (int x=0; x<DAY_17_COLUMN_WIDTH; x++)
        {
            if (column->map[y][x] == DROPPING && column->map[y+1][x] == RESTING)
            {
#ifdef DEBUG_DAY_17
                printf("  dropping cell at row %d column %d has resting below it; cannot move down\n", y, x);
#endif
                blocked = TRUE;
            }
        }
    }
    
    // cannot place piece if the downward movement is blocked
    if (blocked)
    {
        return FALSE;
    }
#ifdef DEBUG_DAY_17
    printf("  able to move down\n");
#endif
    for (int y=column->piece_bottom_row; y>=column->piece_top_row; y--)
    {
        for (int x=0; x<DAY_17_COLUMN_WIDTH; x++)
        {
            if (column->map[y][x] == DROPPING)
            {
                column->map[y+1][x] = DROPPING;
                column->map[y][x] = EMPTY;
            }
        }
    }
    column->piece_bottom_row++;
    column->piece_top_row++;
#ifdef DEBUG_DAY_17
        printf(" after dropping piece, the new piece_top_row is %d and piece_bottom_row is %d; the column is:\n", column->piece_top_row, column->piece_bottom_row);
        display_column(column);
#endif
    return TRUE;
}

static void finalize_piece(day_17_column_t * column)
{
#ifdef DEBUG_DAY_17
    printf(" finalizing piece\n");
#endif
    for (int y=column->piece_top_row; y<=column->piece_bottom_row; y++)
    {
        for (int x=0; x<DAY_17_COLUMN_WIDTH; x++)
        {
            if (column->map[y][x] == DROPPING)
            {
                column->map[y][x] = RESTING;
            }
        }
    }
    if (column->piece_top_row < column->high_row)
    {
#ifdef DEBUG_DAY_17
    printf("  the piece top row %d is above than the old high row of %d, so it high_row is being adjusted\n", column->piece_top_row, column->high_row);
#endif
        column->high_row = column->piece_top_row;
    }
    else
    {
#ifdef DEBUG_DAY_17
    printf("  the piece top row %d is below than the old high row of %d, so high row is being left alone\n", column->piece_top_row, column->high_row);
#endif        
    }
}

static void place_piece(day_17_column_t * column, char * jet_pattern, int jet_pattern_length)
{
    init_piece(column);
    int placed = FALSE;
    while (placed == FALSE)
    {
        if (jet_pattern[column->jet_pos] == JET_LEFT)
        {
            jet_left_piece(column);
            placed = TRUE;
        }
        else if (jet_pattern[column->jet_pos] == JET_RIGHT)
        {
            jet_right_piece(column);
        }
        else
        {
            fprintf(stderr, "INVALID JET SYMBOL %c\n", jet_pattern[column->jet_pos]);
        }
        column->jet_pos = (column->jet_pos + 1) % jet_pattern_length;
        if (column->piece_bottom_row == DAY_17_COLUMN_HEIGHT-1)
        {
#ifdef DEBUG_DAY_17
            printf(" piece is on bottom row %d; cannot move down\n", DAY_17_COLUMN_HEIGHT-1);
#endif
            placed = TRUE;
        }
        else
        {
            placed = ((drop_piece_one_row(column) == TRUE) ? FALSE : TRUE);
        }
    }
    finalize_piece(column);
}

void day_17_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_17_column_t column;
    
    init_column(&column);

    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    char * jet_pattern = NULL;
    int jet_pattern_length;
    
    // read in the input file with no delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 9, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }

    ld = fd.head_line;
    td = ld->head_token;
    jet_pattern = td->token;
    jet_pattern_length = strlen(jet_pattern);
    
    for (int i=1; i<= PART_1_PIECES; i++)
    {
#ifdef DEBUG_DAY_17
    printf("Placing piece %d\n", i);
#endif
        place_piece(&column, jet_pattern, jet_pattern_length);
#ifdef DEBUG_DAY_17
        printf("After placement of piece %d, column looks like:\n", i);
        display_column(&column);
#endif
    }

    file_data_cleanup(&fd);
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", DAY_17_COLUMN_HEIGHT-column.high_row);
    
    return;
}

