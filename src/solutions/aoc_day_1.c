#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_1.h"
#include "file_utils.h"

int read_and_parse_input(char * filename, day_1_elves_t * elves)
{
    file_data_t fd;
    
    // initialize the data structures
    int current_elf = 0;
    elves->num_elves = 0;
    elves->elves[current_elf].num_calories = 0;
    
    // read in the input file; no delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }
    
#ifdef DEBUG_DAY_1
    printf("Starting with elf %d\n", current_elf);
#endif

    int line_number = 0;
    line_data_t * current = fd.head_line;
    while (current != NULL)
    {
        line_number++;
        // should have 1 token for each line, even a blank line. the blank line will have a length of 0
        if (current->num_tokens == 1)
        {
            token_data_t * td = current->head_token;
            if (strlen(td->token) > 0)
            {
                // add this calories value to the current elf
                if (elves->elves[current_elf].num_calories >= MAX_DAY_1_CALORIES)
                {
                    fprintf(stderr, "!!! Maximum Number of Calories Per Elf Exceeded! Increase MAX_DAY_1_CALORIES!\n");
                    file_data_cleanup(&fd);
                    return FALSE;
                }
                elves->elves[current_elf].calories[elves->elves[current_elf].num_calories] = strtol(td->token, NULL, 10);
#ifdef DEBUG_DAY_1
                printf("Added %d calories to elf %d at %d from line %d\n", elves->elves[current_elf].calories[elves->elves[current_elf].num_calories],                                                                        current_elf, elves->elves[current_elf].num_calories, line_number);
#endif
                elves->elves[current_elf].num_calories++;
            }
            else
            {
                // blank line - switch to the next elf.
                current_elf++;
                if (current_elf >= MAX_DAY_1_ELVES)
                {
                    fprintf(stderr, "!!! Maximum Number of Calories Per Elf Exceeded! Increase MAX_DAY_1_CALORIES!\n");
                    file_data_cleanup(&fd);
                    return FALSE;
                }
#ifdef DEBUG_DAY_1
                printf("Switching to elf %d at line %d\n", current_elf, line_number);
#endif
                elves->elves[current_elf].num_calories = 0;            
            }
        }
        else
        {
            fprintf(stderr, "Invalid number of tokens %d at line %d\n", current->num_tokens, line_number);
            file_data_cleanup(&fd);
            return FALSE;
        }
        current = current->next;
    }
    elves->num_elves = current_elf + 1;
    file_data_cleanup(&fd);
    return TRUE;
}

void day_1_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_1_elves_t elves;
    long largest_calories = 0;
    long current_calories;
    if (read_and_parse_input(filename, &elves) == FALSE)
    {
        fprintf(stderr, "Error reading in the data from %s\n", filename);
        return;
    }
    for (int i=0; i<elves.num_elves; i++)
    {
        current_calories = 0;
        for (int j=0; j<elves.elves[i].num_calories; j++)
        {
            current_calories += elves.elves[i].calories[j];
        }
#ifdef DEBUG_DAY_1
        printf("Elf %d has %ld total calories\n", i, current_calories); 
#endif
        if (current_calories > largest_calories)
        {
#ifdef DEBUG_DAY_1
        printf("  This is the new largest calories replacing value of %ld\n", largest_calories); 
#endif
            largest_calories = current_calories;
        }
    }
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", largest_calories);
    return;
}

void day_1_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_1_elves_t elves;
    long largest_calories[3] = {0,0,0};
    long current_calories;
    if (read_and_parse_input(filename, &elves) == FALSE)
    {
        fprintf(stderr, "Error reading in the data from %s\n", filename);
        return;
    }
    for (int i=0; i<elves.num_elves; i++)
    {
        current_calories = 0;
        for (int j=0; j<elves.elves[i].num_calories; j++)
        {
            current_calories += elves.elves[i].calories[j];
        }
#ifdef DEBUG_DAY_1
        printf("Elf %d has %ld total calories\n", i, current_calories); 
#endif
        if (current_calories > largest_calories[0])
        {
            largest_calories[2] = largest_calories[1];
            largest_calories[1] = largest_calories[0];
            largest_calories[0] = current_calories;
#ifdef DEBUG_DAY_1
        printf("  This is the new first largest calories resulting in %ld %ld %ld\n", largest_calories[0], largest_calories[1], largest_calories[2]); 
#endif
        }
        else if (current_calories > largest_calories[1])
        {
            largest_calories[2] = largest_calories[1];
            largest_calories[1] = current_calories;
#ifdef DEBUG_DAY_1
        printf("  This is the new second largest calories resulting in %ld %ld %ld\n", largest_calories[0], largest_calories[1], largest_calories[2]); 
#endif
        }
        else if (current_calories > largest_calories[2])
        {
            largest_calories[2] = current_calories;
#ifdef DEBUG_DAY_1
        printf("  This is the new third largest calories resulting in %ld %ld %ld\n", largest_calories[0], largest_calories[1], largest_calories[2]); 
#endif
        }
    }
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", largest_calories[0] + largest_calories[1] + largest_calories[2]);
    return;
}
