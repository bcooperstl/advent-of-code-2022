#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_2.h"
#include "file_utils.h"

#define ELF_ROCK 'A'
#define ELF_PAPER 'B'
#define ELF_SCISSORS 'C'
#define ME_ROCK 'X'
#define ME_PAPER 'Y'
#define ME_SCISSORS 'Z'
#define OUTCOME_LOSE 'X'
#define OUTCOME_DRAW 'Y'
#define OUTCOME_WIN 'Z'


static int points_for_round_elf_me(char elf, char me)
{
    if (elf == ELF_ROCK && me == ME_ROCK)
    {
        return 4;
    }
    else if (elf == ELF_ROCK && me == ME_PAPER)
    {
        return 8;
    }
    else if (elf == ELF_ROCK && me == ME_SCISSORS)
    {
        return 3;
    }
    else if (elf == ELF_PAPER && me == ME_ROCK)
    {
        return 1;
    }
    else if (elf == ELF_PAPER && me == ME_PAPER)
    {
        return 5;
    }
    else if (elf == ELF_PAPER && me == ME_SCISSORS)
    {
        return 9;
    }
    else if (elf == ELF_SCISSORS && me == ME_ROCK)
    {
        return 7;
    }
    else if (elf == ELF_SCISSORS && me == ME_PAPER)
    {
        return 2;
    }
    else if (elf == ELF_SCISSORS && me == ME_SCISSORS)
    {
        return 6;
    }
    else
    {
        fprintf(stderr, "!!!INVALID COMBINATION OF %c and %c\n", elf, me);
        return 0;
    }
}

static int points_for_round_elf_outcome(char elf, char outcome)
{
    if (elf == ELF_ROCK && outcome == OUTCOME_LOSE)
    {
        return 3;
    }
    else if (elf == ELF_ROCK && outcome == OUTCOME_DRAW)
    {
        return 4;
    }
    else if (elf == ELF_ROCK && outcome == OUTCOME_WIN)
    {
        return 8;
    }
    else if (elf == ELF_PAPER && outcome == OUTCOME_LOSE)
    {
        return 1;
    }
    else if (elf == ELF_PAPER && outcome == OUTCOME_DRAW)
    {
        return 5;
    }
    else if (elf == ELF_PAPER && outcome == OUTCOME_WIN)
    {
        return 9;
    }
    else if (elf == ELF_SCISSORS && outcome == OUTCOME_LOSE)
    {
        return 2;
    }
    else if (elf == ELF_SCISSORS && outcome == OUTCOME_DRAW)
    {
        return 6;
    }
    else if (elf == ELF_SCISSORS && outcome == OUTCOME_WIN)
    {
        return 7;
    }
    else
    {
        fprintf(stderr, "!!!INVALID COMBINATION OF %c and %c\n", elf, outcome);
        return 0;
    }
}

void day_2_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    int sum = 0;
    file_data_t fd;
    
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
            if (strlen(td->token) == 3)
            {
                int curr = points_for_round_elf_me(td->token[0], td->token[2]);
                sum += curr;
#ifdef DEBUG_DAY2
                printf("Line %d has elf %c me %c resulting in %d points raising total to %d\n", line_number, td->token[0], td->token[2], curr, sum);
#endif
            }
            else
            {
                fprintf(stderr, "Invalid line length at line %d [%s]\n", strlen(td->token), td->token);
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

void day_2_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    int sum = 0;
    file_data_t fd;
    
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
        // should have 1 token for each line of format [A X] where A is elf and X is outcome
        if (current->num_tokens == 1)
        {
            token_data_t * td = current->head_token;
            if (strlen(td->token) == 3)
            {
                int curr = points_for_round_elf_outcome(td->token[0], td->token[2]);
                sum += curr;
#ifdef DEBUG_DAY2
                printf("Line %d has elf %c outcome %c resulting in %d points raising total to %d\n", line_number, td->token[0], td->token[2], curr, sum);
#endif
            }
            else
            {
                fprintf(stderr, "Invalid line length at line %d [%s]\n", strlen(td->token), td->token);
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
