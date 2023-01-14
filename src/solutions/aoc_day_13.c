#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_13.h"
#include "file_utils.h"

#define RIGHT_ORDER 1
#define WRONG_ORDER 2
#define CONTINUE 3

// pos is index for the starting [ of the list
// return value is the pos after the ending ] of the list; i.e. the next char to parse
static int parse_list(char * list, int pos, day_13_element_t * element, int depth)
{
#ifdef DEBUG_DAY_13_PARSE
    printf("%*sParsing list {%s} starting at position %d\n", depth, "", list, pos);
#endif
    
    element->is_list = TRUE;
    element->num_list_elements = 0;
    
#ifdef DEBUG_DAY_13_PARSE
    printf("%*s skipping starting '[' at position %d\n", depth, "", pos);
#endif
    pos++; // skip over the starting [
    while (list[pos] != ']')
    {
        if (list[pos] == '[')
        {
#ifdef DEBUG_DAY_13_PARSE
            printf("%*s adding new list at element %d because of '[' found at position %d\n", depth, "", element->num_list_elements, pos);
#endif
            if (element->num_list_elements > DAY_13_MAX_LIST_ELEMENTS)
            {
                fprintf(stderr, "!!!INCREASE DAY_13_MAX_LIST_ELEMENTS!!!\n");
                return -1;
            }
            element->list_elements[element->num_list_elements] = (day_13_element_t *)malloc(sizeof(day_13_element_t));
            pos = parse_list(list, pos, element->list_elements[element->num_list_elements], depth+1);
            element->num_list_elements++;
#ifdef DEBUG_DAY_13_PARSE
            printf("%*s next character to parse is at position %d\n", depth, "", pos);
#endif
        }
        else if (list[pos] >= '0' && list[pos] <= '9')
        {
#ifdef DEBUG_DAY_13_PARSE
            printf("%*s adding new integer at element %d because of digit found at position %d\n", depth, "", element->num_list_elements, pos);
#endif
            if (element->num_list_elements > DAY_13_MAX_LIST_ELEMENTS)
            {
                fprintf(stderr, "!!!INCREASE DAY_13_MAX_LIST_ELEMENTS!!!\n");
                return -1;
            }
            int value = list[pos]-'0';
            pos++;
            while (list[pos] >= '0' && list[pos] <= '9')
            {
                value = (value * 10) + (list[pos]-'0');
                pos++;
            }
            element->list_elements[element->num_list_elements] = (day_13_element_t *)malloc(sizeof(day_13_element_t));
            element->list_elements[element->num_list_elements]->is_list = FALSE;
            element->list_elements[element->num_list_elements]->integer_value = value;
#ifdef DEBUG_DAY_13_PARSE
            printf("%*s integer value is %d; next character to parse is at position %d\n", depth, "", value, pos);
#endif
            element->num_list_elements++;
        }
        else
        {
            if (list[pos] != ',')
            {
                fprintf(stderr, "!!!Invalid character %c at position %d of {%s}!!!\n", list[pos], pos, list); 
                return -1;
            }
            pos++;
        }
    }
#ifdef DEBUG_DAY_13_PARSE
    printf("%*s Done parsing list; had %d elements; next position to parse is %d\n", depth, "", element->num_list_elements, pos+1);
#endif
    return pos+1;
}

static void cleanup_list(day_13_element_t * element)
{
    for (int i=0; i<element->num_list_elements; i++)
    {
        if (element->list_elements[i]->is_list == TRUE)
        {
            cleanup_list(element->list_elements[i]);
        }
        free(element->list_elements[i]);
    }
    return;
}

static void convert_int_to_list(day_13_element_t * element)
{
    day_13_element_t * child = (day_13_element_t *)malloc(sizeof(day_13_element_t));
    child->is_list = FALSE;
    child->integer_value = element->integer_value;
    element->is_list = TRUE;
    element->num_list_elements = 1;
    element->list_elements[0] = child;
    return;
}

static void dump_element(day_13_element_t * element)
{
    if (element->is_list == FALSE)
    {
        printf("%d", element->integer_value);
    }
    else // is_list = TRUE
    {
        printf("[");
        for (int i=0; i<element->num_list_elements; i++)
        {
            dump_element(element->list_elements[i]);
            if (i != element->num_list_elements-1)
            {
                printf(",");
            }
        }
        printf("]");
    }
}

static int compare(day_13_element_t * left, day_13_element_t * right, int depth)
{
#ifdef DEBUG_DAY_13
    printf("%*s- Compare ", depth*2, "");
    dump_element(left);
    printf(" vs ");
    dump_element(right);
    printf("\n");
#endif
    if (left->is_list == TRUE && right->is_list == FALSE)
    {
        convert_int_to_list(right);
#ifdef DEBUG_DAY_13
        printf("%*s- Mixed types; convert right to ", depth*2+2, "");
        dump_element(right);
        printf(" and retry comparision\n");
        printf("%*s- Compare ", depth*2, "");
        dump_element(left);
        printf(" vs ");
        dump_element(right);
        printf("\n");
#endif
    }
    if (left->is_list == FALSE && right->is_list == TRUE)
    {
        convert_int_to_list(left);
#ifdef DEBUG_DAY_13
        printf("%*s- Mixed types; convert left to ", depth*2+2, "");
        dump_element(left);
        printf(" and retry comparision\n");
        printf("%*s- Compare ", depth*2, "");
        dump_element(left);
        printf(" vs ");
        dump_element(right);
        printf("\n");
#endif
    }

    if (left->is_list == FALSE && right->is_list == FALSE)
    {
        if (left->integer_value < right->integer_value)
        {
#ifdef DEBUG_DAY_13
            printf("%*s- Left side is smaller, so inputs are in the right order\n", depth*2+2, "");
#endif
            return RIGHT_ORDER;
        }
        else if (left->integer_value > right->integer_value)
        {
#ifdef DEBUG_DAY_13
            printf("%*s- Right side is smaller, so inputs are in the wrong order\n", depth*2+2, "");
#endif
            return WRONG_ORDER;
        }
        else
        {
#ifdef DEBUG_DAY_13
            printf("%*s- Left and right sides are equal; no decision made\n", depth*2+2, "");
#endif
            return CONTINUE;
        }
    }
    
    if (left->is_list == TRUE && right->is_list == TRUE)
    {
        int pos = 0;
        while (1)
        {
            if (left->num_list_elements > pos) // left exists
            {
                if (right->num_list_elements > pos) // right exists
                {
                    int comp = compare(left->list_elements[pos], right->list_elements[pos], depth+1);
                    if (comp != CONTINUE)
                    {
                        return comp;
                    }
                }
                else // right does not exist; right ran out first; wrong order\n
                {
#ifdef DEBUG_DAY_13
                    printf("%*s- Right side ran out of items, so inputs are in the wrong order\n", depth*2+2, "");
#endif
                    return WRONG_ORDER;
                }
            }
            else // left does not exist
            {
                if (right->num_list_elements > pos) // right exists
                {
#ifdef DEBUG_DAY_13
                    printf("%*s- Left side ran out of items, so inputs are in the right order\n", depth*2+2, "");
#endif
                    return RIGHT_ORDER;
                }
                else // right does not exist
                {
#ifdef DEBUG_DAY_13
                    printf("%*s- Both sides ran out of items, no decision made\n", depth*2+2, "");
#endif
                    return CONTINUE;
                }
            }
            pos++;
        }
    }
    return CONTINUE;
}    

void day_13_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    int sum_indices = 0;
    int num_pairs;
    
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
        return;
    }
    
    num_pairs = (fd.num_lines + 1) / 3;

#ifdef DEBUG_DAY_13_PARSE
    printf("The input file has %d lines, which means there are %d pairs\n", fd.num_lines, num_pairs);
#endif

    ld = fd.head_line;
    
    for (int index=1; index<=num_pairs; index++)
    {
        day_13_element_t left, right;
        parse_list(ld->head_token->token, 0, &left, 0);
        ld = ld->next;
        parse_list(ld->head_token->token, 0, &right, 0);
        if (index != num_pairs)
        {
            ld = ld->next->next; // advance 2 lines to the start of the next pair; don't do this on last pair - will seg fault
        }
        
        int comp = compare(&left, &right, 0);
        if (comp == RIGHT_ORDER)
        {
            sum_indices += index;
#ifdef DEBUG_DAY_13            
            printf("Pair %d is in the right order; adding to sum\n", index);
#endif
        }
        else if (comp == WRONG_ORDER)
        {
#ifdef DEBUG_DAY_13            
            printf("Pair %d is in the wrong order\n", index);
#endif
        }
        else
        {    
            fprintf(stderr, "!!!Pair %d not in right or wrong order. Fix something!!!\n", index);
#ifdef DEBUG_DAY_13            
            dump_element(&left);
            printf("\n");
            dump_element(&right);
            printf("\n");
#endif
        }
    }
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", sum_indices);
    
    return;
}
