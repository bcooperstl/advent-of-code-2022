#ifndef __AOC_DAY_16__
#define __AOC_DAY_16__

#include "constants.h"

#define DAY_16_LABEL_LENGTH 2
#define DAY_16_MAX_VALVES 50
#define DAY_16_MAX_IMPORTANT_VALVES 15    
#define DAY_16_MAX_NEIGHBORS 5
    
struct day_16_valve;
    
struct day_16_valve
{
    char label[DAY_16_LABEL_LENGTH+1];
    int flow_rate;
    struct day_16_valve * neighbors[DAY_16_MAX_NEIGHBORS];
    int num_neighbors;
    int all_index;
    int important_index;
};

typedef struct day_16_valve day_16_valve_t;

struct day_16_valves
{
    day_16_valve_t all_valves[DAY_16_MAX_VALVES];
    int num_all_valves;
    day_16_valve_t * important_valves[DAY_16_MAX_IMPORTANT_VALVES];
    int num_important_valves;
    day_16_valve_t * start_valve;
};

typedef struct day_16_valves day_16_valves_t;

struct day_16_valve_distance_maps
{
    // indexes are from, to
    int all_distances[DAY_16_MAX_VALVES][DAY_16_MAX_VALVES];
    
    // indexes are from, to
    int important_distances[DAY_16_MAX_IMPORTANT_VALVES][DAY_16_MAX_IMPORTANT_VALVES];
    
    // index is to
    int start_distances[DAY_16_MAX_IMPORTANT_VALVES];
    int num_all_distances;
    int num_important_distances;
};

typedef struct day_16_valve_distance_maps day_16_valve_distance_maps_t;

struct day_16_path
{
    char used[DAY_16_MAX_IMPORTANT_VALVES];
    int current_position;
    int minutes_remaining;
    int total_pressure;
};

typedef struct day_16_path day_16_path_t;

struct day_16_path_pages
{
    day_16_path_t * paths[DAY_16_MAX_IMPORTANT_VALVES];
    int num_paths_used[DAY_16_MAX_IMPORTANT_VALVES];
};

typedef struct day_16_path_pages day_16_path_pages_t;

struct day_16_path_2
{
    char used[DAY_16_MAX_IMPORTANT_VALVES];
    char current_position[2];
    char minutes_remaining[2];
    short int total_pressure;
};

typedef struct day_16_path_2 day_16_path_2_t;

struct day_16_path_pages_2
{
    day_16_path_2_t * paths[DAY_16_MAX_IMPORTANT_VALVES];
    int num_paths_used[DAY_16_MAX_IMPORTANT_VALVES];
    int bests[DAY_16_MAX_IMPORTANT_VALVES];
};

typedef struct day_16_path_pages_2 day_16_path_pages_2_t;

void day_16_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_16_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
