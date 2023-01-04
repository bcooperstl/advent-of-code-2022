#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_11.h"
#include "file_utils.h"

static int parse_input(char * filename, day_11_monkeys_t * monkeys)
{
    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with no delimiteres
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }
    
    int num_monkeys = (fd.num_lines + 1) / 7;

#ifdef DEBUG_DAY_11_PARSE
    printf("The input file has %d lines, which means there are %d monkeys\n", fd.num_lines, num_monkeys);
#endif

    ld = fd.head_line;
    td = ld->head_token;
    
    for (int monkey_idx=0; monkey_idx < num_monkeys; monkey_idx++)
    {   
#ifdef DEBUG_DAY_11_PARSE
        printf("Parsing Monkey %d\n", monkey_idx);
#endif
        // skip the Monkey line; corresponds to index
        ld = ld->next;
        
        // items start at position 18 and are all 2-digit numbers
        td = ld->head_token;
        monkeys->monkeys[monkey_idx].num_items = ((strlen(td->token) - 18) / 4) + 1;
#ifdef DEBUG_DAY_11_PARSE
        printf("  There are %d items\n", monkeys->monkeys[monkey_idx].num_items);
#endif
        for (int item_idx=0; item_idx < monkeys->monkeys[monkey_idx].num_items; item_idx++)
        {
            monkeys->monkeys[monkey_idx].items[item_idx] = strtol(td->token+18+(4*item_idx), NULL, 10);
#ifdef DEBUG_DAY_11_PARSE
            printf("    Item %d is %d\n", item_idx, monkeys->monkeys[monkey_idx].items[item_idx]);
#endif
        }
        ld = ld->next;
        // operation_type and operation_constant
        td = ld->head_token;
        if (strstr(td->token, "old * old") != NULL)
        {
            monkeys->monkeys[monkey_idx].operation_type = DAY11_OP_OLD_SQUARED;
#ifdef DEBUG_DAY_11_PARSE
            printf("  The operation is old * old\n");
#endif
        }
        else if (td->token[23] == '*')
        {
            monkeys->monkeys[monkey_idx].operation_type = DAY11_OP_OLD_MULTIPLY;
            monkeys->monkeys[monkey_idx].operation_constant = strtol(td->token+25, NULL, 10);
#ifdef DEBUG_DAY_11_PARSE
            printf("  The operation is old * %d\n", monkeys->monkeys[monkey_idx].operation_constant);
#endif
        }
        else if (td->token[23] == '+')
        {
            monkeys->monkeys[monkey_idx].operation_type = DAY11_OP_OLD_ADD;
            monkeys->monkeys[monkey_idx].operation_constant = strtol(td->token+25, NULL, 10);
#ifdef DEBUG_DAY_11_PARSE
            printf("  The operation is old + %d\n", monkeys->monkeys[monkey_idx].operation_constant);
#endif
        }
        else 
        {
            fprintf(stderr, "Invalid operation for monkey %d; line is [%s]\n", monkey_idx, td->token);
            file_data_cleanup(&fd);
            return FALSE;
        }
        ld = ld->next;

        // test modulus at position 21
        td = ld->head_token;
        monkeys->monkeys[monkey_idx].test_modulus = strtol(td->token+21, NULL, 10);
#ifdef DEBUG_DAY_11_PARSE
            printf("  The test modulus is %d\n", monkeys->monkeys[monkey_idx].test_modulus);
#endif
        ld = ld->next;
        
        // true destination at position 29
        td = ld->head_token;
        monkeys->monkeys[monkey_idx].true_destination = strtol(td->token+29, NULL, 10);
#ifdef DEBUG_DAY_11_PARSE
            printf("  The true destination monkey is %d\n", monkeys->monkeys[monkey_idx].true_destination);
#endif
        ld = ld->next;
        
        // false destination at position 30
        td = ld->head_token;
        monkeys->monkeys[monkey_idx].false_destination = strtol(td->token+30, NULL, 10);
#ifdef DEBUG_DAY_11_PARSE
            printf("  The false destination monkey is %d\n", monkeys->monkeys[monkey_idx].false_destination);
#endif
        if (ld->next != NULL)
        {
            ld = ld->next;
        }
        
        // skip the blank line (if we aren't already at the endif
        if (ld->next != NULL)
        {
            ld = ld->next;
        }
        // init the inspection count to 0
        monkeys->monkeys[monkey_idx].inspection_count = 0;
    }
    
    file_data_cleanup(&fd);
    
    return TRUE;
}

void day_11_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_11_monkeys_t monkeys;
    
    if (parse_input(filename, &monkeys) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    int num_visible = 0;
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", num_visible);
    
    return;
}

