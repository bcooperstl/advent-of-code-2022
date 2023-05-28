#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_21.h"
#include "file_utils.h"

#define PART_1_TARGET_MONKEY "root"

static void read_and_parse_input(char * filename, day_21_monkeys_t * monkeys)
{
    file_data_t fd;
    
    // read in the input file; space delimiters
    
    file_data_init(&fd);
    file_data_read_file(&fd, filename, " ", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }
    
    monkeys->num_monkeys = 0;
    line_data_t * ld = fd.head_line;
    
    while (ld != NULL)
    {
        /* 2 formats:
        operation is [root: pppw + sjmn]
        value is     [dbpl: 5]
        */

        token_data_t * td = ld->head_token;
        
        strncpy(monkeys->monkeys[monkeys->num_monkeys].own_name, td->token, DAY_21_MONKEY_NAME_LENGTH+1);
        monkeys->monkeys[monkeys->num_monkeys].own_name[DAY_21_MONKEY_NAME_LENGTH]='\0'; // get rid of the trailing : character; needs to be null-terminated anyways
        
        if (ld->num_tokens == 2) // value line
        {
            monkeys->monkeys[monkeys->num_monkeys].type = DAY_21_MONKEY_IS_VALUE;
            monkeys->monkeys[monkeys->num_monkeys].is_evaluated = TRUE;
            td = td->next; // advance to value
            monkeys->monkeys[monkeys->num_monkeys].value = strtoll(td->token, NULL, 10);
#ifdef DEBUG_DAY_21
            printf("Monkey %d named %s has value %lld\n", monkeys->num_monkeys, monkeys->monkeys[monkeys->num_monkeys].own_name, monkeys->monkeys[monkeys->num_monkeys].value);
#endif
        }
        else if (ld->num_tokens == 4)
        {
            monkeys->monkeys[monkeys->num_monkeys].type = DAY_21_MONKEY_IS_OPERATION;
            monkeys->monkeys[monkeys->num_monkeys].is_evaluated = FALSE;
            monkeys->monkeys[monkeys->num_monkeys].operand_indices[0] = -1;
            monkeys->monkeys[monkeys->num_monkeys].operand_indices[1] = -1;
            
            td = td->next; // advance to first operand
            strncpy(monkeys->monkeys[monkeys->num_monkeys].operand_names[0], td->token, DAY_21_MONKEY_NAME_LENGTH+1);
            
            td = td->next; // advance to operation
            monkeys->monkeys[monkeys->num_monkeys].operation = td->token[0];

            td = td->next; // advance to second operand
            strncpy(monkeys->monkeys[monkeys->num_monkeys].operand_names[1], td->token, DAY_21_MONKEY_NAME_LENGTH+1);
            
#ifdef DEBUG_DAY_21
            printf("Monkey %d named %s has operation %s %c %s\n", monkeys->num_monkeys, monkeys->monkeys[monkeys->num_monkeys].own_name, 
                                                                  monkeys->monkeys[monkeys->num_monkeys].operand_names[0],
                                                                  monkeys->monkeys[monkeys->num_monkeys].operation,
                                                                  monkeys->monkeys[monkeys->num_monkeys].operand_names[1]);
#endif
        }
        else
        {
            fprintf(stderr, "Invalid number of tokens %d given for monkey %s\n", ld->num_tokens, monkeys->monkeys[monkeys->num_monkeys].own_name);
            return;
        }
        monkeys->num_monkeys++;
        ld = ld->next;
    }
    
#ifdef DEBUG_DAY_21
    printf("There were %d monkeys in the file\n", monkeys->num_monkeys);
#endif
    file_data_cleanup(&fd);
    
    return;
}

static void fixup_operation_pointers(day_21_monkeys_t * monkeys)
{
    for (int i=0; i<monkeys->num_monkeys; i++)
    {
#ifdef DEBUG_DAY_21
        printf("Searching for operands that correspond to monkey %d (%s)\n", i, monkeys->monkeys[i].own_name);
#endif
        for (int j=0; j<monkeys->num_monkeys; j++)
        {
            if (monkeys->monkeys[j].type == DAY_21_MONKEY_IS_OPERATION)
            {
                if (strncmp(monkeys->monkeys[i].own_name, monkeys->monkeys[j].operand_names[0], DAY_21_MONKEY_NAME_LENGTH) == 0)
                {
                    monkeys->monkeys[j].operand_indices[0] = i;
#ifdef DEBUG_DAY_21
                    printf("  Assigned to monkey %d (%s) operand 0\n", j, monkeys->monkeys[j].own_name);
#endif
                }
                if (strncmp(monkeys->monkeys[i].own_name, monkeys->monkeys[j].operand_names[1], DAY_21_MONKEY_NAME_LENGTH) == 0)
                {
                    monkeys->monkeys[j].operand_indices[1] = i;
#ifdef DEBUG_DAY_21
                    printf("  Assigned to monkey %d (%s) operand 1\n", j, monkeys->monkeys[j].own_name);
#endif
                }
            }
        }
    }        
}

static long long get_value(day_21_monkeys_t * monkeys, int monkey_index)
{
    if (monkeys->monkeys[monkey_index].is_evaluated == TRUE)
    {
        return monkeys->monkeys[monkey_index].value;
    }
    
    if (monkeys->monkeys[monkey_index].type == DAY_21_MONKEY_IS_OPERATION)
    {
        long long left = get_value(monkeys, monkeys->monkeys[monkey_index].operand_indices[0]);
        long long right = get_value(monkeys, monkeys->monkeys[monkey_index].operand_indices[1]);
        switch (monkeys->monkeys[monkey_index].operation)
        {
            case '+':
                monkeys->monkeys[monkey_index].value = left + right;
                break;
            case '-':
                monkeys->monkeys[monkey_index].value = left - right;
                break;
            case '*':
                monkeys->monkeys[monkey_index].value = left * right;
                break;
            case '/':
                monkeys->monkeys[monkey_index].value = left / right;
                break;
            default:
                fprintf(stderr, "Invalid operation %c for monkey %d (%s)\n", monkeys->monkeys[monkey_index].operation, monkey_index, monkeys->monkeys[monkey_index].own_name);
                monkeys->monkeys[monkey_index].value = 0;
                break;
        }
#ifdef DEBUG_DAY_21
        printf("Monkey %d (%s) has calculated value of %s (%lld) %c %s (%lld) = %lld\n", monkey_index, monkeys->monkeys[monkey_index].own_name,
                                                                              monkeys->monkeys[monkey_index].operand_names[0], left, 
                                                                              monkeys->monkeys[monkey_index].operation,
                                                                              monkeys->monkeys[monkey_index].operand_names[1], right,
                                                                              monkeys->monkeys[monkey_index].value);
#endif
    }
    else
    {
        fprintf(stderr, "***Check logic - should not have non-evaluated value type monkey***\n");
    }
    return monkeys->monkeys[monkey_index].value;    
}


void day_21_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_21_monkeys_t monkeys;
    
    read_and_parse_input(filename, &monkeys);
    
    fixup_operation_pointers(&monkeys);
    
    int root_index = 0;
    for (int i=0; i<monkeys.num_monkeys; i++)
    {
        if (strncmp(monkeys.monkeys[i].own_name, PART_1_TARGET_MONKEY, DAY_21_MONKEY_NAME_LENGTH) == 0)
        {
            root_index = i;
            break;
        }
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%lld", get_value(&monkeys, root_index));
    
    return;
}
