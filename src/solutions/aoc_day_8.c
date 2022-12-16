#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_8.h"
#include "file_utils.h"

static void init_forest(day_8_forest_t * forest, int rows, int cols)
{
    forest->rows = rows;
    forest->cols = cols;
    forest->data = (char **)malloc(rows * sizeof(char *));
    for (int row=0; row<rows; row++)
    {
        forest->data[row] = (char *)malloc(cols * sizeof(char));
    }
}

static void cleanup_forest(day_8_forest_t * forest)
{
    for (int row=0; row<forest->rows; row++)
    {
        free(forest->data[row]);
    }
    free(forest->data);
}

static int parse_input(char * filename, day_8_forest_t * forest)
{
    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with no delimiteres
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }

    ld = fd.head_line;
    td = ld->head_token;
    
    init_forest(forest, fd.num_lines, strlen(td->token)); // rows = num_lines, cols = length of the first line
    
#ifdef DEBUG_DAY_8
    printf("Forest from %s initialized with %d rows and %d cols\n", filename, forest->rows, forest->cols);
#endif
    
    int row=0;
    
    while (ld != NULL)
    {
        td = ld->head_token;
        memcpy(forest->data[row], td->token, forest->cols);
        ld = ld->next;
        row++;
    }

    file_data_cleanup(&fd);
    
    return TRUE;
}

static int is_visible(day_8_forest_t * forest, int row, int col)
{
#ifdef DEBUG_DAY_8
        printf("Checking if row %d col %d (%c=%d) is visible\n", row, col, forest->data[row][col], forest->data[row][col]);
#endif
    // short circuit for visible at top, bottom, left, or right
    if (row == 0 || 
        row == (forest->rows - 1) || 
        col == 0 || 
        col == (forest->cols - 1))
    {
#ifdef DEBUG_DAY_8
        printf("  row %d col %d is visible because it is on the edge of the forest\n", row, col);
#endif
        return TRUE;
    }

    // check if visible from the top
    int is_vis_dir = TRUE;
    for (int above_row=0; above_row<row; above_row++)
    {
        if (forest->data[above_row][col] >= forest->data[row][col])
        {
            is_vis_dir = FALSE;
#ifdef DEBUG_DAY_8
            printf("  row %d col %d is not visible from the top because row %d col %d is not lower\n", row, col, above_row, col);
#endif
            break;
        }
    }
    if (is_vis_dir == TRUE)
    {
#ifdef DEBUG_DAY_8
        printf("  row %d col %d is visible because it can be viewed from the top\n", row, col);
#endif
        return TRUE;
    }
    
    // check if visible from the bottom
    is_vis_dir = TRUE;
    for (int below_row=forest->rows-1; below_row>row; below_row--)
    {
        if (forest->data[below_row][col] >= forest->data[row][col])
        {
            is_vis_dir = FALSE;
#ifdef DEBUG_DAY_8
            printf("  row %d col %d is not visible from the bottom because row %d col %d is not lower\n", row, col, below_row, col);
#endif
            break;
        }
    }
    if (is_vis_dir == TRUE)
    {
#ifdef DEBUG_DAY_8
        printf("  row %d col %d is visible because it can be viewed from the bottom\n", row, col);
#endif
        return TRUE;
    }
    
    // check if visible from the left
    is_vis_dir = TRUE;
    for (int left_col=0; left_col<col; left_col++)
    {
        if (forest->data[row][left_col] >= forest->data[row][col])
        {
            is_vis_dir = FALSE;
#ifdef DEBUG_DAY_8
            printf("  row %d col %d is not visible from the left because row %d col %d is not lower\n", row, col, row, left_col);
#endif
            break;
        }
    }
    if (is_vis_dir == TRUE)
    {
#ifdef DEBUG_DAY_8
        printf("  row %d col %d is visible because it can be viewed from the left\n", row, col);
#endif
        return TRUE;
    }
    
    // check if visible from the right
    is_vis_dir = TRUE;
    for (int right_col=forest->cols-1; right_col>col; right_col--)
    {
        if (forest->data[row][right_col] >= forest->data[row][col])
        {
            is_vis_dir = FALSE;
#ifdef DEBUG_DAY_8
            printf("  row %d col %d is not visible from the right because row %d col %d is not lower\n", row, col, row, right_col);
#endif
            break;
        }
    }
    if (is_vis_dir == TRUE)
    {
#ifdef DEBUG_DAY_8
        printf("  row %d col %d is visible because it can be viewed from the right\n", row, col);
#endif
        return TRUE;
    }
    
#ifdef DEBUG_DAY_8
    printf("  row %d col %d is not visible from any direction\n", row, col);
#endif
    return FALSE;
}

void day_8_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_8_forest_t forest;
    
    if (parse_input(filename, &forest) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    int num_visible = 0;
    for (int row=0; row<forest.rows; row++)
    {
        for (int col=0; col<forest.cols; col++)
        {
            if (is_visible(&forest, row, col) == TRUE)
            {
                num_visible++;
            }
        }
    }
    
    cleanup_forest(&forest);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", num_visible);
    
    return;
}
