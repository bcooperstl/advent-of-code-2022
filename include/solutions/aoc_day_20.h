#ifndef __AOC_DAY_20__
#define __AOC_DAY_20__

#include <stdint.h>

#include "constants.h"

#define DAY_20_MAX_NODES 5000

struct day_20_ring_node;

struct day_20_ring_node
{
    long long value;
    struct day_20_ring_node * prev;
    struct day_20_ring_node * next;
};

typedef struct day_20_ring_node day_20_ring_node_t;

struct day_20_index_node
{
    long long value;
    day_20_ring_node_t ring_node;
};

typedef struct day_20_index_node day_20_index_node_t;

struct day_20_index
{
    day_20_index_node_t index_nodes[DAY_20_MAX_NODES];
    int num_nodes;
    int zero_node_index;
};

typedef struct day_20_index day_20_index_t;

void day_20_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_20_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
