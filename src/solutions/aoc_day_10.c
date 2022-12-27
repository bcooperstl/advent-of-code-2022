#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_10.h"
#include "file_utils.h"

#define INPUT_NOOP "noop"
#define INPUT_ADDX "addx"

#define NUM_KEY_CYCLES 6

static int parse_input(char * filename, day_10_instructions_t * instructions)
{
    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with space delimiteres
    file_data_init(&fd);
    file_data_read_file(&fd, filename, " ", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }

    ld = fd.head_line;
    td = ld->head_token;
    
    instructions->num_instructions = 0;
    
    while (ld != NULL)
    {
        td = ld->head_token;
        if (strncmp(td->token, INPUT_NOOP, 4) == 0)
        {
            instructions->instructions[instructions->num_instructions].type = DAY_10_TYPE_NOOP;
        }
        else if (strncmp(td->token, INPUT_ADDX, 4) == 0)
        {
            instructions->instructions[instructions->num_instructions].type = DAY_10_TYPE_ADDX;
            instructions->instructions[instructions->num_instructions].value = strtol(td->next->token, NULL, 10);
        }
        else
        {
            fprintf(stderr, "**Invalid Insturction %s given at number %d\n", td->token, instructions->num_instructions);
        }
        
        ld = ld->next;
        instructions->num_instructions++;
    }

    file_data_cleanup(&fd);
    
    return TRUE;
}

void day_10_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_10_instructions_t instructions;
    
    int x = 1;
    int cycle = 0;
    
    day_10_key_cycle_t key_cycles[NUM_KEY_CYCLES];
    key_cycles[0].cycle_number = 20;
    key_cycles[1].cycle_number = 60;
    key_cycles[2].cycle_number = 100;
    key_cycles[3].cycle_number = 140;
    key_cycles[4].cycle_number = 180;
    key_cycles[5].cycle_number = 220;
    
    if (parse_input(filename, &instructions) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    for (int i=0; i<instructions.num_instructions; i++)
    {
        if (instructions.instructions[i].type == DAY_10_TYPE_NOOP)
        {
            cycle++;
#ifdef DEBUG_DAY_10            
            printf("Instruction %d is NOOP, doing nothing at cycle %d and leaving X at %d\n", i, cycle, x);
#endif
            for (int k=0; k<NUM_KEY_CYCLES; k++)
            {
                if (key_cycles[k].cycle_number == cycle)
                {
                    key_cycles[k].value = x;
#ifdef DEBUG_DAY_10            
                    printf("Key cycle %d occurred; setting value to %d\n", cycle, x);
#endif
                }
            }
        }
        else if (instructions.instructions[i].type == DAY_10_TYPE_ADDX)
        {
            for (int j=0; j<2; j++)
            {
                cycle++;
#ifdef DEBUG_DAY_10            
                printf("Instruction %d is ADDX, using cycle %d\n", i, cycle);
#endif
                for (int k=0; k<NUM_KEY_CYCLES; k++)
                {
                    if (key_cycles[k].cycle_number == cycle)
                    {
                        key_cycles[k].value = x;
#ifdef DEBUG_DAY_10            
                        printf("Key cycle %d occurred; setting value to %d\n", cycle, x);
#endif
                    }
                }
            }
            x+=instructions.instructions[i].value;
#ifdef DEBUG_DAY_10            
            printf("Instruction %d is ADDX, incrementing x by %d to %d after cycles were completed\n", i, instructions.instructions[i].value, x);
#endif
        }
    }
    
    int sum = 0;
    for (int k=0; k<NUM_KEY_CYCLES; k++)
    {
#ifdef DEBUG_DAY_10            
        printf("Adding %d to sum for cycle num %d value %d\n", key_cycles[k].cycle_number * key_cycles[k].value, key_cycles[k].cycle_number, key_cycles[k].value);
#endif
        sum += (key_cycles[k].cycle_number * key_cycles[k].value);
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", sum);
    
    return;
}
