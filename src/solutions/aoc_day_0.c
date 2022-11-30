#include <stdio.h>
#include <stdlib.h>

#include "aoc_solutions.h"
#include "aoc_day_0.h"
#include "file_utils.h"

int read_input(char * filename, day0_values_t * values)
{
    file_data_t fd;
    values->num_values = 0;
    
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }
    
    line_data_t * current = fd.head_line;
    int line_number = 0;
    while (current != NULL)
    {
        line_number++;
        if (current->num_tokens != 1)
        {
            fprintf(stderr, "Invalid number of tokens %d at line %d\n", current->num_tokens, line_number);
            current = current->next;
            continue;
        }
        token_data_t * td = current->head_token;
        values->values[values->num_values] = strtol(td->token, NULL, 10);
        values->num_values++;
        current = current->next;
    }
    file_data_cleanup(&fd);
    return TRUE;
}

void day0_part1(char * filename, extra_args_t * extra_args, char * result)
{
    printf("in part 1\n");
    long sum = 0;
    day0_values_t values;
    if (read_input(filename, &values) == FALSE)
    {
        fprintf(stderr, "Error reading in the data from %s\n", filename);
        return;
    }
    for (int i=0; i<values.num_values; i++)
    {
        sum+=values.values[i];
    }
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", sum);
    return;
}

void day0_part2(char * filename, extra_args_t * extra_args, char * result)
{
    printf("in part 2\n");

    if (extra_args->num_extra_args > 0)
    {
        printf("There are %d extra arguments given:\n", extra_args->num_extra_args);
        for (int i=0; i<extra_args->num_extra_args; i++)
        {
            printf("[%s]\n", extra_args->extra_args[i]);
        }
    }

    long sum = 0;
    day0_values_t values;
    if (read_input(filename, &values) == FALSE)
    {
        fprintf(stderr, "Error reading in the data from %s\n", filename);
        return;
    }
    for (int i=0; i<values.num_values; i++)
    {
        sum-=values.values[i];
    }
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", sum);
    return;
}
