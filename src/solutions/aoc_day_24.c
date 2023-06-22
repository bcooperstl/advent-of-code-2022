#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_24.h"
#include "file_utils.h"

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

void day_24_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_24_map_t initial_map;
    
    read_and_parse_input(filename, &initial_map);
    
#ifdef DEBUG_DAY_24
    printf("Initial Map:\n");
    display_map(&initial_map);
#endif

    snprintf(result, MAX_RESULT_LENGTH+1, "%d", 0);

    return;
}
