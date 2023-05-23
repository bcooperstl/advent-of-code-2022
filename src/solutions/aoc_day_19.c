#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_19.h"
#include "file_utils.h"

#define PART_1_MINUTES 24

static void init_state(day_19_state_t * state)
{
    state->minutes_elapsed = 0;
    state->num_ore_robots = 1;
    state->num_clay_robots = 0;
    state->num_obsidian_robots = 0;
    state->num_geode_robots = 0;
    state->num_ore = 0;
    state->num_clay = 0;
    state->num_obsidian = 0;
    state->num_geode = 0;
    state->next = NULL;
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

static void build_geode_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_geode_robots++;
    state->num_ore-=blueprint->geode_robot_ore_cost;
    state->num_obsidian-=blueprint->geode_robot_obsidian_cost;
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

static void advance_minute(day_19_state_t * state)
{
    state->minutes_elapsed++;
    return;
}

static void clone_state(day_19_state_t * to, day_19_state_t * from)
{
    memcpy(to, from, sizeof(day_19_state_t));
    to->next = NULL;
    return;
}

static void display_state(day_19_state_t * state)
{
    printf(" Minutes: %d.   Resources: Ore %d Clay %d Obs %d Geodes %d.   Robots: Ore %d Clay %d Obs %d Geodes %d\n",
            state->minutes_elapsed, 
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

static void delete_state_list(day_19_state_t * head)
{
    day_19_state_t * current = head;
    day_19_state_t * next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    return;
}

static int find_largest_geodes(day_19_state_t * head)
{
    int max_num_geodes = 0;
    day_19_state_t * current = head;
    while (current != NULL)
    {
        if (current->num_geode > max_num_geodes)
        {
            max_num_geodes = current->num_geode;
        }
        current = current->next;
    }
    return max_num_geodes;
}

static int work_blueprint_for_most_geodes(day_19_blueprint_t * blueprint, int num_minutes)
{
    day_19_state_t * current_minute_states, * next_minute_states;
    current_minute_states = (day_19_state_t *) malloc (sizeof(day_19_state_t));
    init_state(current_minute_states);
#ifdef DEBUG_DAY_19
    printf("Starting State at Minute 0:\n");
    display_state(current_minute_states);
#endif

    for (int i=1; i<=num_minutes; i++)
    {
        printf("== Minute %d ==\n", i);
        next_minute_states = NULL;
        day_19_state_t * next_minute_pos = NULL;
        
        day_19_state_t * current_minute_pos = current_minute_states;
        while (current_minute_pos != NULL)
        {
#ifdef DEBUG_DAY_19
            printf(" working state: ");
            display_state(current_minute_pos);
#endif
            // can always do no-op, but don't do it if we can create all four other robots
            if ((can_build_ore_robot(current_minute_pos, blueprint) == FALSE) ||
                (can_build_clay_robot(current_minute_pos, blueprint) == FALSE) ||
                (can_build_obsidian_robot(current_minute_pos, blueprint) == FALSE) ||
                (can_build_geode_robot(current_minute_pos, blueprint) == FALSE))
            {
                day_19_state_t * next_minute_no_op = (day_19_state_t *) malloc (sizeof(day_19_state_t));
                clone_state(next_minute_no_op, current_minute_pos);
                accumulate_resources(next_minute_no_op);
                advance_minute(next_minute_no_op);
#ifdef DEBUG_DAY_19
                printf("  created no-op state: ");
                display_state(next_minute_no_op);
#endif
                if (next_minute_states == NULL)
                {
                    next_minute_states = next_minute_no_op;
                }
                else
                {
                    next_minute_pos->next = next_minute_no_op;
                }
                
                next_minute_pos = next_minute_no_op;
            }
            
            // check for creating ore robot
            if (can_build_ore_robot(current_minute_pos, blueprint) == TRUE)
            {
                day_19_state_t * next_minute_robot = (day_19_state_t *) malloc (sizeof(day_19_state_t));
                clone_state(next_minute_robot, current_minute_pos);
                build_ore_robot(next_minute_robot, blueprint);
                accumulate_resources(next_minute_robot);
                advance_minute(next_minute_robot);
#ifdef DEBUG_DAY_19
                printf("  created ore robot state: ");
                display_state(next_minute_robot);
#endif
                next_minute_pos->next = next_minute_robot;
                next_minute_pos = next_minute_pos->next;
            }


            // check for creating clay robot
            if (can_build_clay_robot(current_minute_pos, blueprint) == TRUE)
            {
                day_19_state_t * next_minute_robot = (day_19_state_t *) malloc (sizeof(day_19_state_t));
                clone_state(next_minute_robot, current_minute_pos);
                build_clay_robot(next_minute_robot, blueprint);
                accumulate_resources(next_minute_robot);
                advance_minute(next_minute_robot);
#ifdef DEBUG_DAY_19
                printf("  created clay robot state: ");
                display_state(next_minute_robot);
#endif
                next_minute_pos->next = next_minute_robot;
                next_minute_pos = next_minute_pos->next;
            }



            // check for creating obsidian robot
            if (can_build_obsidian_robot(current_minute_pos, blueprint) == TRUE)
            {
                day_19_state_t * next_minute_robot = (day_19_state_t *) malloc (sizeof(day_19_state_t));
                clone_state(next_minute_robot, current_minute_pos);
                build_obsidian_robot(next_minute_robot, blueprint);
                accumulate_resources(next_minute_robot);
                advance_minute(next_minute_robot);
#ifdef DEBUG_DAY_19
                printf("  created obsidian robot state: ");
                display_state(next_minute_robot);
#endif
                next_minute_pos->next = next_minute_robot;
                next_minute_pos = next_minute_pos->next;
            }


            // check for creating geode robot
            if (can_build_geode_robot(current_minute_pos, blueprint) == TRUE)
            {
                day_19_state_t * next_minute_robot = (day_19_state_t *) malloc (sizeof(day_19_state_t));
                clone_state(next_minute_robot, current_minute_pos);
                build_geode_robot(next_minute_robot, blueprint);
                accumulate_resources(next_minute_robot);
                advance_minute(next_minute_robot);
#ifdef DEBUG_DAY_19
                printf("  created geode robot state: ");
                display_state(next_minute_robot);
#endif
                next_minute_pos->next = next_minute_robot;
                next_minute_pos = next_minute_pos->next;
            }

            
            current_minute_pos = current_minute_pos->next;
        }
        
        delete_state_list(current_minute_states);
        current_minute_states = next_minute_states;
    }
    
    int max_geodes = find_largest_geodes(current_minute_states);
    delete_state_list(current_minute_states);
    
#ifdef DEBUG_DAY_19
    printf("Maximum number of geodes for blueprint %d is %d\n", blueprint->blueprint_id, max_geodes);
#endif
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
    
    work_blueprint_for_most_geodes(&blueprints.blueprints[0], 8);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", 0);
    
    return;
}