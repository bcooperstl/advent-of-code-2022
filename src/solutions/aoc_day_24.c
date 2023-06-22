#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_24.h"
#include "file_utils.h"

#define UP '^'
#define DOWN 'v'
#define LEFT '<'
#define RIGHT '>'
#define UP_DOWN '|'
#define LEFT_RIGHT '-'
#define EMPTY '.'
#define PATH 'E'

static void read_and_parse_input(char * filename, day_24_map_t * init_map)
{
    file_data_t fd;
    
    // read in the input file;  no delimiters
    
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }
    
    init_map->num_rows = fd.num_lines;
    init_map->num_cols = strlen(fd.head_line->head_token->token);
    
    line_data_t * ld = fd.head_line;
    
    int row_count = 0;
    while (ld != NULL && strlen(ld->head_token->token) > 0)
    {
        strncpy(init_map->data[row_count], ld->head_token->token, DAY_24_MAP_MAX_COLS);
        init_map->data[row_count][init_map->num_cols]='\0'; // make sure a null terminator is set
        
        row_count++;
        ld = ld->next;
    }
    
    file_data_cleanup(&fd);
    
    return;
}

static void display_map(day_24_map_t * map)
{
    printf("Map is %d rows and %d cols\n", map->num_rows, map->num_cols);
    for (int i=0; i<map->num_rows; i++)
    {
        printf("%s\n", map->data[i]);
    }
    return;
}

static void copy_map(day_24_map_t * dest, day_24_map_t * src)
{
    memcpy(dest, src, sizeof(day_24_map_t));
}

static void build_permutations(day_24_map_component_permutations_t * permutations, day_24_map_t * init_map)
{
    permutations->num_up_down_permutations = init_map->num_rows - 2; // drop the top and bottom rows for permutations
    permutations->num_left_right_permutations = init_map->num_cols - 2; // drop the left and right cols for permutations

#ifdef DEBUG_DAY_24
    printf("Building permutations for %d up/down and %d left/right options\n", permutations->num_up_down_permutations, permutations->num_left_right_permutations);
#endif

    // make a blank map, as well as the base up/down and left/right permutation maps
    copy_map(&permutations->blank_map, init_map);
    
    day_24_map_t * base_up_down = &permutations->up_down_permutations[0];
    day_24_map_t * base_left_right = &permutations->left_right_permutations[0];
    copy_map(base_up_down, init_map);
    copy_map(base_left_right, init_map);
    
    for (int row=1; row<init_map->num_rows-1; row++)
    {
        for (int col=1; col<init_map->num_cols-1; col++)
        {
            // turn off all in blank map
            permutations->blank_map.data[row][col] = EMPTY;
            
            // if up or down, turn off in base left/right
            if ((init_map->data[row][col] == UP) || (init_map->data[row][col] == DOWN))
            {
                base_left_right->data[row][col] = EMPTY;
            }

            // if left or right, turn off in base up/down
            if ((init_map->data[row][col] == LEFT) || (init_map->data[row][col] == RIGHT))
            {
                base_up_down->data[row][col] = EMPTY;
            }
        }
    }

#ifdef DEBUG_DAY_24
    printf("Blank Map:\n");
    display_map(&permutations->blank_map);
    printf("Base Up/Down Map:\n");
    display_map(base_up_down);
    printf("Base Left/Right Map:\n");
    display_map(base_left_right);
#endif
    
    // Work the up/down permutations
    // Step 1 - copy the blank map to all of the other permutations
    for (int i=1; i<permutations->num_up_down_permutations; i++)
    {
        copy_map(&permutations->up_down_permutations[i], &permutations->blank_map);
    }
    
    // Step 2 - plot the ups and downs
    for (int base_row=1; base_row<=base_up_down->num_rows-1; base_row++)
    {
        for (int base_col=1; base_col<=base_up_down->num_cols-1; base_col++)
        {
            if (base_up_down->data[base_row][base_col] == UP)
            {
                for (int perm_idx=1; perm_idx<permutations->num_up_down_permutations; perm_idx++)
                {
                    int target_row = (permutations->num_up_down_permutations + base_row - perm_idx ) % permutations->num_up_down_permutations;
                    if (target_row == 0)
                    {
                        target_row = permutations->num_up_down_permutations;
                    }
                    permutations->up_down_permutations[perm_idx].data[target_row][base_col] = (permutations->up_down_permutations[perm_idx].data[target_row][base_col] == EMPTY ? UP : UP_DOWN);
                }
            }
            if (base_up_down->data[base_row][base_col] == DOWN)
            {
                for (int perm_idx=1; perm_idx<permutations->num_up_down_permutations; perm_idx++)
                {
                    int target_row = (base_row + perm_idx ) % permutations->num_up_down_permutations;
                    if (target_row == 0)
                    {
                        target_row = permutations->num_up_down_permutations;
                    }
                    permutations->up_down_permutations[perm_idx].data[target_row][base_col] = (permutations->up_down_permutations[perm_idx].data[target_row][base_col] == EMPTY ? DOWN : UP_DOWN);
                }
            }
        }
    }
#ifdef DEBUG_DAY_24
    for (int i=0; i<permutations->num_up_down_permutations; i++)
    {
        printf("UP_DOWN Permuation %d:\n", i);
        display_map(&permutations->up_down_permutations[i]);
    }
#endif
            
    // Work the left/right permutations
    // Step 1 - copy the blank map to all of the other permutations
    for (int i=1; i<permutations->num_left_right_permutations; i++)
    {
        copy_map(&permutations->left_right_permutations[i], &permutations->blank_map);
    }
    
    // Step 2 - plot the lefts and rights
    for (int base_row=1; base_row<=base_left_right->num_rows-1; base_row++)
    {
        for (int base_col=1; base_col<=base_left_right->num_cols-1; base_col++)
        {
            if (base_left_right->data[base_row][base_col] == LEFT)
            {
                for (int perm_idx=1; perm_idx<permutations->num_left_right_permutations; perm_idx++)
                {
                    int target_col = (permutations->num_left_right_permutations + base_col - perm_idx ) % permutations->num_left_right_permutations;
                    if (target_col == 0)
                    {
                        target_col = permutations->num_left_right_permutations;
                    }
                    permutations->left_right_permutations[perm_idx].data[base_row][target_col] = (permutations->left_right_permutations[perm_idx].data[base_row][target_col] == EMPTY ? LEFT : LEFT_RIGHT);
                }
            }
            if (base_left_right->data[base_row][base_col] == RIGHT)
            {
                for (int perm_idx=1; perm_idx<permutations->num_left_right_permutations; perm_idx++)
                {
                    int target_col = (base_col + perm_idx ) % permutations->num_left_right_permutations;
                    if (target_col == 0)
                    {
                        target_col = permutations->num_left_right_permutations;
                    }
                    permutations->left_right_permutations[perm_idx].data[base_row][target_col] = (permutations->left_right_permutations[perm_idx].data[base_row][target_col] == EMPTY ? RIGHT : LEFT_RIGHT);
                }
            }
        }
    }
#ifdef DEBUG_DAY_24
    for (int i=0; i<permutations->num_left_right_permutations; i++)
    {
        printf("left_right Permuation %d:\n", i);
        display_map(&permutations->left_right_permutations[i]);
    }
#endif
            
    return;
}

static void create_map_for_minute(day_24_map_t * target, day_24_map_component_permutations_t * permutations, int minute)
{
    // Copy the blank map over the target first
    copy_map(target, &permutations->blank_map);

    // calculate source maps
    int source_up_down = minute % permutations->num_up_down_permutations;
    int source_left_right = minute % permutations->num_left_right_permutations;
    
    day_24_map_t * source_up_down_map = &permutations->up_down_permutations[source_up_down];
    day_24_map_t * source_left_right_map = &permutations->left_right_permutations[source_left_right];
    
    
#ifdef DEBUG_DAY_24
    printf("Creating map for minute %d based on up/down map %d of %d and left/right map %d of %d\n", minute, source_up_down, permutations->num_up_down_permutations,
           source_left_right, permutations->num_left_right_permutations);
    display_map(source_up_down_map);
    display_map(source_left_right_map);
#endif
    
    // now load the stuff
    
    for (int row=1; row<target->num_rows-1; row++)
    {
        for (int col=1; col<target->num_cols-1; col++)
        {
            if (source_up_down_map->data[row][col] == UP)
            {
                if (source_left_right_map->data[row][col] == EMPTY)
                {
                    target->data[row][col] = UP;
                }
                else if ((source_left_right_map->data[row][col] == LEFT) || (source_left_right_map->data[row][col] == RIGHT))
                {
                    target->data[row][col] = '2';
                }
                else if (source_left_right_map->data[row][col] == LEFT_RIGHT)
                {
                    target->data[row][col] = '3';
                }
                else
                {
                    fprintf(stderr, "SOMETHING MESSED UP AT ROW %d COL %d\n", row, col);
                }
            }
            else if (source_up_down_map->data[row][col] == DOWN)
            {
                if (source_left_right_map->data[row][col] == EMPTY)
                {
                    target->data[row][col] = DOWN;
                }
                else if ((source_left_right_map->data[row][col] == LEFT) || (source_left_right_map->data[row][col] == RIGHT))
                {
                    target->data[row][col] = '2';
                }
                else if (source_left_right_map->data[row][col] == LEFT_RIGHT)
                {
                    target->data[row][col] = '3';
                }
                else
                {
                    fprintf(stderr, "SOMETHING MESSED UP AT ROW %d COL %d\n", row, col);
                }
            }
            else if (source_up_down_map->data[row][col] == UP_DOWN)
            {
                if (source_left_right_map->data[row][col] == EMPTY)
                {
                    target->data[row][col] = '2';
                }
                else if ((source_left_right_map->data[row][col] == LEFT) || (source_left_right_map->data[row][col] == RIGHT))
                {
                    target->data[row][col] = '3';
                }
                else if (source_left_right_map->data[row][col] == LEFT_RIGHT)
                {
                    target->data[row][col] = '4';
                }
                else
                {
                    fprintf(stderr, "SOMETHING MESSED UP AT ROW %d COL %d\n", row, col);
                }
            }
            else if (source_up_down_map->data[row][col] == EMPTY)
            {
                if (source_left_right_map->data[row][col] == EMPTY)
                {
                    target->data[row][col] = EMPTY;
                }
                else if (source_left_right_map->data[row][col] == LEFT)
                {
                    target->data[row][col] = LEFT;
                }
                else if (source_left_right_map->data[row][col] == RIGHT)
                {
                    target->data[row][col] = RIGHT;
                }
                else if (source_left_right_map->data[row][col] == LEFT_RIGHT)
                {
                    target->data[row][col] = '2';
                }
                else
                {
                    fprintf(stderr, "SOMETHING MESSED UP AT ROW %d COL %d\n", row, col);
                }
            }
            else
            {
                fprintf(stderr, "SOMETHING MESSED UP AT ROW %d COL %d\n", row, col);
            }
        }
    }
                
                    
#ifdef DEBUG_DAY_24
    printf("Calculated Map is:\n");
    display_map(target);
#endif
    return;
}

static void expand_paths(day_24_map_t * next_map, day_24_map_t * prev_map_with_paths)
{
#ifdef DEBUG_DAY_24
    printf("Expanding paths from prior Map:\n");
    display_map(prev_map_with_paths);
    printf("Into target map:\n");
    display_map(next_map);
#endif

    // Check two possible start positions
    if (prev_map_with_paths->data[0][1] == PATH)
    {
        next_map->data[0][1] = PATH;
        if (next_map->data[1][1] == EMPTY)
        {
            next_map->data[1][1] = PATH;
        }
    }
    if (prev_map_with_paths->data[prev_map_with_paths->num_rows-1][prev_map_with_paths->num_cols-2] == PATH)
    {
        next_map->data[prev_map_with_paths->num_rows-1][prev_map_with_paths->num_cols-2] = PATH;
        if (next_map->data[prev_map_with_paths->num_rows-2][prev_map_with_paths->num_cols-2] == EMPTY)
        {
            next_map->data[prev_map_with_paths->num_rows-2][prev_map_with_paths->num_cols-2] = PATH;
        }
    }
    
    // Look through every reachable location in prev_map_with_paths to find PATH elements. 
    // When a PATH is found, check self, up, down, left, right in next_map->and expand to any open spaces
    for (int prev_row=1; prev_row < prev_map_with_paths->num_rows-1; prev_row++)
    {
        for (int prev_col=1; prev_col < prev_map_with_paths->num_cols-1; prev_col++)
        {
            if (prev_map_with_paths->data[prev_row][prev_col] == PATH)
            {
                // self
                if (next_map->data[prev_row][prev_col] == EMPTY)
                {
                    next_map->data[prev_row][prev_col] = PATH;
                }
                // up
                if (next_map->data[prev_row-1][prev_col] == EMPTY)
                {
                    next_map->data[prev_row-1][prev_col] = PATH;
                }
                // down
                if (next_map->data[prev_row+1][prev_col] == EMPTY)
                {
                    next_map->data[prev_row+1][prev_col] = PATH;
                }
                // left
                if (next_map->data[prev_row][prev_col-1] == EMPTY)
                {
                    next_map->data[prev_row][prev_col-1] = PATH;
                }
                // right
                if (next_map->data[prev_row][prev_col+1] == EMPTY)
                {
                    next_map->data[prev_row][prev_col+1] = PATH;
                }
            }
        }
    }
#ifdef DEBUG_DAY_24
    printf("After combining:\n");
    display_map(next_map);
#endif
    return;
}

void day_24_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_24_map_t init_map;
    day_24_map_component_permutations_t component_permutations;
    
    read_and_parse_input(filename, &init_map);
    
#ifdef DEBUG_DAY_24
    printf("Initial Map:\n");
    display_map(&init_map);
#endif

    build_permutations(&component_permutations, &init_map);
    
    day_24_map_t current_map;
    day_24_map_t next_map;
    
    copy_map(&current_map, &init_map);
    // start the start point at row 0, col 1
    current_map.data[0][1] = PATH;
    
#ifdef DEBUG_DAY_24
    printf("CURRENT MAP AFTER SET:\n");
    display_map(&current_map);
#endif
    // we are searching for last row, second to last column
    int target_row = init_map.num_rows-1;
    int target_col = init_map.num_cols-2;
    
    int num_minutes = 0;
    while (current_map.data[target_row][target_col] != PATH)
    {
        num_minutes++;
        create_map_for_minute(&next_map, &component_permutations, num_minutes);
        expand_paths(&next_map, &current_map);
        copy_map(&current_map, &next_map);
    }
        
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", num_minutes);

    return;
}

void day_24_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_24_map_t init_map;
    day_24_map_component_permutations_t component_permutations;
    
    read_and_parse_input(filename, &init_map);
    
#ifdef DEBUG_DAY_24
    printf("Initial Map:\n");
    display_map(&init_map);
#endif

    build_permutations(&component_permutations, &init_map);
    
    day_24_map_t current_map;
    day_24_map_t next_map;
    
    copy_map(&current_map, &init_map);
    // start the start point at row 0, col 1
    current_map.data[0][1] = PATH;
    
    // we are searching for last row, second to last column
    int target_row = init_map.num_rows-1;
    int target_col = init_map.num_cols-2;
    
    int num_minutes = 0;
    while (current_map.data[target_row][target_col] != PATH)
    {
        num_minutes++;
        create_map_for_minute(&next_map, &component_permutations, num_minutes);
        expand_paths(&next_map, &current_map);
        copy_map(&current_map, &next_map);
    }
    
    // now go back to the start
    create_map_for_minute(&current_map, &component_permutations, num_minutes);
    current_map.data[target_row][target_col] = PATH;
    while (current_map.data[0][1] != PATH)
    {
        num_minutes++;
        create_map_for_minute(&next_map, &component_permutations, num_minutes);
        expand_paths(&next_map, &current_map);
        copy_map(&current_map, &next_map);
    }
    
    
    // now go back to the finish
    create_map_for_minute(&current_map, &component_permutations, num_minutes);
    current_map.data[0][1] = PATH;
    while (current_map.data[target_row][target_col] != PATH)
    {
        num_minutes++;
        create_map_for_minute(&next_map, &component_permutations, num_minutes);
        expand_paths(&next_map, &current_map);
        copy_map(&current_map, &next_map);
    }
    
    
        
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", num_minutes);

    return;
}
