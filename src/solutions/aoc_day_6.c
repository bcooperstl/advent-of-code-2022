#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_6.h"
#include "file_utils.h"

void day_6_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    int marker = 0;
    file_data_t fd;
    
    // read in the input file; no delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "-,", 2, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }
    
    line_data_t * ld = fd.head_line;
    token_data_t * td = ld->head_token;
    char * input = td->token;
    
    for (int i=3; i<strlen(input); i++)
    {
        if ((input[i-3] != input[i-2]) && // 1st and 2nd
            (input[i-3] != input[i-1]) && // 1st and 3rd
            (input[i-3] != input[i]) && // 1st and 4th
            (input[i-2] != input[i-1]) && // 2nd and 3rd
            (input[i-2] != input[i]) && // 2nd and 4th
            (input[i-1] != input[i])) // 3rd and 4th
        {
            marker = i+1;
            break;
        }
    }
    
    file_data_cleanup(&fd);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", marker);
    return;
}
