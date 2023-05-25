#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_19.h"
#include "file_utils.h"

#define PART_1_MINUTES 24
#define MINUTES_TO_CATCH_UP_GEODE_ROBOTS 2

static void init_state_page(day_19_state_page_t * page, int minutes)
{
    page->num_used_states = 0;
    page->minutes_elapsed = minutes;
    page->next = NULL;
}

static day_19_state_t * get_next_available_state(day_19_state_page_t ** page)
{
    day_19_state_t * ret;
    
    if ((*page)->num_used_states == DAY_19_STATES_PER_PAGE)
    {
#ifdef DEBUG_DAY_19
        printf("------Advancing next page due to current page being full\n");
#endif
        // page is full; allocate a new page
        (*page)->next = (day_19_state_page_t *)malloc (sizeof(day_19_state_page_t));
        (*page)->next->minutes_elapsed = (*page)->minutes_elapsed;
        (*page)->next->num_used_states = 0;
        (*page)->next->next = NULL;
        (*page) = (*page)->next;
    }
    
    ret = &((*page)->states[(*page)->num_used_states]);
    (*page)->num_used_states++;
    return ret;
}

static day_19_state_t * free_state_pages(day_19_state_page_t * head)
{
    day_19_state_page_t * current = head;
    while (current != NULL)
    {
        day_19_state_page_t * next = current->next;
        free(current);
        current = next;
    }
}

static void init_state(day_19_state_t * state)
{
    state->num_ore_robots = 1;
    state->num_clay_robots = 0;
    state->num_obsidian_robots = 0;
    state->num_geode_robots = 0;
    state->num_ore = 0;
    state->num_clay = 0;
    state->num_obsidian = 0;
    state->num_geode = 0;
}

static int can_build_ore_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    // we can never use more than any of these costs in a turn, so there is no point in having more than that number of ore robots
    if ((state->num_ore_robots >= blueprint->ore_robot_ore_cost) && 
        (state->num_ore_robots >= blueprint->clay_robot_ore_cost) && 
        (state->num_ore_robots >= blueprint->obsidian_robot_ore_cost) && 
        (state->num_ore_robots >= blueprint->geode_robot_ore_cost))
    {
        return FALSE;
    }
    if (state->num_ore >= blueprint->ore_robot_ore_cost)
    {
        return TRUE;
    }
    return FALSE;
}

static int can_build_clay_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    // we can never use more than obsidian_robot_clay_cost units of clay in a turn, so we don't need to create a clay robot if we have that many
    if (state->num_clay_robots >= blueprint->obsidian_robot_clay_cost)
    {
        return FALSE;
    }
    if (state->num_ore >= blueprint->clay_robot_ore_cost)
    {
        return TRUE;
    }
    return FALSE;
}

static int can_build_obsidian_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    // we can never use more than geode_robot_obsidian_cost units of obsidian in a turn, so we don't need to create a obsidian robot if we have that many
    if (state->num_obsidian_robots >= blueprint->geode_robot_obsidian_cost)
    {
        return FALSE;
    }
    if ((state->num_ore >= blueprint->obsidian_robot_ore_cost) && (state->num_clay >= blueprint->obsidian_robot_clay_cost))
    {
        return TRUE;
    }
    return FALSE;
}

static int can_build_geode_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    if ((state->num_ore >= blueprint->geode_robot_ore_cost) && (state->num_obsidian >= blueprint->geode_robot_obsidian_cost))
    {
        return TRUE;
    }
    return FALSE;
}

static void build_ore_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_ore_robots++;
    state->num_ore-=blueprint->ore_robot_ore_cost;
    return;
}

static void build_clay_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_clay_robots++;
    state->num_ore-=blueprint->clay_robot_ore_cost;
    return;
}

static void build_obsidian_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_obsidian_robots++;
    state->num_ore-=blueprint->obsidian_robot_ore_cost;
    state->num_clay-=blueprint->obsidian_robot_clay_cost;
    return;
}

static void build_geode_robot(day_19_state_t * state, day_19_blueprint_t * blueprint, int time)
{
    state->num_geode_robots++;
    state->num_ore-=blueprint->geode_robot_ore_cost;
    state->num_obsidian-=blueprint->geode_robot_obsidian_cost;
    if (state->num_geode_robots > blueprint->max_geode_robots_at_time[time])
    {
        blueprint->max_geode_robots_at_time[time] = state->num_geode_robots;
    }
    return;
}

static void accumulate_resources(day_19_state_t * state)
{
    state->num_ore+=state->num_ore_robots;
    state->num_clay+=state->num_clay_robots;
    state->num_obsidian+=state->num_obsidian_robots;
    state->num_geode+=state->num_geode_robots;
    return;
}

static void advance_minute(day_19_state_page_t * page)
{
    page->minutes_elapsed++;
    return;
}

static void clone_state(day_19_state_t * to, day_19_state_t * from)
{
    memcpy(to, from, sizeof(day_19_state_t));
    return;
}

static void display_state(day_19_state_t * state)
{
    printf(" Resources: Ore %d Clay %d Obs %d Geodes %d.   Robots: Ore %d Clay %d Obs %d Geodes %d\n",
            state->num_ore, 
            state->num_clay, 
            state->num_obsidian, 
            state->num_geode, 
            state->num_ore_robots, 
            state->num_clay_robots, 
            state->num_obsidian_robots, 
            state->num_geode_robots); 
    return;
}

static void init_blueprints(day_19_blueprints_t * blueprints)
{
    blueprints->num_blueprints = 0;
}

// Blueprint 1: Each ore robot costs 3 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 18 clay. Each geode robot costs 3 ore and 13 obsidian.
// 1         2  3    4   5     6     7 8    9    10   11    12    13 14  15   16       17    18    19 20 21  22 23    24   25    26    27    28 29 30  31 32
// skip 1; extract ID; 
// skip 4; extract ore_robot_ore_cost;
// skip 5; extract clay_robot_ore_cost;
// skip 5; extract obsidian_robot_ore_cost; skip 2; extract obsidian_robot_clay_cost;
// skip 5; extract geode_robot_ore_cost; skip 2; extract geode_robot_obsidian_cost; skip last 1
static int parse_blueprint_line(line_data_t * ld, day_19_blueprint_t * blueprint)
{
    if (ld->num_tokens != 32)
    {
        fprintf(stderr, "***Invalid number of tokens %d for line; expecting 32***\n", ld->num_tokens);
        return FALSE;
    }
    token_data_t * td = ld->head_token;
    // skip 1
    td = td->next;
    // extract ID;
    blueprint->blueprint_id = strtol(td->token, NULL, 10);
    td = td->next;
    // skip 4
    td = td->next->next->next->next;
    // extract ore_robot_ore_cost
    blueprint->ore_robot_ore_cost = strtol(td->token, NULL, 10);
    td = td->next;
    // skip 5
    td = td->next->next->next->next->next;
    // extract clay_robot_ore_cost
    blueprint->clay_robot_ore_cost = strtol(td->token, NULL, 10);
    td = td->next;
    // skip 5
    td = td->next->next->next->next->next;
    // extract obsidian_robot_ore_cost
    blueprint->obsidian_robot_ore_cost = strtol(td->token, NULL, 10);
    td = td->next;
    // skip 2
    td = td->next->next;
    // extract obsidian_robot_clay_cost
    blueprint->obsidian_robot_clay_cost = strtol(td->token, NULL, 10);
    td = td->next;
    // skip 5
    td = td->next->next->next->next->next;
    // extract geode_robot_ore_cost
    blueprint->geode_robot_ore_cost = strtol(td->token, NULL, 10);
    td = td->next;
    // skip 2
    td = td->next->next;
    // extract geode_robot_obsidian_cost
    blueprint->geode_robot_obsidian_cost = strtol(td->token, NULL, 10);
    
#ifdef DEBUG_DAY_19
    printf("Blueprint %d:\n", blueprint->blueprint_id);
    printf("Ore robot costs %d ore\n", blueprint->ore_robot_ore_cost);
    printf("Clay robot costs %d ore\n", blueprint->clay_robot_ore_cost);
    printf("Obsidian robot costs %d ore and %d clay\n", blueprint->obsidian_robot_ore_cost, blueprint->obsidian_robot_clay_cost);
    printf("Geode reobot costs %d ore and %d obsidian\n", blueprint->geode_robot_ore_cost, blueprint->geode_robot_obsidian_cost);
#endif

    for (int i=0; i<DAY_19_MAX_NUM_MINUTES; i++)
    {
        blueprint->max_geode_robots_at_time[i] = 0;
    }
    return TRUE;
}

static int read_and_parse_input(char * filename, day_19_blueprints_t * blueprints)
{
    file_data_t fd;
    
    // read in the input file; space delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, " ", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }
    
    line_data_t * current = fd.head_line;
    while (current != NULL)
    {
        if (parse_blueprint_line(current, &blueprints->blueprints[blueprints->num_blueprints]) == FALSE)
        {
            fprintf(stderr, "Error parsing blueprint occurred on line %d\n", blueprints->num_blueprints+1);
            return FALSE;
        }
        blueprints->num_blueprints++;
        current = current->next;
    }
    file_data_cleanup(&fd);
    return TRUE;
}

static int find_largest_geodes(day_19_state_page_t * head)
{
    int max_num_geodes = 0;
    day_19_state_page_t * current = head;
    while (current != NULL)
    {
        for (int i=0; i<current->num_used_states; i++)
        {
            if (current->states[i].num_geode > max_num_geodes)
            {
                max_num_geodes = current->states[i].num_geode;
            }
        }
        current = current->next;
    }
    return max_num_geodes;
}

static int work_blueprint_for_most_geodes(day_19_blueprint_t * blueprint, int num_minutes)
{
    day_19_state_page_t * current_minute_head_page, * next_minute_head_page, * current_minute_current_page, *next_minute_current_page;
    current_minute_head_page = (day_19_state_page_t *) malloc (sizeof(day_19_state_page_t));
    init_state_page(current_minute_head_page, 0);
    init_state(&current_minute_head_page->states[0]);
    current_minute_head_page->num_used_states = 1;
    
#ifdef DEBUG_DAY_19
    printf("Starting State at Minute 0:\n");
    display_state(&current_minute_head_page->states[0]);
#endif

    int current_minute_page;
    // all but last 2 minutes - do everything
    for (int i=1; i<=num_minutes-2; i++)
    {
        printf("== Minute %d ==\n", i);
        next_minute_head_page = (day_19_state_page_t *) malloc (sizeof(day_19_state_page_t));
        init_state_page(next_minute_head_page, current_minute_head_page->minutes_elapsed+1);
        
        current_minute_current_page = current_minute_head_page;
        next_minute_current_page = next_minute_head_page;
        current_minute_page = 0;
        while (current_minute_current_page != NULL)
        {
#ifdef DEBUG_DAY_19
            printf(" Processing current minute page %d\n", current_minute_page);
#endif
            for (int state = 0; state < current_minute_current_page->num_used_states; state++)
            {
                day_19_state_t * current_minute_state = &current_minute_current_page->states[state];
#ifdef DEBUG_DAY_19
                printf("  working state %d: ", state);
                display_state(current_minute_state);
#endif

                // check for creating geode robot
                if (can_build_geode_robot(current_minute_state, blueprint) == TRUE)
                {
                    day_19_state_t * next_minute_robot = get_next_available_state(&next_minute_current_page);
                    clone_state(next_minute_robot, current_minute_state);
                    accumulate_resources(next_minute_robot);
                    build_geode_robot(next_minute_robot, blueprint, i);
#ifdef DEBUG_DAY_19
                    printf("  created geode robot state: ");
                    display_state(next_minute_robot);
#endif
                }
                else if ((i < MINUTES_TO_CATCH_UP_GEODE_ROBOTS) || current_minute_state->num_geode_robots >= (blueprint->max_geode_robots_at_time[i-MINUTES_TO_CATCH_UP_GEODE_ROBOTS]))
                {
                    // can always do no-op, but don't do it if we can create all four other robots
                    if ((can_build_ore_robot(current_minute_state, blueprint) == FALSE) ||
                        (can_build_clay_robot(current_minute_state, blueprint) == FALSE) ||
                        (can_build_obsidian_robot(current_minute_state, blueprint) == FALSE))
                    {
                        day_19_state_t * next_minute_no_op = get_next_available_state(&next_minute_current_page);
                        clone_state(next_minute_no_op, current_minute_state);
                        accumulate_resources(next_minute_no_op);
#ifdef DEBUG_DAY_19
                        printf("  created no-op state: ");
                        display_state(next_minute_no_op);
#endif
                    }
                    
                    // check for creating ore robot
                    if (can_build_ore_robot(current_minute_state, blueprint) == TRUE)
                    {
                        day_19_state_t * next_minute_robot = get_next_available_state(&next_minute_current_page);
                        clone_state(next_minute_robot, current_minute_state);
                        accumulate_resources(next_minute_robot);
                        build_ore_robot(next_minute_robot, blueprint);
#ifdef DEBUG_DAY_19
                        printf("  created ore robot state: ");
                        display_state(next_minute_robot);
#endif
                    }
        
        
                    // check for creating clay robot
                    if (can_build_clay_robot(current_minute_state, blueprint) == TRUE)
                    {
                        day_19_state_t * next_minute_robot = get_next_available_state(&next_minute_current_page);
                        clone_state(next_minute_robot, current_minute_state);
                        accumulate_resources(next_minute_robot);
                        build_clay_robot(next_minute_robot, blueprint);
#ifdef DEBUG_DAY_19
                        printf("  created clay robot state: ");
                        display_state(next_minute_robot);
#endif
                    }
        
        
        
                    // check for creating obsidian robot
                    if (can_build_obsidian_robot(current_minute_state, blueprint) == TRUE)
                    {
                        day_19_state_t * next_minute_robot = get_next_available_state(&next_minute_current_page);
                        clone_state(next_minute_robot, current_minute_state);
                        accumulate_resources(next_minute_robot);
                        build_obsidian_robot(next_minute_robot, blueprint);
#ifdef DEBUG_DAY_19
                        printf("  created obsidian robot state: ");
                        display_state(next_minute_robot);
#endif
                    }
                }
                
            }
            current_minute_page++;
            current_minute_current_page = current_minute_current_page->next;
        }
        free_state_pages(current_minute_head_page);
        current_minute_head_page = next_minute_head_page;
        printf("The maximum number of geode robots at minute %d is %d\n", i, blueprint->max_geode_robots_at_time[i]);
    }
    
    // Second to last minute - only options that affect output are adding geode robot or mining resources for the last minute
    printf("== Minute %d ==\n", num_minutes - 1);
        
    next_minute_head_page = (day_19_state_page_t *) malloc (sizeof(day_19_state_page_t));
    init_state_page(next_minute_head_page, current_minute_head_page->minutes_elapsed+1);
        
    current_minute_current_page = current_minute_head_page;
    next_minute_current_page = next_minute_head_page;
    current_minute_page = 0;
    while (current_minute_current_page != NULL)
    {
#ifdef DEBUG_DAY_19
        printf(" Processing current minute page %d\n", current_minute_page);
#endif
        for (int state = 0; state < current_minute_current_page->num_used_states; state++)
        {
            day_19_state_t * current_minute_state = &current_minute_current_page->states[state];
#ifdef DEBUG_DAY_19
            printf("  working state %d: ", state);
            display_state(current_minute_state);
#endif

            // check for creating geode robot
            if (can_build_geode_robot(current_minute_state, blueprint) == TRUE)
            {
                day_19_state_t * next_minute_robot = get_next_available_state(&next_minute_current_page);
                clone_state(next_minute_robot, current_minute_state);
                accumulate_resources(next_minute_robot);
                build_geode_robot(next_minute_robot, blueprint, num_minutes-1);
#ifdef DEBUG_DAY_19
                printf("  created geode robot state: ");
                display_state(next_minute_robot);
#endif
            }

            day_19_state_t * next_minute_no_op = get_next_available_state(&next_minute_current_page);
            clone_state(next_minute_no_op, current_minute_state);
            accumulate_resources(next_minute_no_op);
#ifdef DEBUG_DAY_19
            printf("  created no-op state: ");
            display_state(next_minute_no_op);
#endif
                    
        }
        current_minute_page++;
        current_minute_current_page = current_minute_current_page->next;
    }
    free_state_pages(current_minute_head_page);
    current_minute_head_page = next_minute_head_page;

    printf("The maximum number of geode robots at minute %d is %d\n", num_minutes-1, blueprint->max_geode_robots_at_time[num_minutes-1]);


    // All we have to do is mine resources for the last minute
    printf("== Minute %d ==\n", num_minutes);
        
    current_minute_current_page = current_minute_head_page;
    current_minute_page = 0;
    while (current_minute_current_page != NULL)
    {
#ifdef DEBUG_DAY_19
        printf(" Processing current minute page %d\n", current_minute_page);
#endif
        for (int state = 0; state < current_minute_current_page->num_used_states; state++)
        {
            day_19_state_t * current_minute_state = &current_minute_current_page->states[state];
#ifdef DEBUG_DAY_19
            printf("  accumulating state %d: ", state);
            display_state(current_minute_state);
#endif
            accumulate_resources(current_minute_state);
        }
        current_minute_page++;
        current_minute_current_page = current_minute_current_page->next;
    }
    
    int max_geodes = find_largest_geodes(current_minute_head_page);
    free_state_pages(current_minute_head_page);
    
    printf("Maximum number of geodes for blueprint %d is %d\n", blueprint->blueprint_id, max_geodes);
    return max_geodes;
}

void day_19_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_19_blueprints_t blueprints;
    
    init_blueprints(&blueprints);
    if (read_and_parse_input(filename, &blueprints) != TRUE)
    {
        return;
    }
    
    int total_quality = 0;
    for (int i=0; i<blueprints.num_blueprints; i++)
    {
        printf("Working blueprint %d\n", blueprints.blueprints[i].blueprint_id);
        total_quality += (blueprints.blueprints[i].blueprint_id * work_blueprint_for_most_geodes(&blueprints.blueprints[i], PART_1_MINUTES));
    }
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", total_quality);
    
    return;
}
