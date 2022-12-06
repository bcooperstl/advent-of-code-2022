#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_5.h"
#include "file_utils.h"

static void display_stacks(day_5_stacks_t * stacks)
{
    for (int i=0; i<stacks->num_stacks; i++)
    {
        printf("%d| %s\n", i+1, stacks->crates[i]);
    }
    printf("\n");
}

static int read_and_parse_input(char * filename, day_5_stacks_t * stacks, day_5_moves_t * moves)
{
    file_data_t fd_no_delim;
    line_data_t * line_no_delim;
    token_data_t * token_no_delim;
    
    file_data_t fd_delim;
    line_data_t * line_delim;
    token_data_t * token_delim;
    
    int max_stack_size, curr_stack_size;
    
    // read in the input file with no delimiters
    file_data_init(&fd_no_delim);
    file_data_read_file(&fd_no_delim, filename, "", 0, '\0', '\0');
    if (fd_no_delim.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd_no_delim);
        return FALSE;
    }

    // read in the input file again with space delimiters
    file_data_init(&fd_delim);
    file_data_read_file(&fd_delim, filename, " ", 1, '\0', '\0');
    if (fd_delim.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd_delim);
        file_data_cleanup(&fd_no_delim);
        return FALSE;
    }

    // initialize data structures
    stacks->num_stacks = 0;
    for (int i=0; i<MAX_DAY_5_STACKS; i++)
    {
        memset(stacks->crates[i], '\0', MAX_DAY_5_STACK_DEPTH + 1);
    }
    
    moves->num_moves = 0;
    
    // determine number of stacks
    line_no_delim = fd_no_delim.head_line;
    token_no_delim = line_no_delim->head_token;
    stacks->num_stacks = (strlen(token_no_delim->token) + 1)/4;
#ifdef DEBUG_DAY_5
    printf("The input file has %d stacks\n", stacks->num_stacks);
#endif
    
    // find the highest stack depth
    line_delim = fd_delim.head_line;
    
    max_stack_size = 0;
    
    token_delim = line_delim->head_token->next;
    while (token_delim->token[0] != '1')
    {
#ifdef DEBUG_DAY_5
        printf("Input token is [%s]\n", token_delim->token);
#endif
        max_stack_size++;
        line_delim = line_delim->next;
        token_delim = line_delim->head_token->next;
    }
#ifdef DEBUG_DAY_5
    printf("The maximum stack depth is %d\n", max_stack_size);
#endif

    // load the values to the stacks
    for (curr_stack_size = max_stack_size; curr_stack_size > 0; curr_stack_size--)
    {
        for (int col=0; col < stacks->num_stacks; col++)
        {
            if (token_no_delim->token[4*col+1] != ' ')
            {
                stacks->crates[col][curr_stack_size-1] = token_no_delim->token[4*col+1];
#ifdef DEBUG_DAY_5
                printf("Set stack %d depth %d to %c\n", col+1, curr_stack_size-1, stacks->crates[col][curr_stack_size-1]);
#endif
            }
        }
        line_no_delim = line_no_delim->next;
        token_no_delim = line_no_delim->head_token;
    }

#ifdef DEBUG_DAY_5
    printf("After loading, the current stacks are:\n");
    display_stacks(stacks);
#endif
    
    // load the moves; first have to advance line_delim twice to get to first move
    line_delim = line_delim->next->next;
    
    while (line_delim != NULL)
    {
        // count from col 2
        token_delim = line_delim->head_token->next;
        moves->moves[moves->num_moves].count = strtol(token_delim->token, NULL, 10);
        // 'from' from col 4
        token_delim = token_delim->next->next;
        moves->moves[moves->num_moves].from = strtol(token_delim->token, NULL, 10);
        // 'to' from col 6
        token_delim = token_delim->next->next;
        moves->moves[moves->num_moves].to = strtol(token_delim->token, NULL, 10);
#ifdef DEBUG_DAY_5
        printf("Move %d is moving %d crates from %d to %d\n", moves->num_moves+1, moves->moves[moves->num_moves].count, moves->moves[moves->num_moves].from, moves->moves[moves->num_moves].to);
#endif
        moves->num_moves++;
        line_delim = line_delim->next;
    }
    
    file_data_cleanup(&fd_no_delim);
    file_data_cleanup(&fd_delim);
    
    return TRUE;
}

void day_5_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    int sum = 0;
    file_data_t fd;
    day_5_stacks_t stacks;
    day_5_moves_t moves;
    
    if (read_and_parse_input(filename, &stacks, &moves) != TRUE)
    {
        fprintf(stderr, "!! Error parsing input\n");
        return;
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", 5);
    return;
}
