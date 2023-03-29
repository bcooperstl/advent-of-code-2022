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
#define PART_2_PIECES 1000000000000ll // long long

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
    column->rows_purged = 0ll;
}

static void display_column(day_17_column_t * column)
{
    int start_row = column->piece_top_row;
    if (column->high_row < column->piece_top_row)
    {
        start_row = column->high_row;
    }
    for (int i=start_row; i<DAY_17_COLUMN_HEIGHT; i++)
    {
        printf("%04d  |%s|\n", i, column->map[i]);
    }
    printf("      +-------+\n");
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

static void purge_rows(day_17_column_t * column)
{
#ifdef DEBUG_DAY_17
    printf(" trying to purge rows\n");
#endif
    int rows_to_purge = DAY_17_COLUMN_HEIGHT;
    for (int x=0; x<DAY_17_COLUMN_WIDTH; x++)
    {    
        int top_resting_row = DAY_17_COLUMN_HEIGHT;
        for (int y=column->high_row; y<DAY_17_COLUMN_HEIGHT; y++)
        {
            if (column->map[y][x] == RESTING)
            {
                top_resting_row = y;
                break;
            }
        }
        if (top_resting_row == DAY_17_COLUMN_HEIGHT)
        {
#ifdef DEBUG_DAY_17
            printf("  column %d does not have any resting pieces. cannot purge rows\n", x);
#endif
            rows_to_purge = 0;
        }
        else
        {
            int col_rows_to_purge = DAY_17_COLUMN_HEIGHT - top_resting_row - 1;
#ifdef DEBUG_DAY_17
            printf("  column %d has highest # at row %d; can purge %d rows below it\n", x, top_resting_row, col_rows_to_purge);
#endif
            if (col_rows_to_purge < rows_to_purge)
            {
                rows_to_purge = col_rows_to_purge;
#ifdef DEBUG_DAY_17
                printf("   adjusting overall rows to purge to match column %d\n", x);
#endif
            }
            else
            {
#ifdef DEBUG_DAY_17
                printf("   overall rows to purge is more restrictive %d; not changing for to match column %d\n", rows_to_purge, x);
#endif
            }
        
        }
    }
#ifdef DEBUG_DAY_17
    printf(" after checking all columns, there are %d rows to purge\n", rows_to_purge);
#endif
    if (rows_to_purge == 0)
    {
        return;
    }
    int rows_to_keep = DAY_17_COLUMN_HEIGHT - column->high_row - rows_to_purge;
    
#ifdef DEBUG_DAY_17
    printf("BEFORE PURGING; top_row is %d and rows_purged = %lld:\n", column->high_row, column->rows_purged);
    display_column(column);
    printf("There are %d rows to keep and %d rows to purge\n", rows_to_keep, rows_to_purge);
#endif
    for (int i=0; i<rows_to_keep; i++)
    {
        int dest_y = DAY_17_COLUMN_HEIGHT - 1 - i;
        int src_y = dest_y - rows_to_purge;

#ifdef DEBUG_DAY_17
    printf("  copying row %d to %d\n", src_y, dest_y);
#endif
        strncpy(column->map[dest_y], column->map[src_y], DAY_17_COLUMN_WIDTH+1);
    }
    for (int i=0; i<rows_to_purge; i++)
    {
#ifdef DEBUG_DAY_17
    printf("  clearing row %d\n", column->high_row+i);
#endif
        for (int x=0; x<DAY_17_COLUMN_WIDTH; x++)
        {
            column->map[column->high_row+i][x] = EMPTY;
        }
    }
    
    column->rows_purged += rows_to_purge;
    column->high_row += rows_to_purge;
#ifdef DEBUG_DAY_17
    printf("AFTER PURGING; top_row is %d and rows_purged = %lld:\n", column->high_row, column->rows_purged);
    display_column(column);
#endif
    return;
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
    purge_rows(column);
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
    snprintf(result, MAX_RESULT_LENGTH+1, "%lld", column.rows_purged + ((long long)(DAY_17_COLUMN_HEIGHT-column.high_row)));
    
    return;
}

static void init_state_history(day_17_state_history_t * history)
{
    for (int i=0; i<DAY_17_MAX_JET_PATTERN_LENGTH; i++)
    {
        history->states_by_jet_pos[i] = NULL;
    }
}

static void cleanup_state_history(day_17_state_history_t * history)
{
    for (int i=0; i<DAY_17_MAX_JET_PATTERN_LENGTH; i++)
    {
        if (history->states_by_jet_pos[i] != NULL)
        {
            day_17_state_t * curr = history->states_by_jet_pos[i];
            day_17_state_t * next;
            while (curr != NULL)
            {
                next = curr->next;
                free(curr);
                curr = next;
            }
        }
    }
}

static void build_state(day_17_column_t * column, day_17_state_t * state, long long pieces_placed)
{
    for (int i=0; i<DAY_17_COLUMN_HEIGHT; i++)
    {
        memcpy(state->map[i], column->map[i], DAY_17_COLUMN_WIDTH+1);
    }
    state->high_row = column->high_row;
    state->rows_purged = column->rows_purged;
    state->pieces_placed = pieces_placed;
    state->next = NULL;
}

static day_17_state_t * find_matching_state(day_17_state_history_t * history, day_17_state_t * find_state, int jet_pos)
{
    day_17_state_t * hist_state = history->states_by_jet_pos[jet_pos];
    while (hist_state != NULL)
    {
        if (hist_state->high_row == find_state->high_row)
        {
            int mismatch = FALSE;
            for (int i=hist_state->high_row; i<DAY_17_COLUMN_HEIGHT; i++)
            {
                if (strncmp(hist_state->map[i], find_state->map[i], DAY_17_COLUMN_WIDTH) != 0)
                {
                    mismatch = TRUE;
                    break;
                }
            }
            if (mismatch == FALSE)
            {
                return hist_state;
            }
        }
        
        hist_state = hist_state->next;
    }
    return NULL;
}

static void add_current_state_to_history(day_17_state_history_t * history, day_17_state_t * new_state, int jet_pos)
{
    if (history->states_by_jet_pos[jet_pos] == NULL)
    {
        history->states_by_jet_pos[jet_pos] = new_state;
    }
    else
    {
        day_17_state_t * curr = history->states_by_jet_pos[jet_pos];
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = new_state;
    }
    return;
}

void day_17_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_17_column_t column;
    day_17_state_history_t history;
    
    init_column(&column);
    init_state_history(&history);

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
    
    int match_found_and_adjusted = FALSE;
    
    for (long long i=1; i<= PART_2_PIECES; i++)
    {
#ifdef DEBUG_DAY_17_HISTORY
    printf("Placing piece %d\n", i);
#endif
        place_piece(&column, jet_pattern, jet_pattern_length);
#ifdef DEBUG_DAY_17
        printf("After placement of piece %d, column looks like:\n", i);
        display_column(&column);
#endif
        if (match_found_and_adjusted == FALSE && column.piece_pos == 1) // the horizonal line was just placed; going to track for after it
        {
#ifdef DEBUG_DAY_17_HISTORY
            printf("Building history for piece %lld with jet_position %d\n", i, column.jet_pos);
#endif
            day_17_state_t * new_state;
            build_state(&column, new_state, i);
            day_17_state_t * matched_state = NULL;
            matched_state = find_matching_state(&history, new_state, column.jet_pos);
            if (matched_state != NULL)
            {
                match_found_and_adjusted = TRUE;

#ifdef DEBUG_DAY_17_HISTORY
                printf(" Matching state found!\n");
                printf("  Matching state has %lld pieces placed and %lld rows purged!\n", matched_state->pieces_placed, matched_state->rows_purged);
                printf("  Current state has %lld pieces placed and %lld rows purged!\n", new_state->pieces_placed, new_state->rows_purged);
                
#endif
                
                long long pieces_placed_difference = new_state->pieces_placed - matched_state->pieces_placed;
                long long rows_purged_difference = new_state->rows_purged - matched_state->rows_purged;
                long long cycles_to_run = (PART_2_PIECES - i) / pieces_placed_difference;
                
#ifdef DEBUG_DAY_17_HISTORY
                printf("  Cycle will place %lld pieces and purge %lld rows.\n", pieces_placed_difference, rows_purged_difference);
                printf("  Going to run %lld cycles\n", cycles_to_run);
#endif
                
                i = i + (cycles_to_run * pieces_placed_difference);
                column.rows_purged = column.rows_purged + (cycles_to_run * rows_purged_difference);
                
#ifdef DEBUG_DAY_17_HISTORY
                printf("  After running cycles, there are %lld pieces placed and %lld rows purged.\n", i, column->rows_purged);
#endif
                
                free(new_state); // clean up memory
            }
            else
            {
#ifdef DEBUG_DAY_17_HISTORY
                printf(" No matching state found; adding to history\n");
#endif
                add_current_state_to_history(&history, new_state, column.jet_pos);
            }
        
        }
    }

    file_data_cleanup(&fd);
    snprintf(result, MAX_RESULT_LENGTH+1, "%lld", column.rows_purged + ((long long)(DAY_17_COLUMN_HEIGHT-column.high_row)));
    
    cleanup_state_history(&history);
    
    return;
}

