#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "aoc_solutions.h"
#include "aoc_day_12.h"
#include "file_utils.h"

static int parse_input(char * filename, day_12_map_t * map)
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
    
    map->num_rows = fd.num_lines;
    map->num_cols = strlen(td->token);
    
#ifdef DEBUG_DAY_12
    printf("map from %s initialized with %d rows and %d cols\n", filename, map->num_rows, map->num_cols);
#endif
    
    int row=0;
    
    while (ld != NULL)
    {
        td = ld->head_token;
        for (int col=0; col<map->num_cols; col++)
        {
            map->nodes[row][col].elevation = td->token[col];
            map->nodes[row][col].steps_to_here = INT_MAX;
            map->nodes[row][col].evaluated = FALSE;
#ifdef DEBUG_DAY_12
            printf("%c", map->nodes[row][col].elevation);
#endif
        }            
#ifdef DEBUG_DAY_12
        printf("\n");
#endif
        ld = ld->next;
        row++;
    }

    return TRUE;
}

static void find_start_and_end (day_12_map_t * map, int * start_row, int * start_col, int * end_row, int * end_col)
{
    for (int row=0; row<map->num_rows; row++)
    {
        for (int col=0; col<map->num_cols; col++)
        {
            if (map->nodes[row][col].elevation == 'S')
            {
#ifdef DEBUG_DAY_12
                printf("Starting location is at row=%d, col=%d\n", row, col);
#endif
                map->nodes[row][col].elevation = 'a';
                map->nodes[row][col].steps_to_here = 0;
                *start_row = row;
                *start_col = col;
            }
            if (map->nodes[row][col].elevation == 'E')
            {
#ifdef DEBUG_DAY_12
                printf("Ending location is at row=%d, col=%d\n", row, col);
#endif
                map->nodes[row][col].elevation = 'z';
                *end_row = row;
                *end_col = col;
            }
        }
    }
    return;
}   

static void reset_map (day_12_map_t * map)
{
    for (int row=0; row<map->num_rows; row++)
    {
        for (int col=0; col<map->num_cols; col++)
        {
            map->nodes[row][col].steps_to_here = INT_MAX;
            map->nodes[row][col].evaluated = FALSE;
        }
    }
    return;
}

            

static int find_next_node_to_evaluate(day_12_map_t * map, int * next_row, int * next_col)
{
    int found_any = FALSE;
    int min_steps = INT_MAX;

#ifdef DEBUG_DAY_12
    printf("Searching for next node to evaluate\n");
#endif
    for (int row=0; row<map->num_rows; row++)
    {
        for (int col=0; col<map->num_cols; col++)
        {
            if (map->nodes[row][col].evaluated == FALSE && map->nodes[row][col].steps_to_here < min_steps)
            {
                min_steps = map->nodes[row][col].steps_to_here;
                found_any = TRUE;
                *next_row = row;
                *next_col = col;
#ifdef DEBUG_DAY_12
                printf(" minimum found at row=%d col=%d with steps_to_here=%d\n", row, col, min_steps);
#endif
            }
        }
    }
    return found_any;
}

static void evaluate_node(day_12_map_t * map, int node_row, int node_col)
{
    char node_elevation = map->nodes[node_row][node_col].elevation;
    int  node_steps = map->nodes[node_row][node_col].steps_to_here;

#ifdef DEBUG_DAY_12
    printf("Evaluating neighbors for row=%d col=%d with elevation=%c and steps_to_here=%d\n", node_row, node_col, node_elevation, node_steps);
#endif

    // up
    if (node_row > 0)
    {
        if ((map->nodes[node_row-1][node_col].evaluated == FALSE) &&
            (map->nodes[node_row-1][node_col].elevation <= (node_elevation + 1)) &&
            (map->nodes[node_row-1][node_col].steps_to_here > node_steps + 1))
        {
#ifdef DEBUG_DAY_12
            printf(" Marking UP node at row=%d and col=%d to have steps %d\n", node_row-1, node_col, node_steps+1);
#endif
            map->nodes[node_row-1][node_col].steps_to_here = node_steps+1;
        }
    }
    
    // down
    if (node_row < (map->num_rows - 1))
    {
        if ((map->nodes[node_row+1][node_col].evaluated == FALSE) &&
            (map->nodes[node_row+1][node_col].elevation <= (node_elevation + 1)) &&
            (map->nodes[node_row+1][node_col].steps_to_here > node_steps + 1))
        {
#ifdef DEBUG_DAY_12
            printf(" Marking DOWN node at row=%d and col=%d to have steps %d\n", node_row+1, node_col, node_steps+1);
#endif
            map->nodes[node_row+1][node_col].steps_to_here = node_steps+1;
        }
    }
        
    // left
    if (node_col > 0)
    {
        if ((map->nodes[node_row][node_col-1].evaluated == FALSE) &&
            (map->nodes[node_row][node_col-1].elevation <= (node_elevation + 1)) &&
            (map->nodes[node_row][node_col-1].steps_to_here > node_steps + 1))
        {
#ifdef DEBUG_DAY_12
            printf(" Marking LEFT node at row=%d and col=%d to have steps %d\n", node_row, node_col-1, node_steps+1);
#endif
            map->nodes[node_row][node_col-1].steps_to_here = node_steps+1;
        }
    }
    
    // right
    if (node_col < (map->num_cols - 1))
    {
        if ((map->nodes[node_row][node_col+1].evaluated == FALSE) &&
            (map->nodes[node_row][node_col+1].elevation <= (node_elevation + 1)) &&
            (map->nodes[node_row][node_col+1].steps_to_here > node_steps + 1))
        {
#ifdef DEBUG_DAY_12
            printf(" Marking RIGHT node at row=%d and col=%d to have steps %d\n", node_row, node_col+1, node_steps+1);
#endif
            map->nodes[node_row][node_col+1].steps_to_here = node_steps+1;
        }
    }
    
    map->nodes[node_row][node_col].evaluated = TRUE;
    
    return;
}
 
void day_12_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_12_map_t map;
    int start_row, start_col, end_row, end_col, node_row, node_col;
    
    if (parse_input(filename, &map) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    find_start_and_end(&map, &start_row, &start_col, &end_row, &end_col);
    
    while (find_next_node_to_evaluate(&map, &node_row, &node_col) == TRUE)
    {
        evaluate_node(&map, node_row, node_col);
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", map.nodes[end_row][end_col].steps_to_here);
    
    return;
}

void day_12_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_12_map_t map;
    int start_row, start_col, end_row, end_col, node_row, node_col;
    
    int best_steps = INT_MAX;
    
    if (parse_input(filename, &map) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    find_start_and_end(&map, &start_row, &start_col, &end_row, &end_col);
    
    for (start_row=0; start_row<map.num_rows; start_row++)
    {
        for (start_col=0; start_col<map.num_cols; start_col++)
        {
            if (map.nodes[start_row][start_col].elevation == 'a')
            {
                if ((start_row != 0 && map.nodes[start_row-1][start_col].elevation != 'a') ||
                    (start_row != map.num_rows-1 && map.nodes[start_row+1][start_col].elevation != 'a') ||
                    (start_col != 0 && map.nodes[start_row][start_col-1].elevation != 'a') ||
                    (start_col != map.num_cols-1 && map.nodes[start_row][start_col+1].elevation != 'a'))
                {
#ifdef DEBUG_DAY_12_2
                    printf("Running with start at row=%d col=%d\n", start_row, end_row);
#endif
                    reset_map(&map);
                    map.nodes[start_row][start_col].steps_to_here = 0;
                
                    while (find_next_node_to_evaluate(&map, &node_row, &node_col) == TRUE)
                    {
                        evaluate_node(&map, node_row, node_col);
                    }
                
#ifdef DEBUG_DAY_12_2
                    printf(" End node can be reached in %d steps\n", map.nodes[end_row][end_col].steps_to_here);
#endif
                    if (map.nodes[end_row][end_col].steps_to_here < best_steps)
                    {
                        best_steps = map.nodes[end_row][end_col].steps_to_here;
#ifdef DEBUG_DAY_12_2
                        printf(" NEW BEST STEPS\n");
#endif
                    }
                }
                else
                {
#ifdef DEBUG_DAY_12_2
                    printf("Short circuiting to skip start at row=%d col=%d\n", start_row, end_row);
#endif
                }
            }
        }
    }
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", best_steps);
    
    return;
}

