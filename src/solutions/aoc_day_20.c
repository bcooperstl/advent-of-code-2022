#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_20.h"
#include "file_utils.h"

static void move_ring_node(day_20_ring_node_t * ring_node)
{
    day_20_ring_node_t * old_prev = ring_node->prev;
    day_20_ring_node_t * old_next = ring_node->next;
    
#ifdef DEBUG_DAY_20
    printf("Working node %d %d %d**%d**%d %d %d\n", ring_node->prev->prev->value, ring_node->prev->value, ring_node->prev->next->value, ring_node->value, ring_node->next->prev->value, ring_node->next->value, ring_node->next->next->value);
#endif

    if (ring_node->value == 0)
    {
#ifdef DEBUG_DAY_20
        printf(" Nothing to move for ring node with value 0\n");
#endif
    }

    if (ring_node->value > 0)
    {
#ifdef DEBUG_DAY_20
        printf(" Moving node %d forward\n", ring_node->value);
        
#endif
        day_20_ring_node_t * insert_after_node = ring_node;
        // pull it out of the list
        old_prev->next = old_next;
        old_next->prev = old_prev;
        
        for (int i=0; i<ring_node->value; i++)
        {
            insert_after_node = insert_after_node->next;
        }
        
        // assign new prev and next for ring_node
        ring_node->prev = insert_after_node;
        ring_node->next = insert_after_node->next;
            
        // add ring_node in new spot
        ring_node->next->prev = ring_node;
        insert_after_node->next = ring_node;
    }
    
    if (ring_node->value < 0)
    {
#ifdef DEBUG_DAY_20
        printf(" Moving node %d backward\n", ring_node->value);
        
#endif
        day_20_ring_node_t * insert_before_node = ring_node;
        // pull it out of the list
        old_prev->next = old_next;
        old_next->prev = old_prev;
        
        for (int i=0; i>ring_node->value; i--)
        {
            insert_before_node = insert_before_node->prev;
        }
        
        // assign new prev and next for ring_node
        ring_node->prev = insert_before_node->prev;
        ring_node->next = insert_before_node;
        
        // add ring_node in new spot
        ring_node->prev->next = ring_node;
        insert_before_node->prev = ring_node;
    }
    
#ifdef DEBUG_DAY_20
    printf("After moving node %d %d %d**%d**%d %d %d    old %d**%d %d**%d\n", 
           ring_node->prev->prev->value, ring_node->prev->value, 
           ring_node->prev->next->value, ring_node->value, ring_node->next->prev->value, 
           ring_node->next->value, ring_node->next->next->value,
           old_prev->value, old_next->prev->value,
           old_prev->next->value, old_next->value);
#endif
}

static void read_and_parse_input(char * filename, day_20_index_t * index)
{
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
    
    index->num_nodes = 0;
    line_data_t * ld = fd.head_line;
    
    while (ld != NULL)
    {
        token_data_t * td = ld->head_token;
        index->index_nodes[index->num_nodes].value = strtol(td->token, NULL, 10);
        index->index_nodes[index->num_nodes].ring_node.value = index->index_nodes[index->num_nodes].value;
        if (index->index_nodes[index->num_nodes].value == 0)
        {
#ifdef DEBUG_DAY_20
            printf("Zero value found at input line %d:\n", index->num_nodes);
#endif
            index->zero_node_index = index->num_nodes;
        }
        ld = ld->next;
        index->num_nodes++;
    }
    
#ifdef DEBUG_DAY_20
    printf("There were %d nodes in the file\n", index->num_nodes);
#endif
    file_data_cleanup(&fd);
    
    for (int i=0; i<index->num_nodes; i++)
    {
        index->index_nodes[i].ring_node.next = &index->index_nodes[(i+1)%index->num_nodes].ring_node;
        index->index_nodes[i].ring_node.prev = &index->index_nodes[(index->num_nodes+i-1)%index->num_nodes].ring_node;
    }
    
    return;
}

void day_20_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_20_index_t index;
    
    read_and_parse_input(filename, &index);
    
    int coordinate_sum = 0;
    
    for (int i=0; i<index.num_nodes; i++)
    {
        move_ring_node(&index.index_nodes[i].ring_node);
    }
    
    day_20_ring_node_t * coord = &index.index_nodes[index.zero_node_index].ring_node;
#ifdef DEBUG_DAY_20
    printf("Coordinate from position %d is %d\n", 0, coord->value);
#endif
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<1000; j++)
        {
            coord = coord->next;
        }
        
#ifdef DEBUG_DAY_20
        printf("Coordinate from position %d is %d\n", (i+1)*1000, coord->value);
#endif
        coordinate_sum+=coord->value;
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", coordinate_sum);
    
    return;
}