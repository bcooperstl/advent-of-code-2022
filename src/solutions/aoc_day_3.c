#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_3.h"
#include "file_utils.h"

static int create_rucksack(char * input, day_3_rucksack_t * rucksack)
{
    int full_length = strlen(input);
    if (full_length == 0 || (full_length % 2 != 0))
    {
        fprintf(stderr, "Invalid length of %d for rucksack from [%s]\n", full_length, input);
        return FALSE;
    }
    
    rucksack->compartment_length = full_length / 2;
    memset(rucksack->compartments[0], '\0', MAX_DAY_3_COMPARTMENT_LENGTH+1);
    memset(rucksack->compartments[1], '\0', MAX_DAY_3_COMPARTMENT_LENGTH+1);
    
    strncpy(rucksack->compartments[0], input, rucksack->compartment_length);
    strncpy(rucksack->compartments[1], input+rucksack->compartment_length, rucksack->compartment_length);

#ifdef DEBUG_DAY_3
    printf("Rucksack [%s] is split into [%s] and [%s]\n", input, rucksack->compartments[0], rucksack->compartments[1]);
#endif
    return TRUE;
}

static int get_priority(char ch)
{
    if (ch >= 'a' && ch <= 'z')
    {
        return ch - 'a' + 1;
    }
    else if (ch >= 'A' && ch <= 'Z')
    {
        return ch - 'A' + 27;
    }
    else
    {
        fprintf(stderr, "!!! Invalid character %c checked for prioity!!!\n", ch);
    }
    return 0;
}

static char find_dupe(day_3_rucksack_t * rucksack)
{
    for (int i=0; i<rucksack->compartment_length; i++)
    {
        if (strchr(rucksack->compartments[1], rucksack->compartments[0][i]) != NULL)
        {
            return rucksack->compartments[0][i];
        }
    }
    fprintf(stderr, "!!! No common character found between [%s] and [%s]\n", rucksack->compartments[0], rucksack->compartments[1]);
    return '\0';
}

void day_3_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    int sum = 0;
    file_data_t fd;
    day_3_rucksack_t rucksack;
    
    // read in the input file; no delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
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
        // should have 1 token for each line of format [A X] where A is elf and X is me
        if (current->num_tokens == 1)
        {
            token_data_t * td = current->head_token;
            if (create_rucksack(td->token, &rucksack) == TRUE)
            {
                char match = find_dupe(&rucksack);
                int curr = get_priority(match);
                sum += curr;
#ifdef DEBUG_DAY_3
                printf("Line %d [%s] has matching char %c worth %d priority\n", line_number, td->token, match, curr);
#endif
            }
            else
            {
                fprintf(stderr, "Error creating rucksack at line %d [%s]\n", strlen(td->token), td->token);
                file_data_cleanup(&fd);
                return;
            }
        }
        else
        {
            fprintf(stderr, "Invalid number of tokens %d at line %d\n", current->num_tokens, line_number);
            file_data_cleanup(&fd);
            return;
        }
        current = current->next;
    }

    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", sum);
    return;
}
