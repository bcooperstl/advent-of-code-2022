#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_16.h"
#include "file_utils.h"

#define USED_0  0x0001
#define USED_1  0x0002
#define USED_2  0x0004
#define USED_3  0x0008
#define USED_4  0x0010
#define USED_5  0x0020
#define USED_6  0x0040
#define USED_7  0x0080
#define USED_8  0x0100
#define USED_9  0x0200
#define USED_10 0x0400
#define USED_11 0x0800
#define USED_12 0x1000
#define USED_13 0x2000
#define USED_14 0x4000
#define USED_15 0x8000

static uint16_t masks[16] = {USED_0, USED_1, USED_2, USED_3, USED_4, USED_5, USED_6, USED_7,
                             USED_8, USED_9, USED_10, USED_11, USED_12, USED_13, USED_14, USED_15};

static int parse_input(char * filename, day_16_valves_t * valves)
{
    valves->num_all_valves = 0;
    valves->num_important_valves = 0;
    valves->start_valve = NULL;
    
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
    
    ld = fd.head_line;
    while (ld != NULL)
    {
        td = ld->head_token;
        valves->all_valves[valves->num_all_valves].label[0] = td->token[6];
        valves->all_valves[valves->num_all_valves].label[1] = td->token[7];
        valves->all_valves[valves->num_all_valves].label[2] = '\0';
        valves->all_valves[valves->num_all_valves].flow_rate = strtol(td->token+23, NULL, 10);
#ifdef DEBUG_DAY_16_PARSE
        printf("Valve %d created with label %s and flow rate %d\n", 
               valves->num_all_valves, valves->all_valves[valves->num_all_valves].label,
               valves->all_valves[valves->num_all_valves].flow_rate);
#endif
        ld = ld->next;
        valves->num_all_valves++;
    }
    
    ld = fd.head_line;
    int valve_index = 0;
    while (ld != NULL)
    {
        td = ld->head_token;
        day_16_valve_t * current_valve = &valves->all_valves[valve_index];
        int loc = (td->token[49]==' ' ? 50 : 49);
#ifdef DEBUG_DAY_16_PARSE
        printf("Parsing neighbors from [%s] starting at %d\n", td->token, loc);
#endif
        current_valve->num_neighbors = 0;
        while (loc < strlen(td->token))
        {
            for (int match_index=0; match_index < valves->num_all_valves; match_index++)
            {
                if (valves->all_valves[match_index].label[0] == td->token[loc] &&
                    valves->all_valves[match_index].label[1] == td->token[loc+1])
                {
                    current_valve->neighbors[current_valve->num_neighbors]=&valves->all_valves[match_index];
#ifdef DEBUG_DAY_16_PARSE
                    printf(" Neighbor %d of valve %s is %s\n", 
                           current_valve->num_neighbors,
                           current_valve->label,
                           current_valve->neighbors[current_valve->num_neighbors]->label);
#endif
                    break;
                }
            }
            current_valve->num_neighbors++;
            loc+=4;
        }
        ld = ld->next;
        valve_index++;
    }
    
#ifdef DEBUG_DAY_16_PARSE
    printf("Assigning index values to valves\n");
#endif
    
    for (int i=0; i<valves->num_all_valves; i++)
    {
        valves->all_valves[i].all_index = i;
        day_16_valve_t * valve = &valves->all_valves[i];
#ifdef DEBUG_DAY_16_PARSE
        printf(" Valve %s gets all_index %d\n", valve->label, valve->all_index);
#endif

        if (strncmp(valve->label, "AA", 2) == 0)
        {
            valves->start_valve = &valves->all_valves[i];
#ifdef DEBUG_DAY_16_PARSE
            printf("  Valve %s assigned as start_valve\n", valve->label);
#endif
        }
    
        if (valves->all_valves[i].flow_rate > 0)
        {
            valves->all_valves[i].important_index = valves->num_important_valves;
            valves->important_valves[valves->num_important_valves]=&valves->all_valves[i];
#ifdef DEBUG_DAY_16_PARSE
            printf(" Valve %s (flow rate %d) is an important valve. gets important_index %d\n", valve->label, valve->flow_rate, valves->all_valves[i].important_index);            
#endif
            valves->num_important_valves++;
        }
    }
    return TRUE;
}

static void map_distances(day_16_valve_distance_maps_t * distances, day_16_valves_t * valves)
{
    distances->num_all_distances = valves->num_all_valves;
    distances->num_important_distances = valves->num_important_valves;
    
    for (int from=0; from<valves->num_all_valves; from++)
    {
#ifdef DEBUG_DAY_16_DISTANCES
        printf("Computing distances from value %d(%s)\n", from, valves->all_valves[from].label);
#endif
        for (int to=0; to<valves->num_all_valves; to++)
        {
            distances->all_distances[from][to] = -1;
        }
        distances->all_distances[from][from] = 0;
        int current_distance = 0;
        while (1)
        {
#ifdef DEBUG_DAY_16_DISTANCES
            printf(" Checking for items at distance %d\n", current_distance);
#endif
            for (int to=0; to<valves->num_all_valves; to++)
            {
                if (distances->all_distances[from][to] == current_distance)
                {
#ifdef DEBUG_DAY_16_DISTANCES
                    printf(" Valve %d(%s) matches. checking its neighbors\n", to, valves->all_valves[to].label);
#endif
                    for (int neighbor=0; neighbor<valves->all_valves[to].num_neighbors; neighbor++)
                    {
                        day_16_valve_t * neighbor_valve = valves->all_valves[to].neighbors[neighbor];
                        if (distances->all_distances[from][neighbor_valve->all_index] == -1)
                        {
                            distances->all_distances[from][neighbor_valve->all_index] = current_distance+1;
#ifdef DEBUG_DAY_16_DISTANCES
                            printf("  Neighbor %d(%s) does not have a distance set. Setting to %d\n",
                                  neighbor_valve->all_index, neighbor_valve->label, 
                                  distances->all_distances[from][neighbor_valve->all_index]);
#endif
                        }
                        else
                        {
#ifdef DEBUG_DAY_16_DISTANCES
                            printf("  Neighbor %d(%s) already has distance of %d\n",
                                  neighbor_valve->all_index, neighbor_valve->label, 
                                  distances->all_distances[from][neighbor_valve->all_index]);
#endif
                        }
                    }
                }
            }
            int all_processed = TRUE;
            for (int to=0; to<valves->num_all_valves; to++)
            {
                if (distances->all_distances[from][to] == -1)
                {
                    all_processed = FALSE;
                }
            }
            if (all_processed == TRUE)
            {
                break;
            }
            current_distance++;
        }
    }
    
#ifdef DEBUG_DAY_16_DISTANCES
    printf("Populating important and start distances\n");
#endif
    for (int from=0; from<distances->num_important_distances; from++)
    {
        for (int to=0; to<distances->num_important_distances; to++)
        {
            distances->important_distances[from][to] = distances->all_distances[valves->important_valves[from]->all_index][valves->important_valves[to]->all_index];
#ifdef DEBUG_DAY_16_DISTANCES
            printf("Important distance from %s to %s is %d\n", 
                   valves->important_valves[from]->label,
                   valves->important_valves[to]->label,
                   distances->important_distances[from][to]);
#endif
        }
        distances->start_distances[from] = distances->all_distances[valves->start_valve->all_index][valves->important_valves[from]->all_index];
#ifdef DEBUG_DAY_16_DISTANCES
    printf("Start distance to %s is %d\n", 
                   valves->important_valves[from]->label,
                   distances->start_distances[from]);
#endif
    }
}

static void init_path(day_16_path_t * path)
{
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        path->used[i] = FALSE;
    }
        path->current_position = -1;
    path->minutes_remaining = 30;
    path->total_pressure = 0;
}

static void copy_path(day_16_path_t * to, day_16_path_t * from)
{
    memcpy(to, from, sizeof(day_16_path_t));
}

static void init_path_pages(day_16_path_pages_t * pages)
{
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        pages->paths[i] = NULL;
        pages->num_paths_used[i] = 0;
    }
}

static void cleanup_path_pages(day_16_path_pages_t * pages)
{
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        if (pages->paths[i] != NULL)
        {
            free(pages->paths[i]);
            pages->paths[i] = NULL;
        }
        pages->num_paths_used[i] = 0;
    }
}

static int find_best_pressure(day_16_path_pages_t * pages)
{
    int best = 0;
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        for (int j=0; j<pages->num_paths_used[i]; j++)
        {
            if (pages->paths[i][j].total_pressure > best)
            {
                best = pages->paths[i][j].total_pressure;
#ifdef DEBUG_DAY_16
                printf("New best pressure of %d found at [%d][%d]\n", best, i ,j);
#endif
            }
        }
    }
    return best;
}

static void init_path_page_at_depth(day_16_path_pages_t * pages, int depth, int num_important)
{
    if (depth == 0)
    {
        // first level - will need to declare num_important paths
        pages->paths[0] = (day_16_path_t *)malloc(sizeof(day_16_path_t) * num_important);
    }
    else
    {
        // other levels - need to have up to (num_important - depth) possible paths for each path used at the prior level
        int num_to_allocate = pages->num_paths_used[depth - 1] * (num_important - depth);
        pages->paths[depth] = (day_16_path_t *)malloc(sizeof(day_16_path_t) * num_to_allocate);
    }
}

static void bfs_process_first_level(day_16_valve_distance_maps_t * distances, day_16_valves_t * valves, day_16_path_pages_t * pages)
{
    int num_important = valves->num_important_valves;
#ifdef DEBUG_DAY_16_BFS
    printf("Initializing BFS search at depth 0 for %d important valves; moves from start\n", num_important);
#endif
    init_path_page_at_depth(pages, 0, num_important);
    for (int i=0; i<num_important; i++)
    {
#ifdef DEBUG_DAY_16_BFS
        printf(" Processing move from start to %s to be stored at [%d,%d]\n", valves->important_valves[i]->label, 0, i);
#endif

        day_16_path_t * current_path = &pages->paths[0][i];
        init_path(current_path);
        int minutes_to_travel = distances->start_distances[i];
        current_path->used[i] = TRUE;
        current_path->current_position = i;
        current_path->minutes_remaining -= (minutes_to_travel + 1);
        int added_pressure = valves->important_valves[i]->flow_rate * current_path->minutes_remaining;
        current_path->total_pressure += added_pressure;
        pages->num_paths_used[0]++;
#ifdef DEBUG_DAY_16_BFS
        printf("  It took %d minutes to travel and 1 minute to open the valve, resulting in %d minutes remaining\n", minutes_to_travel, current_path->minutes_remaining);
        printf("  %d minutes of %d flow_rate adds %d current pressure, resulting in %d total pressure released\n", current_path->minutes_remaining, valves->important_valves[i]->flow_rate, added_pressure, current_path->total_pressure);
#endif
    }
}

static void bfs_process_level(day_16_valve_distance_maps_t * distances, day_16_valves_t * valves, day_16_path_pages_t * pages, int depth)
{
    int num_important = valves->num_important_valves;
#ifdef DEBUG_DAY_16_BFS
    printf("Initializing BFS search at depth %d for %d paths at depth %d\n", depth, pages->num_paths_used[depth-1], depth-1);
#endif
    init_path_page_at_depth(pages, depth, num_important);

    for (int from_idx=0; from_idx < pages->num_paths_used[depth-1]; from_idx++)
    {
        day_16_path_t * from = &pages->paths[depth-1][from_idx];
#ifdef DEBUG_DAY_16_BFS
        printf(" Processing moves from [%d,%d] (at %s) with %d minutes remaining\n", depth-1, from_idx, valves->important_valves[from->current_position]->label, from->minutes_remaining);
#endif
        
        for (int to_idx=0; to_idx<num_important; to_idx++)
        {
#ifdef DEBUG_DAY_16_BFS
            printf("  Checking move to %s\n", valves->important_valves[to_idx]->label);
#endif
            if (from->used[to_idx] == TRUE)
            {
#ifdef DEBUG_DAY_16_BFS
                printf("   Valve already visited. Skipping\n");
#endif
                continue;
            }
            int minutes_to_travel = distances->important_distances[from->current_position][to_idx];
#ifdef DEBUG_DAY_16_BFS
            printf("   It will take %d minutes to travel from %s to %s\n", minutes_to_travel, valves->important_valves[from->current_position]->label, valves->important_valves[to_idx]->label);
#endif
            if ((minutes_to_travel + 1) >= from->minutes_remaining)
            {
#ifdef DEBUG_DAY_16_BFS
                printf("   With only %d minutes remaining, the valve will not flow before time runs out. Skipping\n", from->minutes_remaining);
#endif
                continue;
            }
            
            day_16_path_t * current_path = &pages->paths[depth][pages->num_paths_used[depth]];
            copy_path(current_path, from);
            current_path->used[to_idx] = TRUE;
            current_path->current_position = to_idx;
            current_path->minutes_remaining -= (minutes_to_travel + 1);
            int added_pressure = valves->important_valves[to_idx]->flow_rate * current_path->minutes_remaining;
            current_path->total_pressure += added_pressure;
            pages->num_paths_used[depth]++;
#ifdef DEBUG_DAY_16_BFS
            printf("   It took %d minutes to travel and 1 minute to open the valve, resulting in %d minutes remaining\n", minutes_to_travel, current_path->minutes_remaining);
            printf("   %d minutes of %d flow_rate adds %d current pressure, resulting in %d total pressure released\n", current_path->minutes_remaining, valves->important_valves[to_idx]->flow_rate, added_pressure, current_path->total_pressure);
#endif
        }
    }
}

void day_16_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_16_valves_t valves;
    day_16_valve_distance_maps_t distances;
    day_16_path_pages_t path_pages;
    
    // read in the input file to valves
    
    parse_input(filename, &valves);
#ifdef DEBUG_DAY_16
    printf("Input file had %d total values and %d important valves\n", valves.num_all_valves, valves.num_important_valves);
#endif
    
    // create the distance map
    
    map_distances(&distances, &valves);
#ifdef DEBUG_DAY_16
    printf("Distances mapped\n");
#endif
    
    init_path_pages(&path_pages);
    
#ifdef DEBUG_DAY_16
    printf("Initializing search at depth 0\n");
#endif
    
    bfs_process_first_level(&distances, &valves, &path_pages);
    
    int depth=1;
    while (depth < valves.num_important_valves)
    {
#ifdef DEBUG_DAY_16
        printf("Search at depth %d had %d paths\n", depth-1, path_pages.num_paths_used[depth-1]);
#endif
        if (path_pages.num_paths_used[depth-1] == 0)
        {
#ifdef DEBUG_DAY_16
            printf("Nothing left to do. Stopping search\n");
#endif
            break;
        }
#ifdef DEBUG_DAY_16
        printf("Searching at depth %d\n", depth);
#endif
        bfs_process_level(&distances, &valves, &path_pages, depth);
        depth++;
    }
    
#ifdef DEBUG_DAY_16
    printf("Searching complete. Finding best pressure\n");
#endif
    
    int best = find_best_pressure(&path_pages);
    
    cleanup_path_pages(&path_pages);

    snprintf(result, MAX_RESULT_LENGTH+1, "%d", best);
    
    return;
}

static void init_path_2(day_16_path_2_packed_t * path)
{
    path->used = 0;
    path->current_position[0] = -1;
    path->current_position[0] = -1;
    path->minutes_remaining[0] = 26;
    path->minutes_remaining[1] = 26;
    path->total_pressure = 0;
}

static void copy_path_2(day_16_path_2_packed_t * to, day_16_path_2_packed_t * from)
{
    memcpy(to, from, sizeof(day_16_path_2_packed_t));
}

static void init_path_pages_2(day_16_path_pages_2_t * pages)
{
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        pages->paths[i] = NULL;
        pages->num_paths_used[i] = 0;
        pages->bests[i] = 0;
    }
}

static void find_best_at_depth(day_16_path_pages_2_t * pages, int depth)
{
    if (pages->paths[depth] == NULL)
    {
        pages->bests[depth] = 0;
    }
    else
    {
        for (int i=0; i<pages->num_paths_used[depth]; i++)
        {
            if (pages->paths[depth][i].total_pressure > pages->bests[depth])
            {
                pages->bests[depth] = pages->paths[depth][i].total_pressure;
            }
        }
    }
}

static void cleanup_path_pages_2_single_depth(day_16_path_pages_2_t * pages, int depth)
{
    if (pages->paths[depth] == NULL)
    {
        return;
    }
    free(pages->paths[depth]);
    pages->paths[depth] = NULL;
    pages->num_paths_used[depth] = 0;
}

static void cleanup_path_pages_2(day_16_path_pages_2_t * pages)
{
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        if (pages->paths[i] != NULL)
        {
            free(pages->paths[i]);
            pages->paths[i] = NULL;
        }
        pages->num_paths_used[i] = 0;
    }
}

static int find_best_pressure_2(day_16_path_pages_2_t * pages)
{
    int best = 0;
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        if (pages->bests[i] > best)
        {
            best = pages->bests[i];
        }
    }
    return best;
}

static void init_path_page_at_depth_2(day_16_path_pages_2_t * pages, int depth, int num_important)
{
    int num_to_allocate;
    if (depth == 0)
    {
        // first level - only allocate 1
        num_to_allocate = 1;
    }
    else
    {
        // other levels - need to have up to (num_important - depth - 1) * 2 possible paths for each path used at the prior level
        // need 2 for each prior one because of 2 different items to visit each location
        num_to_allocate = pages->num_paths_used[depth - 1] * (num_important - depth - 1) * 2;
        
    }
    pages->paths[depth] = (day_16_path_2_packed_t *)malloc(sizeof(day_16_path_2_packed_t) * num_to_allocate);
}

static void bfs_process_first_level_2(day_16_valve_distance_maps_t * distances, day_16_valves_t * valves, day_16_path_pages_2_t * pages, int first_idx, int second_idx)
{
    int num_important = valves->num_important_valves;
#ifdef DEBUG_DAY_16_BFS
    printf("Initializing BFS search at depth 0 for %d important valves; moves from start\n", num_important);
#endif
    init_path_page_at_depth_2(pages, 0, num_important);
#ifdef DEBUG_DAY_16_BFS
    printf(" Processing move with 0 from start to %s and 1 from start to %s to be stored at [%d,%d]\n", valves->important_valves[first_idx]->label, valves->important_valves[second_idx]->label, 0, 0);
#endif
    day_16_path_2_packed_t * current_path = &pages->paths[0][0];

    init_path_2(current_path);
    // do 0 with first_idx
    int minutes_to_travel = distances->start_distances[first_idx];
    current_path->used = current_path->used | masks[first_idx];
    current_path->current_position[0] = first_idx;
    current_path->minutes_remaining[0] -= (minutes_to_travel + 1);
    int added_pressure = valves->important_valves[first_idx]->flow_rate * current_path->minutes_remaining[0];
    current_path->total_pressure += added_pressure;
#ifdef DEBUG_DAY_16_BFS
    printf("  It took %d minutes for 0 to travel and 1 minute to open the valve, resulting in %d minutes remaining\n", minutes_to_travel, current_path->minutes_remaining[0]);
    printf("  %d minutes of %d flow_rate adds %d current pressure, resulting in %d total pressure released\n", current_path->minutes_remaining[0], valves->important_valves[first_idx]->flow_rate, added_pressure, current_path->total_pressure);
#endif
    
    // do 1 with second_idx
    minutes_to_travel = distances->start_distances[second_idx];
    current_path->used = current_path->used | masks[second_idx];
    current_path->current_position[1] = second_idx;
    current_path->minutes_remaining[1] -= (minutes_to_travel + 1);
    added_pressure = valves->important_valves[second_idx]->flow_rate * current_path->minutes_remaining[1];
    current_path->total_pressure += added_pressure;
#ifdef DEBUG_DAY_16_BFS
    printf("  It took %d minutes for 1 to travel and 1 minute to open the valve, resulting in %d minutes remaining\n", minutes_to_travel, current_path->minutes_remaining[1]);
    printf("  %d minutes of %d flow_rate adds %d current pressure, resulting in %d total pressure released\n", current_path->minutes_remaining[1], valves->important_valves[second_idx]->flow_rate, added_pressure, current_path->total_pressure);
#endif

    pages->num_paths_used[0] = 1;
}

static void bfs_process_level_2(day_16_valve_distance_maps_t * distances, day_16_valves_t * valves, day_16_path_pages_2_t * pages, int depth)
{
    int num_important = valves->num_important_valves;
#ifdef DEBUG_DAY_16_BFS
    printf("Initializing BFS search at depth %d for %d paths at depth %d\n", depth, pages->num_paths_used[depth-1], depth-1);
#endif
    init_path_page_at_depth_2(pages, depth, num_important);

    for (int from_idx=0; from_idx < pages->num_paths_used[depth-1]; from_idx++)
    {
        day_16_path_2_packed_t * from = &pages->paths[depth-1][from_idx];
#ifdef DEBUG_DAY_16_BFS
        printf(" Processing moves from [%d,%d]:  0 at %s with %d minutes remaining and 1 at %s with %d minutes remaining\n", depth-1, from_idx, valves->important_valves[from->current_position[0]]->label, from->minutes_remaining[0], valves->important_valves[from->current_position[1]]->label, from->minutes_remaining[1]);
#endif
        
        for (int to_idx=0; to_idx<num_important; to_idx++)
        {
#ifdef DEBUG_DAY_16_BFS
            printf("  Checking moves to %s\n", valves->important_valves[to_idx]->label);
#endif
            if ((from->used & masks[to_idx]) != 0)
            {
#ifdef DEBUG_DAY_16_BFS
                printf("   Valve already visited. Skipping\n");
#endif
                continue;
            }
            
            // check 0
            int minutes_to_travel = distances->important_distances[from->current_position[0]][to_idx];
#ifdef DEBUG_DAY_16_BFS
            printf("   It will take %d minutes for 0 to travel from %s to %s\n", minutes_to_travel, valves->important_valves[from->current_position[0]]->label, valves->important_valves[to_idx]->label);
#endif
            if ((minutes_to_travel + 1) >= from->minutes_remaining[0])
            {
#ifdef DEBUG_DAY_16_BFS
                printf("   With only %d minutes remaining, the valve will not flow before time runs out. Skipping\n", from->minutes_remaining[0]);
#endif
            }
            else
            {
                day_16_path_2_packed_t * current_path = &pages->paths[depth][pages->num_paths_used[depth]];
                copy_path_2(current_path, from);
                current_path->used = current_path->used | masks[to_idx];
                current_path->current_position[0] = to_idx;
                current_path->minutes_remaining[0] -= (minutes_to_travel + 1);
                int added_pressure = valves->important_valves[to_idx]->flow_rate * current_path->minutes_remaining[0];
                current_path->total_pressure += added_pressure;
                pages->num_paths_used[depth]++;
#ifdef DEBUG_DAY_16_BFS
                printf("   It took %d minutes to travel and 1 minute to open the valve, resulting in %d minutes remaining\n", minutes_to_travel, current_path->minutes_remaining[0]);
                printf("   %d minutes of %d flow_rate adds %d current pressure, resulting in %d total pressure released\n", current_path->minutes_remaining[0], valves->important_valves[to_idx]->flow_rate, added_pressure, current_path->total_pressure);
#endif
            }
            
            // check 1
            minutes_to_travel = distances->important_distances[from->current_position[1]][to_idx];
#ifdef DEBUG_DAY_16_BFS
            printf("   It will take %d minutes for 1 to travel from %s to %s\n", minutes_to_travel, valves->important_valves[from->current_position[1]]->label, valves->important_valves[to_idx]->label);
#endif
            if ((minutes_to_travel + 1) >= from->minutes_remaining[1])
            {
#ifdef DEBUG_DAY_16_BFS
                printf("   With only %d minutes remaining, the valve will not flow before time runs out. Skipping\n", from->minutes_remaining[1]);
#endif
            }
            else
            {
                day_16_path_2_packed_t * current_path = &pages->paths[depth][pages->num_paths_used[depth]];
                copy_path_2(current_path, from);
                current_path->used = current_path->used | masks[to_idx];
                current_path->current_position[1] = to_idx;
                current_path->minutes_remaining[1] -= (minutes_to_travel + 1);
                int added_pressure = valves->important_valves[to_idx]->flow_rate * current_path->minutes_remaining[1];
                current_path->total_pressure += added_pressure;
                pages->num_paths_used[depth]++;
#ifdef DEBUG_DAY_16_BFS
                printf("   It took %d minutes to travel and 1 minute to open the valve, resulting in %d minutes remaining\n", minutes_to_travel, current_path->minutes_remaining[1]);
                printf("   %d minutes of %d flow_rate adds %d current pressure, resulting in %d total pressure released\n", current_path->minutes_remaining[1], valves->important_valves[to_idx]->flow_rate, added_pressure, current_path->total_pressure);
#endif
            }
        }
    }
}

void day_16_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_16_valves_t valves;
    day_16_valve_distance_maps_t distances;
    day_16_path_pages_2_t path_pages;
    
    printf("Size of day_16_path_2_packed_t is %d\n", sizeof(day_16_path_2_packed_t));
    
    for (int i=0; i<16; i++)
    {
        printf("%d %hu\n", i, masks[i]);
    }
    
    // read in the input file to valves
    
    parse_input(filename, &valves);
#ifdef DEBUG_DAY_16
    printf("Input file had %d total values and %d important valves\n", valves.num_all_valves, valves.num_important_valves);
#endif
    
    // create the distance map
    
    map_distances(&distances, &valves);
#ifdef DEBUG_DAY_16
    printf("Distances mapped\n");
#endif
    
    int overall_best = 0;
    
    for (int i=0; i<valves.num_important_valves-1; i++)
    {
        for (int j=i+1; j<valves.num_important_valves; j++)
        {    
#ifdef DEBUG_DAY_16
            printf("Initializing search at depth 0 to %d and %d\n", i, j);
#endif
            init_path_pages_2(&path_pages);
        
            bfs_process_first_level_2(&distances, &valves, &path_pages, i, j);
            find_best_at_depth(&path_pages, 0);
            
            int depth=1;
            while (depth < valves.num_important_valves)
            {
#ifdef DEBUG_DAY_16
                printf("Search at depth %d had %d paths\n", depth-1, path_pages.num_paths_used[depth-1]);
#endif
                if (path_pages.num_paths_used[depth-1] == 0)
                {
#ifdef DEBUG_DAY_16
                    printf("Nothing left to do. Stopping search\n");
#endif
                    break;
                }
#ifdef DEBUG_DAY_16
                printf("Searching at depth %d\n", depth);
#endif
                bfs_process_level_2(&distances, &valves, &path_pages, depth);
                find_best_at_depth(&path_pages, depth);
                if (depth >= 2)
                {
                    cleanup_path_pages_2_single_depth(&path_pages, depth-2);
                }
                depth++;
            }
    
#ifdef DEBUG_DAY_16
            printf("Searching complete. Finding best pressure\n");
#endif

            int best = find_best_pressure_2(&path_pages);
            if (best > overall_best)
            {
#ifdef DEBUG_DAY_16
                printf("Best of %d is better than prior overall best of %d\n", best, overall_best);
#endif
                overall_best = best;
            }
            cleanup_path_pages_2(&path_pages);
        }
    }

    snprintf(result, MAX_RESULT_LENGTH+1, "%d", overall_best);
    
    return;
}

