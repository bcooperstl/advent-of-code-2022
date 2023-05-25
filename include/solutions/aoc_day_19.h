#ifndef __AOC_DAY_19__
#define __AOC_DAY_19__

#include <stdint.h>

#include "constants.h"

#define DAY_19_MAX_BLUEPRINTS 30
#define DAY_19_STATES_PER_PAGE 65536
#define DAY_19_MAX_NUM_MINUTES 36

struct day_19_blueprint
{
    int blueprint_id;
    int ore_robot_ore_cost;
    int clay_robot_ore_cost;
    int obsidian_robot_ore_cost;
    int obsidian_robot_clay_cost;
    int geode_robot_ore_cost;
    int geode_robot_obsidian_cost;
    int max_geode_robots_at_time[DAY_19_MAX_NUM_MINUTES];
};

typedef struct day_19_blueprint day_19_blueprint_t;

struct day_19_blueprints
{
    day_19_blueprint_t blueprints[DAY_19_MAX_BLUEPRINTS];
    int num_blueprints;
};

typedef struct day_19_blueprints day_19_blueprints_t;

struct day_19_state
{
    unsigned char num_ore_robots;
    unsigned char num_clay_robots;
    unsigned char num_obsidian_robots;
    unsigned char num_geode_robots;
    unsigned char num_ore;
    unsigned char num_clay;
    unsigned char num_obsidian;
    unsigned char num_geode;
};

typedef struct day_19_state day_19_state_t;

struct day_19_state_page;

struct day_19_state_page
{
    day_19_state_t states[DAY_19_STATES_PER_PAGE];
    int num_used_states;
    int minutes_elapsed;
    struct day_19_state_page * next;
};

typedef struct day_19_state_page day_19_state_page_t;

void day_19_part_1(char * filename, extra_args_t * extra_args, char * result);
void day_19_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
