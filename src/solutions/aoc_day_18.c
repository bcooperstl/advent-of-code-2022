#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_18.h"
#include "file_utils.h"

#define EMPTY ' '
#define DROP '#'
#define AIR '.'

static void init_cubes(day_18_cubes_t * cube)
{
    for (int x=0; x<DAY_18_ALLOC; x++)
    {
        for (int y=0; y<DAY_18_ALLOC; y++)
        {
            for (int z=0; z<DAY_18_ALLOC; z++)
            {
                cube->world[x][y][z] = EMPTY;
            }
        }
    }
}

static void init_cubes_with_air(day_18_cubes_t * cube)
{
    init_cubes(cube);
    for (int x=0; x<DAY_18_SIZE; x++)
    {
        for (int y=0; y<DAY_18_SIZE; y++)
        {
            for (int z=0; z<DAY_18_SIZE; z++)
            {
                cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = AIR;
            }
        }
    }
}

static void add_drop(day_18_cubes_t * cube, int x, int y, int z)
{
    cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = DROP;
}

static int count_exposed_sides(day_18_cubes_t * cube)
{
    int sum = 0;
    for (int x=0; x<DAY_18_SIZE; x++)
    {
        for (int y=0; y<DAY_18_SIZE; y++)
        {
            for (int z=0; z<DAY_18_SIZE; z++)
            {
                if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] == DROP)
                {
                    // drop at x,y,z. Add 1 for each of the size sides that is not a drop
#ifdef DEBUG_DAY_18
                    printf("Drop found at %d,%d,%d\n", x, y, z);
#endif

                    if (cube->world[x+DAY_18_OFFSET-1][y+DAY_18_OFFSET][z+DAY_18_OFFSET] == EMPTY)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Edge found and added at %d,%d,%d\n", x-1, y, z);
#endif
                        sum+=1;
                    }
                    if (cube->world[x+DAY_18_OFFSET+1][y+DAY_18_OFFSET][z+DAY_18_OFFSET] == EMPTY)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Edge found and added at %d,%d,%d\n", x+1, y, z);
#endif
                        sum+=1;
                    }
                    if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET-1][z+DAY_18_OFFSET] == EMPTY)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Edge found and added at %d,%d,%d\n", x, y-1, z);
#endif
                        sum+=1;
                    }
                    if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET+1][z+DAY_18_OFFSET] == EMPTY)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Edge found and added at %d,%d,%d\n", x, y+1, z);
#endif
                        sum+=1;
                    }
                    if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET-1] == EMPTY)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Edge found and added at %d,%d,%d\n", x, y, z-1);
#endif
                        sum+=1;
                    }
                    if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET+1] == EMPTY)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Edge found and added at %d,%d,%d\n", x, y, z+1);
#endif
                        sum+=1;
                    }
                }
            }
        }
    }
    return sum;
}

void day_18_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_18_cubes_t cubes;
    
    init_cubes(&cubes);

    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with comma delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, ",", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }

    ld = fd.head_line;
    while (ld != NULL)
    {
        if (ld->num_tokens != 3)
        {
            fprintf(stderr, "***Invalid number %d of tokens\n", ld->num_tokens);
            break;
        }
        td = ld->head_token;
        int x, y, z;
        x = strtol(td->token, NULL, 10);
        td = td->next;
        y = strtol(td->token, NULL, 10);
        td = td->next;
        z = strtol(td->token, NULL, 10);
#ifdef DEBUG_DAY_18
        printf("Turning on drop %d,%d,%d\n", x, y, z);
#endif
        add_drop(&cubes, x, y, z);
        ld = ld->next;
    }
    
    file_data_cleanup(&fd);
    
    int num_edges = count_exposed_sides(&cubes);
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", num_edges);
    
    return;
}


static void expand_empty_to_air(day_18_cubes_t * cube)
{
    int num_expanded = 0;
    do
    {
#ifdef DEBUG_DAY_18
        printf("New round of expanded empty to air\n");
#endif
        num_expanded = 0;
        for (int x=0; x<DAY_18_SIZE; x++)
        {
            for (int y=0; y<DAY_18_SIZE; y++)
            {
                for (int z=0; z<DAY_18_SIZE; z++)
                {
                    if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] == AIR)
                    {
#ifdef DEBUG_DAY_18
                        printf(" Air found at %d,%d,%d\n", x, y, z);
#endif

                        if (cube->world[x+DAY_18_OFFSET-1][y+DAY_18_OFFSET][z+DAY_18_OFFSET] == EMPTY)
                        {
#ifdef DEBUG_DAY_18
                            printf("  Empty found at %d,%d,%d\n", x-1, y, z);
#endif
                            cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = EMPTY;
                            num_expanded++;
                        }
                        else if (cube->world[x+DAY_18_OFFSET+1][y+DAY_18_OFFSET][z+DAY_18_OFFSET] == EMPTY)
                        {
#ifdef DEBUG_DAY_18
                            printf("  Empty found at %d,%d,%d\n", x+1, y, z);
#endif
                            cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = EMPTY;
                            num_expanded++;
                        }
                        else if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET-1][z+DAY_18_OFFSET] == EMPTY)
                        {
#ifdef DEBUG_DAY_18
                            printf("  Empty found at %d,%d,%d\n", x, y-1, z);
#endif
                            cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = EMPTY;
                            num_expanded++;
                        }
                        else if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET+1][z+DAY_18_OFFSET] == EMPTY)
                        {
#ifdef DEBUG_DAY_18
                            printf("  Empty found at %d,%d,%d\n", x, y+1, z);
#endif
                            cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = EMPTY;
                            num_expanded++;
                        }
                        else if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET-1] == EMPTY)
                        {
#ifdef DEBUG_DAY_18
                            printf("  Empty found at %d,%d,%d\n", x, y, z-1);
#endif
                            cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = EMPTY;
                            num_expanded++;
                        }
                        else if (cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET+1] == EMPTY)
                        {
#ifdef DEBUG_DAY_18
                            printf("  Empty found at %d,%d,%d\n", x, y, z+1);
#endif
                            cube->world[x+DAY_18_OFFSET][y+DAY_18_OFFSET][z+DAY_18_OFFSET] = EMPTY;
                            num_expanded++;
                        }
                    }
                }
            }
        }
    } while (num_expanded != 0);

    return;
}

void day_18_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_18_cubes_t cubes;
    
    init_cubes_with_air(&cubes);

    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with comma delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, ",", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }

    ld = fd.head_line;
    while (ld != NULL)
    {
        if (ld->num_tokens != 3)
        {
            fprintf(stderr, "***Invalid number %d of tokens\n", ld->num_tokens);
            break;
        }
        td = ld->head_token;
        int x, y, z;
        x = strtol(td->token, NULL, 10);
        td = td->next;
        y = strtol(td->token, NULL, 10);
        td = td->next;
        z = strtol(td->token, NULL, 10);
#ifdef DEBUG_DAY_18
        printf("Turning on drop %d,%d,%d\n", x, y, z);
#endif
        add_drop(&cubes, x, y, z);
        ld = ld->next;
    }
    
    file_data_cleanup(&fd);
    
    expand_empty_to_air(&cubes);
    
    int num_edges = count_exposed_sides(&cubes);
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", num_edges);
    
    return;
}
