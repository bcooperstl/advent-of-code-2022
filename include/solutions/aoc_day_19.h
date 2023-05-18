#ifndef __AOC_DAY_19__
#define __AOC_DAY_19__

#include <stdint.h>

#include "constants.h"

#define DAY_19_MAX_BLUEPRINTS 30

struct day_19_blueprint
{
    int blueprint_id;
    int ore_robot_ore_cost;
    int clay_robot_ore_cost;
    int obsidian_robot_ore_cost;
    int obsidian_robot_clay_cost;
    int geode_robot_ore_cost;
    int geode_robot_obsidian_cost;
};

typedef struct day_19_blueprint day_19_blueprint_t;

struct day_19_blueprints
{
    day_19_blueprint_t blueprints[DAY_19_MAX_BLUEPRINTS];
    int num_blueprints;
};

typedef struct day_19_blueprints day_19_blueprints_t;

struct day_19_state;

struct day_19_state
{
    int minutes_elapsed;
    int num_ore_robots;
    int num_clay_robots;
    int num_obsidian_robots;
    int num_geode_robots;
    int num_ore;
    int num_clay;
    int num_obsidian;
    int num_geode;
    struct day_19_state * next;
};

typedef struct day_19_state day_19_state_t;

void day_19_part_1(char * filename, extra_args_t * extra_args, char * result);
//void day_19_part_2(char * filename, extra_args_t * extra_args, char * result);

#endif
