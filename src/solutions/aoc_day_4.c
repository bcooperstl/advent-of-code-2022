#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_4.h"
#include "file_utils.h"

static int check_contains(int first_low, int first_high, int second_low, int second_high)
{
    // check if first contains second
    if ((first_low <= second_low) && (first_high >= second_high))
    {
#ifdef DEBUG_DAY_4
        printf("The first region of %d-%d contains the second region of %d-%d\n", first_low, first_high, second_low, second_high);
#endif
        return TRUE;
    }
    // check if second contains first
    if ((second_low <= first_low) && (second_high >= first_high))
    {
#ifdef DEBUG_DAY_4
        printf("The second region of %d-%d contains the first region of %d-%d\n", second_low, second_high, first_low, first_high);
#endif
        return TRUE;
    }
#ifdef DEBUG_DAY_4
    printf("Neither first region of %d-%d or second region of %d-%d contain each other\n", first_low, first_high, second_low, second_high);
#endif
    return FALSE;
}

void day_4_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    int count = 0;
    file_data_t fd;
    
    int first_low, first_high, second_low, second_high;
    
    // read in the input file; no delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "-,", 2, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }
    
    int line_number = 0;
    line_data_t * current = fd.head_line;
    while (current != NULL)
    {
        line_number++;
        // should have 1 token for each line of format [1-2,3-4]
        if (current->num_tokens == 4)
        {
            token_data_t * td = current->head_token;
            first_low = strtol(td->token, NULL, 10);
            td = td->next;
            first_high = strtol(td->token, NULL, 10);
            td = td->next;
            second_low = strtol(td->token, NULL, 10);
            td = td->next;
            second_high = strtol(td->token, NULL, 10);
            
#ifdef DEBUG_DAY_4
            printf("Line %d has first region %d-%d and second region %d-%d\n", line_number, first_low, first_high, second_low, second_high);
#endif
        }
        else
        {
            fprintf(stderr, "Invalid number of tokens %d at line %d\n", current->num_tokens, line_number);
            file_data_cleanup(&fd);
            return;
        }
        
        if (check_contains(first_low, first_high, second_low, second_high) == TRUE)
        {
            count++;
        }
        
        current = current->next;
    }

    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", count);
    return;
}
