#ifndef __AOC_DAY_13__
#define __AOC_DAY_13__

#include "constants.h"

#define DAY_13_MAX_LIST_ELEMENTS 16

struct day_13_element
{
    int is_list;
    int integer_value;
    struct day_13_element * list_elements[DAY_13_MAX_LIST_ELEMENTS];
    int num_list_elements;
};

typedef struct day_13_element day_13_element_t;

void day_13_part_1(char * forestname, extra_args_t * extra_args, char * result);
void day_13_part_2(char * forestname, extra_args_t * extra_args, char * result);

#endif
