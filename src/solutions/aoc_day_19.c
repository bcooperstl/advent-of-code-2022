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
    if ((state->num_ore >= blueprint->obsidian_robot_ore_cost) && state->num_clay >= blueprint->obsidian_robot_clay_cost))
    {
        return TRUE;
    }
    return FALSE;
}

static int can_build_geode_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    if ((state->num_ore >= blueprint->geode_robot_ore_cost) && state->num_obsidian >= blueprint->geode_robot_obsidian_cost))
    {
        return TRUE;
    }
    return FALSE;
}

static int build_ore_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_ore_robots++;
    state->num_ore-=blueprint->ore_robot_ore_cost;
}

static int build_clay_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_clay_robots++;
    state->num_ore-=blueprint->clay_robot_ore_cost;
}

static int build_obsidian_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_obsidian_robots++;
    state->num_ore-=blueprint->obsidian_robot_ore_cost;
    state->num_clay-=blueprint->obsidian_robot_clay_cost;
}

static int build_geode_robot(day_19_state_t * state, day_19_blueprint_t * blueprint)
{
    state->num_geode_robots++;
    state->num_ore-=blueprint->geode_robot_ore_cost;
    state->num_obsidian-=blueprint->geode_robot_obsidian_cost;
}

static int accumulate_resources(day_19_state_t * state)
{
    state->num_ore+=state->num_ore_robots;
    state->num_clay+=state->num_clay_robots;
    state->num_obsidian+=state->num_obsidian_robots;
    state->num_geode+=state->num_geode_robots;
}

static int advance_minute(day_19_state_t * state)
{
    state->minutes_elapsed++;
}

void day_19_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_19_column_t column;
    
    init_column(&column);

    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    char * jet_pattern = NULL;
    int jet_pattern_length;
    
    // read in the input file with no delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }

    ld = fd.head_line;
    td = ld->head_token;
    jet_pattern = td->token;
    jet_pattern_length = strlen(jet_pattern);
    
    for (int i=1; i<= PART_1_PIECES; i++)
    {
#ifdef DEBUG_DAY_19
    printf("Placing piece %d\n", i);
#endif
        place_piece(&column, jet_pattern, jet_pattern_length);
#ifdef DEBUG_DAY_19
        printf("After placement of piece %d, column looks like:\n", i);
        display_column(&column);
#endif
    }

    file_data_cleanup(&fd);
    snprintf(result, MAX_RESULT_LENGTH+1, "%lld", column.rows_purged + ((long long)(DAY_19_COLUMN_HEIGHT-column.high_row)));
    
    return;
}
