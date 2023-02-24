#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_16.h"
#include "file_utils.h"


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
    
    for (int from=0; from<=valves->num_all_valves; from++)
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
                            printf("  Neighbor %d(%s) doealready has distance of %d\n",
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
            printf("Important distnace from %s to %s is %d\n", 
                   valves->important_valves[from]->label,
                   valves->important_valves[to]->label,
                   distances->important_distances[from][to]);
#endif
        }
        distances->start_distances[from] = distances->all_distances[valves->start_valve->all_index][valves->important_valves[from]->all_index];
#ifdef DEBUG_DAY_16_DISTANCES
    printf("Start distnace to %s is %d\n", 
                   valves->important_valves[from]->label,
                   distances->start_distances[from]);
#endif
    }
}

/*
static int find_best_pressure_sparse_stack(day_16_distances_t * distances, day_16_valves_t * valves)
{
    int sparse_stack[30];
    int best_pressure = 0;
    printf("Finding best pressure release in 30 minutes with sparse stack\n");
    for (int i=0; i<sparse_stack; i++)
    {
        sparse_stack[i] = -1;
    }
    int current_index[DAY_16_MAX_IMPORTANT_VALVES];
    current_index[0] = 0;
    while (current_index[0] < distances->num_important_distances)
    {
        
    
    }


}

static int find_best_pressure(day_16_distances_t * distances, day_16_valves_t * valves, int minutes)
{
    int best_pressure = 0;
    printf("Finding best pressure release in %d minutes\n", minutes);
    
    int num_important = distances->num_important_distances;
    int indices[DAY_16_MAX_IMPORTANT_VALVES];
    int minutes_on[DAY_16_MAX_IMPORTANT_VALVES];
    int current_depth = 0;
    int time_remaining = minutes;
    
    for (int i=0; i<num_important; i++)
    {
        indices[i] = 0;
        minutes_on[i] = 0;
        
    }
    
    while (indices[0] < num_important)
    {
        printf("Current state - current_depth %d, moving to important index %d = %s\n", current_depth, indices[current_depth], valves->important_valves[indices[current_depth]]->label); 
        int move_time;
        if (current_depth == 0)
        {
            move_time = distances->start_distances[indices[current_depth]];
        }
        else
        {
            move_time = distances->all_distances[indices[current_depth-1]][indices[current_depth]];
        }
        
    }

    return best_pressure;
}
*/

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
/*
    for (int i=0; i<DAY_16_MAX_IMPORTANT_VALVES; i++)
    {
        to->used[i] = from->used[i];
    }
    to->complete = from->complete;
    to->current_position = from->current_position;
    to->minutes_remaining = from->minutes_remaining;
    to->total_pressure = from->total_pressure;
*/
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

    for (int from_idx=0; from_idx <= pages->num_paths_used[depth-1]; from_idx++)
    {
        day_16_path_t * from = &pages->paths[depth-1][from_idx];
#ifdef DEBUG_DAY_16_BFS
        printf(" Processing moves from [%d,%d] (at %s)\n", depth-1, from_idx, valves->important_valves[from->current_position]->label);
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
            printf("   It will take %d minutes to travel from %s to %s\n", valves->important_valves[from->current_position]->label, valves->important_valves[to_idx]->label);
#endif
            if ((minutes_to_travel + 1) >= from->minutes_remaining)
            {
#ifdef DEBUG_DAY_16_BFS
                printf("   With only %d minutes remaining, the valve will not flow before time runs out. Skipping\n", from->minutes_remaining);
#endif
                continue;
            }
            
            day_16_path_t * current_path = &pages->paths[depth][pages->num_paths_used[depth]];
            copy_path(from, current_path);
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
    
    return;
    
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

