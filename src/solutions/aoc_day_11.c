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
    
    monkeys->num_monkeys = (fd.num_lines + 1) / 7;

#ifdef DEBUG_DAY_11_PARSE
    printf("The input file has %d lines, which means there are %d monkeys\n", fd.num_lines, monkeys->num_monkeys);
#endif

    ld = fd.head_line;
    td = ld->head_token;
    
    for (int monkey_idx=0; monkey_idx < monkeys->num_monkeys; monkey_idx++)
    {   
#ifdef DEBUG_DAY_11_PARSE
        printf("Parsing Monkey %d\n", monkey_idx);
#endif
        // skip the Monkey line; corresponds to index
        monkeys->monkeys[monkey_idx].index_number = monkey_idx;
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
            printf("    Item %d is %ld\n", item_idx, monkeys->monkeys[monkey_idx].items[item_idx]);
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

static void add_item(day_11_monkey_t * monkey, long item)
{
#ifdef DEBUG_DAY_11_ITEM
    printf("     Added monkey %d item position %d with value %ld\n", monkey->index_number, monkey->num_items, item);
#endif
    monkey->items[monkey->num_items] = item;
    monkey->num_items++;
}

static void inspect_item(day_11_monkeys_t * monkeys, day_11_monkey_t * monkey, int item_idx)
{
    long item = monkey->items[item_idx];
#ifdef DEBUG_DAY_11_ITEM
    printf("  Monkey inspected an item with a worry level of %ld.\n", item);
#endif
    switch (monkey->operation_type)
    {
        case DAY11_OP_OLD_SQUARED:
            item = item * item;
#ifdef DEBUG_DAY_11_ITEM
            printf("    Worry level is multiplied by itself to %ld.\n", item);
#endif
            break;
        case DAY11_OP_OLD_MULTIPLY:
            item = item * monkey->operation_constant;
#ifdef DEBUG_DAY_11_ITEM
            printf("    Worry level is multiplied by %ld to %ld.\n", monkey->operation_constant, item);
#endif
            break;
        case DAY11_OP_OLD_ADD:
            item = item + monkey->operation_constant;
#ifdef DEBUG_DAY_11_ITEM
            printf("    Worry level increases by %ld to %ld.\n", monkey->operation_constant, item);
#endif
            break;
    }
    item = item / 3L; // 3 as a long
#ifdef DEBUG_DAY_11_ITEM
    printf("    Monkey gets bored with item. Worry level is divided by 3 to %ld\n", item);
#endif
    if ((item % monkey->test_modulus) == 0)
    {
#ifdef DEBUG_DAY_11_ITEM
        printf("    Current worry level is divisible by %ld.\n", monkey->test_modulus);
        printf("    Item with worry level %ld is thrown to monkey %d\n", item, monkey->true_destination);
#endif
        add_item(&monkeys->monkeys[monkey->true_destination], item);
    }
    else
    {
#ifdef DEBUG_DAY_11_ITEM
        printf("    Current worry level is not divisible by %ld.\n", monkey->test_modulus);
        printf("    Item with worry level %ld is thrown to monkey %d\n", item, monkey->false_destination);
#endif
        add_item(&monkeys->monkeys[monkey->false_destination], item);
    }
    monkey->inspection_count++;
}

static void monkey_inspect_item_list(day_11_monkeys_t * monkeys, day_11_monkey_t * monkey)
{
    for (int item_idx=0; item_idx<monkey->num_items; item_idx++)
    {
        inspect_item(monkeys, monkey, item_idx);
    }
    monkey->num_items = 0;
}

static void do_round(day_11_monkeys_t * monkeys)
{
    for (int monkey_idx=0; monkey_idx < monkeys->num_monkeys; monkey_idx++)
    {
#ifdef DEBUG_DAY_11_ITEM
        printf("Monkey %d:\n", monkey_idx);
#endif
        monkey_inspect_item_list(monkeys, &monkeys->monkeys[monkey_idx]);
    }
    
#ifdef DEBUG_DAY_11
    printf("After round completion, the monkeys are holding items with the following worry levels:\n");
    for (int m=0; m<monkeys->num_monkeys; m++)
    {
        printf("Monkey %d: ", m);
        for (int i=0; i<monkeys->monkeys[m].num_items; i++)
        {
            printf("%ld ", monkeys->monkeys[m].items[i]);
        }
        printf("\n");
    }
#endif
}

void day_11_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_11_monkeys_t monkeys;
    
    if (parse_input(filename, &monkeys) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    for (int i=0; i<20; i++)
    {
        do_round(&monkeys);
    }
    
    long best_two[2] = {0L,0L}; // zero as longs
    for (int i=0; i<monkeys.num_monkeys; i++)
    {
        if (monkeys.monkeys[i].inspection_count > best_two[0])
        {
            best_two[1] = best_two[0];
            best_two[0] = monkeys.monkeys[i].inspection_count;
        }
        else if (monkeys.monkeys[i].inspection_count > best_two[1])
        {
            best_two[1] = monkeys.monkeys[i].inspection_count;
        }
    }    
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", best_two[0] * best_two[1]);
    
    return;
}


static void add_item_2(day_11_monkey_t * monkey, long item)
{
    monkey->items[monkey->num_items] = item;
    monkey->num_items++;
}

static void inspect_item_2(day_11_monkeys_t * monkeys, day_11_monkey_t * monkey, int item_idx)
{
    long item = monkey->items[item_idx];
    switch (monkey->operation_type)
    {
        case DAY11_OP_OLD_SQUARED:
            item = item * item;
            break;
        case DAY11_OP_OLD_MULTIPLY:
            item = item * monkey->operation_constant;
            break;
        case DAY11_OP_OLD_ADD:
            item = item + monkey->operation_constant;
            break;
    }

    if ((item % monkey->test_modulus) == 0)
    {
        add_item_2(&monkeys->monkeys[monkey->true_destination], item % monkeys->product_of_mods);
    }
    else
    {
        add_item_2(&monkeys->monkeys[monkey->false_destination], item % monkeys->product_of_mods);
    }
    monkey->inspection_count++;
}

static void monkey_inspect_item_list_2(day_11_monkeys_t * monkeys, day_11_monkey_t * monkey)
{
    for (int item_idx=0; item_idx<monkey->num_items; item_idx++)
    {
        inspect_item_2(monkeys, monkey, item_idx);
    }
    monkey->num_items = 0;
}

static void do_round_2(day_11_monkeys_t * monkeys)
{
    for (int monkey_idx=0; monkey_idx < monkeys->num_monkeys; monkey_idx++)
    {
        monkey_inspect_item_list_2(monkeys, &monkeys->monkeys[monkey_idx]);
    }
}

void day_11_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_11_monkeys_t monkeys;
    
    if (parse_input(filename, &monkeys) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    monkeys.product_of_mods = 1;
    for (int i=0; i<monkeys.num_monkeys; i++)
    {
        monkeys.product_of_mods *= monkeys.monkeys[i].test_modulus;
    }
    
#ifdef DEBUG_DAY_11
    printf("The product of the mods is %ld\n", monkeys.product_of_mods);
#endif
    
    for (int i=1; i<=10000; i++)
    {
        do_round_2(&monkeys);
#ifdef DEBUG_DAY_11
        if (i == 1 || i == 20 || (i % 1000) == 0)
        {
            printf("== After round %d ==\n", i);
            for (int m=0; m<monkeys.num_monkeys; m++)
            {
                printf("Monkey %d inspected %d items.\n", m, monkeys.monkeys[m].inspection_count);
            }
        }
#endif
    }
    
    long best_two[2] = {0L,0L}; // zero as longs
    for (int i=0; i<monkeys.num_monkeys; i++)
    {
        if (monkeys.monkeys[i].inspection_count > best_two[0])
        {
            best_two[1] = best_two[0];
            best_two[0] = monkeys.monkeys[i].inspection_count;
        }
        else if (monkeys.monkeys[i].inspection_count > best_two[1])
        {
            best_two[1] = monkeys.monkeys[i].inspection_count;
        }
    }    
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", best_two[0] * best_two[1]);
    
    return;
}
