#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_21.h"
#include "file_utils.h"

#define PART_1_TARGET_MONKEY "root"
#define PART_2_EQUAL_MONKEY "root"
#define PART_2_YELL_MONKEY "humn"

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

static void find_yell_dependent(day_21_monkeys_t * monkeys, int yell_index)
{
    for (int i=0; i<monkeys->num_monkeys; i++)
    {
        monkeys->monkeys[i].is_yell_dependant = FALSE;
    }
    
    monkeys->monkeys[yell_index].is_yell_dependant = TRUE;
#ifdef DEBUG_DAY_21
    printf("Setting Yell Dependant Monkeys\n Setting Yell Monkey at %d (%s) as yell dependent\n", yell_index, monkeys->monkeys[yell_index].own_name);
#endif
    int did_something = TRUE;
    while (did_something == TRUE)
    {
        did_something = FALSE;
        for (int i=0; i<monkeys->num_monkeys; i++)
        {
            if (monkeys->monkeys[i].is_yell_dependant == TRUE)
            {
                for (int j=0; j<monkeys->num_monkeys; j++)
                {
                    if (monkeys->monkeys[j].type == DAY_21_MONKEY_IS_OPERATION && monkeys->monkeys[j].is_yell_dependant == FALSE)
                    {
                        for (int k=0; k<2; k++)
                        {
                            int dest_index = monkeys->monkeys[j].operand_indices[k];
                            if (i == dest_index)
                            {
                                monkeys->monkeys[j].is_yell_dependant = TRUE;
#ifdef DEBUG_DAY_21
                                printf(" Setting Monkey at %d (%s) as yell dependent because operand %d is yell dependant monkey %d (%s)\n", j, monkeys->monkeys[j].own_name, k, i, monkeys->monkeys[i].own_name);
#endif
                                did_something = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }
    return;
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
            case '=':
                monkeys->monkeys[monkey_index].value = (left == right ? 1 : 0);
                break;
            default:
                fprintf(stderr, "Invalid operation %c for monkey %d (%s)\n", monkeys->monkeys[monkey_index].operation, monkey_index, monkeys->monkeys[monkey_index].own_name);
                monkeys->monkeys[monkey_index].value = 0;
                break;
        }
        monkeys->monkeys[monkey_index].is_evaluated = TRUE;
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

static void set_non_yell_dependant_values(day_21_monkeys_t * monkeys, int equal_index)
{
    // start with the left and right sides of equal element
#ifdef DEBUG_DAY_21
    printf("Setting non yell-dependant values\n");
#endif
    
    int idx = monkeys->monkeys[equal_index].operand_indices[0];
    if (monkeys->monkeys[idx].is_yell_dependant == TRUE)
    {
#ifdef DEBUG_DAY_21
    printf(" Operand 0 of equal is %d (%s) and is yell-dependant. Skipping this item\n", idx, monkeys->monkeys[idx].own_name);
#endif
    }
    else
    {
#ifdef DEBUG_DAY_21
        printf(" Operand 0 of equal is %d (%s) and is not yell-dependant. Setting from this element\n", idx, monkeys->monkeys[idx].own_name);
#endif
        get_value(monkeys, idx);
    }
    
    idx = monkeys->monkeys[equal_index].operand_indices[1];
    if (monkeys->monkeys[idx].is_yell_dependant == TRUE)
    {
#ifdef DEBUG_DAY_21
    printf(" Operand 1 of equal is %d (%s) and is yell-dependant. Skipping this item\n", idx, monkeys->monkeys[idx].own_name);
#endif
    }
    else
    {
#ifdef DEBUG_DAY_21
        printf(" Operand 1 of equal is %d (%s) and is not yell-dependant. Setting from this element\n", idx, monkeys->monkeys[idx].own_name);
#endif
        get_value(monkeys, idx);
    }
    
    // pick up any stragglers
    for (idx=0; idx<monkeys->num_monkeys; idx++)
    {
        if (monkeys->monkeys[idx].is_yell_dependant == FALSE && monkeys->monkeys[idx].is_evaluated == FALSE)
        {
#ifdef DEBUG_DAY_21
            printf(" Straggler %d (%s) and is not yell-dependant. Setting from this element\n", idx, monkeys->monkeys[idx].own_name);
#endif
            get_value(monkeys, idx);
        }
    }
    return;
}

static void set_and_recurse(day_21_monkeys_t * monkeys, int set_index, long long value)
{
    // terminal case - literal value
    if (monkeys->monkeys[set_index].type == DAY_21_MONKEY_IS_VALUE)
    {
        monkeys->monkeys[set_index].value = value;
#ifdef DEBUG_DAY_21
        printf("Setting value monkey %d (%s) to %lld\n", set_index, monkeys->monkeys[set_index].own_name, value);
#endif
    }
    else // operation
    {
        monkeys->monkeys[set_index].value = value;
#ifdef DEBUG_DAY_21
        printf("Setting operation monkey %d (%s) to %lld\n", set_index, monkeys->monkeys[set_index].own_name, value);
#endif
        int idx[2];
        idx[0] = monkeys->monkeys[set_index].operand_indices[0];
        idx[1] = monkeys->monkeys[set_index].operand_indices[1];
        
        // equality = operation
        if (monkeys->monkeys[set_index].operation == '=')
        {
            // equal operation - need to set the yell-dependant monkey to have the value of the non-yell-dependant monkey
            if (monkeys->monkeys[idx[0]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is =; Monkey %d (%s) is yell-dependant. Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld\n", 
                         idx[0], monkeys->monkeys[idx[0]].own_name, 
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[1]].value,
                         idx[0], monkeys->monkeys[idx[0]].own_name, monkeys->monkeys[idx[1]].value);
#endif
                set_and_recurse(monkeys, idx[0], monkeys->monkeys[idx[1]].value);
            }
            else if (monkeys->monkeys[idx[1]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is =; Monkey %d (%s) is yell-dependant. Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld\n", 
                         idx[1], monkeys->monkeys[idx[1]].own_name, 
                         idx[0], monkeys->monkeys[idx[0]].own_name, monkeys->monkeys[idx[0]].value,
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[0]].value);
#endif
                set_and_recurse(monkeys, idx[1], monkeys->monkeys[idx[0]].value);
            }
        }

        // addition + opereation
        if (monkeys->monkeys[set_index].operation == '+')
        {
            // addition operation - need to set the yell-dependant monkey to have the value of the set monkey minus the value of the non-yell-dependant monkey
            if (monkeys->monkeys[idx[0]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is +; Monkey %d (%s) is yell-dependant. Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld - %lld = %lld\n", 
                         idx[0], monkeys->monkeys[idx[0]].own_name, 
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[1]].value,
                         idx[0], monkeys->monkeys[idx[0]].own_name, value, monkeys->monkeys[idx[1]].value, value - monkeys->monkeys[idx[1]].value);
#endif
                set_and_recurse(monkeys, idx[0], value - monkeys->monkeys[idx[1]].value);
            }
            else if (monkeys->monkeys[idx[1]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is +; Monkey %d (%s) is yell-dependant. Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld - %lld = %lld\n", 
                         idx[1], monkeys->monkeys[idx[1]].own_name, 
                         idx[0], monkeys->monkeys[idx[0]].own_name, monkeys->monkeys[idx[0]].value,
                         idx[1], monkeys->monkeys[idx[1]].own_name, value, monkeys->monkeys[idx[0]].value, value - monkeys->monkeys[idx[0]].value);
#endif
                set_and_recurse(monkeys, idx[1], value - monkeys->monkeys[idx[0]].value);
            }
        }

        // subtraction - opereation
        if (monkeys->monkeys[set_index].operation == '-')
        {
            // subtraction operation
            // value = first - second
            // if yell-dependant monkey is first operand, need to set its value to set_value + second monkey's value
            // if the yell-dependant monkey is second, need to set its value to first - set_value
            if (monkeys->monkeys[idx[0]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is -; First Monkey %d (%s) is yell-dependant. Second Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld + %lld = %lld\n", 
                         idx[0], monkeys->monkeys[idx[0]].own_name, 
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[1]].value,
                         idx[0], monkeys->monkeys[idx[0]].own_name, value, monkeys->monkeys[idx[1]].value, value + monkeys->monkeys[idx[1]].value);
#endif
                set_and_recurse(monkeys, idx[0], value + monkeys->monkeys[idx[1]].value);
            }
            else if (monkeys->monkeys[idx[1]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is -; Second Monkey %d (%s) is yell-dependant. First Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld - %lld = %lld\n", 
                         idx[1], monkeys->monkeys[idx[1]].own_name, 
                         idx[0], monkeys->monkeys[idx[0]].own_name, monkeys->monkeys[idx[0]].value,
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[0]].value, value, monkeys->monkeys[idx[0]].value - value);
#endif
                set_and_recurse(monkeys, idx[1], monkeys->monkeys[idx[0]].value - value);
            }
        }

        // multiplication * opereation
        if (monkeys->monkeys[set_index].operation == '*')
        {
            // multiplication operation - need to set the yell-dependant monkey to have the value of the set monkey divided by the value of the non-yell-dependant monkey
            if (monkeys->monkeys[idx[0]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is *; Monkey %d (%s) is yell-dependant. Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld / %lld = %lld\n", 
                         idx[0], monkeys->monkeys[idx[0]].own_name, 
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[1]].value,
                         idx[0], monkeys->monkeys[idx[0]].own_name, value, monkeys->monkeys[idx[1]].value, value / monkeys->monkeys[idx[1]].value);
#endif
                set_and_recurse(monkeys, idx[0], value / monkeys->monkeys[idx[1]].value);
            }
            else if (monkeys->monkeys[idx[1]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is *; Monkey %d (%s) is yell-dependant. Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld / %lld = %lld\n", 
                         idx[1], monkeys->monkeys[idx[1]].own_name, 
                         idx[0], monkeys->monkeys[idx[0]].own_name, monkeys->monkeys[idx[0]].value,
                         idx[1], monkeys->monkeys[idx[1]].own_name, value, monkeys->monkeys[idx[0]].value, value / monkeys->monkeys[idx[0]].value);
#endif
                set_and_recurse(monkeys, idx[1], value / monkeys->monkeys[idx[0]].value);
            }
        }

        // division / opereation
        if (monkeys->monkeys[set_index].operation == '/')
        {
            // division operation
            // value = first / second
            // first = value * second
            // second = first / value
            // if yell-dependant monkey is first operand, need to set its value to set_value * second monkey's value
            // if the yell-dependant monkey is second, need to set its value to first / set_value
            if (monkeys->monkeys[idx[0]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is /; First Monkey %d (%s) is yell-dependant. Second Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld * %lld = %lld\n", 
                         idx[0], monkeys->monkeys[idx[0]].own_name, 
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[1]].value,
                         idx[0], monkeys->monkeys[idx[0]].own_name, value, monkeys->monkeys[idx[1]].value, value * monkeys->monkeys[idx[1]].value);
#endif
                set_and_recurse(monkeys, idx[0], value * monkeys->monkeys[idx[1]].value);
            }
            else if (monkeys->monkeys[idx[1]].is_yell_dependant == TRUE)
            {
#ifdef DEBUG_DAY_21
                printf(" Operation is /; Second Monkey %d (%s) is yell-dependant. First Monkey %d (%s) is non-yell-dependant with value %lld. Setting monkey %d (%s) to %lld / %lld = %lld\n", 
                         idx[1], monkeys->monkeys[idx[1]].own_name, 
                         idx[0], monkeys->monkeys[idx[0]].own_name, monkeys->monkeys[idx[0]].value,
                         idx[1], monkeys->monkeys[idx[1]].own_name, monkeys->monkeys[idx[0]].value, value, monkeys->monkeys[idx[0]].value / value);
#endif
                set_and_recurse(monkeys, idx[1], monkeys->monkeys[idx[0]].value / value);
            }
        }
    }
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

void day_21_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_21_monkeys_t monkeys;
    
    read_and_parse_input(filename, &monkeys);
    
    fixup_operation_pointers(&monkeys);
    
    int equal_index = 0;
    int yell_index = 0;
    for (int i=0; i<monkeys.num_monkeys; i++)
    {
        if (strncmp(monkeys.monkeys[i].own_name, PART_2_EQUAL_MONKEY, DAY_21_MONKEY_NAME_LENGTH) == 0)
        {
            equal_index = i;
        }
        if (strncmp(monkeys.monkeys[i].own_name, PART_2_YELL_MONKEY, DAY_21_MONKEY_NAME_LENGTH) == 0)
        {
            yell_index = i;
        }
    }
    
#ifdef DEBUG_DAY_21
        printf("Equal Monkey is index %d and Yell Monkey is index %d\n", equal_index, yell_index);
#endif
    
    // assign equal operation to equal monkey
    monkeys.monkeys[equal_index].operation = '=';
    
    find_yell_dependent(&monkeys, yell_index);
    
    set_non_yell_dependant_values(&monkeys, equal_index);
    
    set_and_recurse(&monkeys, equal_index, 1);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%lld", monkeys.monkeys[yell_index].value);
    
    return;
}
