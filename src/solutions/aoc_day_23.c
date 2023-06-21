#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_23.h"
#include "file_utils.h"
#include "aoc_screen.h"

// start neighbors at North and work clockwise`
#define ELF_NEIGHBOR_N  0
#define ELF_NEIGHBOR_NE 1
#define ELF_NEIGHBOR_E  2
#define ELF_NEIGHBOR_SE 3
#define ELF_NEIGHBOR_S  4
#define ELF_NEIGHBOR_SW 5
#define ELF_NEIGHBOR_W  6
#define ELF_NEIGHBOR_NW 7

static int parse_input(char * filename, aoc_screen_t * map)
{
    file_data_t fd;

    // read in the input file with no delimiteres
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }

    load_screen_from_file_defaults(map, &fd);

    file_data_cleanup(&fd);

#ifdef DEBUG_DAY_23
    printf("Initial screen:\n");
    display_screen(map);
#endif

    return TRUE;
}

static void init_elves(day_23_elves_t * elves, aoc_screen_t * map)
{
    elves->num_elves = 0;
#ifdef DEBUG_DAY_23
    printf("Searching for elves in initial screen\n");
#endif
    for (int y=map->min_y; y<=map->max_y; y++)
    {
        for (int x=map->min_x; x<=map->max_x; x++)
        {
            if (screen_get(map, x, y) == DAY_23_ELF)
            {
#ifdef DEBUG_DAY_23
                printf("Elf %d found at row=%d col=%d\n", elves->num_elves, y, x);
#endif
                elves->elves[elves->num_elves].pos_y=y;
                elves->elves[elves->num_elves].pos_x=x;
                elves->num_elves++;
            }
        }
    }
#ifdef DEBUG_DAY_23
    printf("There were %d elves found\n", elves->num_elves);
#endif
    return;
};

static void find_elf_neighbors(day_23_elf_t * elf, aoc_screen_t * map)
{
    elf->elf_neighbors[ELF_NEIGHBOR_N]  = ((screen_get(map, elf->pos_x  , elf->pos_y-1) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_NE] = ((screen_get(map, elf->pos_x+1, elf->pos_y-1) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_E]  = ((screen_get(map, elf->pos_x+1, elf->pos_y  ) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_SE] = ((screen_get(map, elf->pos_x+1, elf->pos_y+1) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_S]  = ((screen_get(map, elf->pos_x  , elf->pos_y+1) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_SW] = ((screen_get(map, elf->pos_x-1, elf->pos_y+1) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_W]  = ((screen_get(map, elf->pos_x-1, elf->pos_y  ) == DAY_23_ELF) ? TRUE : FALSE);
    elf->elf_neighbors[ELF_NEIGHBOR_NW] = ((screen_get(map, elf->pos_x-1, elf->pos_y-1) == DAY_23_ELF) ? TRUE : FALSE);
#ifdef DEBUG_DAY_23
    printf("Elf at row=%d, col=%d has neighbors N=%d NE=%d E=%d SE=%d S=%d SW=%d W=%d NW=%d\n", elf->pos_y, elf->pos_x, elf->elf_neighbors[ELF_NEIGHBOR_N], elf->elf_neighbors[ELF_NEIGHBOR_NE],
           elf->elf_neighbors[ELF_NEIGHBOR_E], elf->elf_neighbors[ELF_NEIGHBOR_SE], elf->elf_neighbors[ELF_NEIGHBOR_S], elf->elf_neighbors[ELF_NEIGHBOR_SW],
           elf->elf_neighbors[ELF_NEIGHBOR_W], elf->elf_neighbors[ELF_NEIGHBOR_NW]);
#endif
    return;
}

static int validate_direction_and_calulate_move(day_23_elf_t * elf, int direction)
{
    int result = FALSE;
    switch (direction)
    {
        case DAY_23_DIRECTION_NORTH:
            if ((elf->elf_neighbors[ELF_NEIGHBOR_N]  == FALSE) &&
                (elf->elf_neighbors[ELF_NEIGHBOR_NE] == FALSE) &&
                (elf->elf_neighbors[ELF_NEIGHBOR_NW] == FALSE))
            {
                    elf->proposed_direction = DAY_23_DIRECTION_NORTH;
                    elf->proposed_x = elf->pos_x;
                    elf->proposed_y = elf->pos_y - 1;
#ifdef DEBUG_DAY_23
                    printf("Elf at row=%d, col=%d will propose to move North to row=%d, col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif        
                    result = TRUE;
            }
            break;
        case DAY_23_DIRECTION_SOUTH:
            if ((elf->elf_neighbors[ELF_NEIGHBOR_S]  == FALSE) &&
                    (elf->elf_neighbors[ELF_NEIGHBOR_SE] == FALSE) &&
                    (elf->elf_neighbors[ELF_NEIGHBOR_SW] == FALSE))
            {
                elf->proposed_direction = DAY_23_DIRECTION_SOUTH;
                elf->proposed_x = elf->pos_x;
                elf->proposed_y = elf->pos_y + 1;
#ifdef DEBUG_DAY_23
                printf("Elf at row=%d, col=%d will propose to move South to row=%d, col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif
                result = TRUE;
            }
            break;
            
        case DAY_23_DIRECTION_WEST:
            if ((elf->elf_neighbors[ELF_NEIGHBOR_W]  == FALSE) &&
                    (elf->elf_neighbors[ELF_NEIGHBOR_NW] == FALSE) &&
                    (elf->elf_neighbors[ELF_NEIGHBOR_SW] == FALSE))
            {
                elf->proposed_direction = DAY_23_DIRECTION_WEST;
                elf->proposed_x = elf->pos_x - 1;
                elf->proposed_y = elf->pos_y;
#ifdef DEBUG_DAY_23
                printf("Elf at row=%d, col=%d will propose to move West to row=%d, col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif
                result = TRUE;
            }
            break;

        case DAY_23_DIRECTION_EAST:
            if ((elf->elf_neighbors[ELF_NEIGHBOR_E]  == FALSE) &&
                    (elf->elf_neighbors[ELF_NEIGHBOR_NE] == FALSE) &&
                    (elf->elf_neighbors[ELF_NEIGHBOR_SE] == FALSE))
            {
                elf->proposed_direction = DAY_23_DIRECTION_EAST;
                elf->proposed_x = elf->pos_x + 1;
                elf->proposed_y = elf->pos_y;
#ifdef DEBUG_DAY_23
                printf("Elf at row=%d, col=%d will propose to move East to row=%d, col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif
                result = TRUE;
            }
            break;

        default:
            fprintf(stderr, "***INVALID DIRECTION %d GIVEN\n", direction);
            break;
    }
    return result;
}
    

static void determine_move_from_neighbors(day_23_elf_t * elf, day_23_direction_list_t * directions)
{
    // first - if no neighbors set, stay put
    if ((elf->elf_neighbors[ELF_NEIGHBOR_N]  == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_NE] == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_E]  == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_SE] == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_S]  == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_SW] == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_W]  == FALSE) &&
        (elf->elf_neighbors[ELF_NEIGHBOR_NW] == FALSE))
    {
        elf->proposed_direction = DAY_23_DIRECTION_STAY;
        elf->proposed_x = elf->pos_x;
        elf->proposed_y = elf->pos_y;
#ifdef DEBUG_DAY_23
        printf("Elf at row=%d, col=%d has no neighbors and will stay put at row=%d, col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif
    }
    else
    {
        // has some neighbors. loop over direction list to see what to do
        int did_match_direction = FALSE;
        for (int i=0; i<4; i++)
        {
            did_match_direction = validate_direction_and_calulate_move(elf, directions->directions[i]);
            if (did_match_direction == TRUE)
            {
                break;
            }
        }
        if (did_match_direction == FALSE)
        {
            elf->proposed_direction = DAY_23_DIRECTION_STAY;
            elf->proposed_x = elf->pos_x;
            elf->proposed_y = elf->pos_y;
#ifdef DEBUG_DAY_23
            printf("Elf at row=%d, col=%d has no moves and will stay put at row=%d, col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif
        }
    }
    return;
}

static void remove_colliding_moves(day_23_elves_t * elves)
{
#ifdef DEBUG_DAY_23
    printf("Checking for colliding moves\n");
#endif
    // mark all moves as valid
    for (int i=0; i<elves->num_elves; i++)
    {
        elves->elves[i].can_move = TRUE;
    }
    
    for (int left=0; left<elves->num_elves-1; left++)
    {
        for (int right=left+1; right<elves->num_elves; right++)
        {
            if ((elves->elves[left].proposed_x == elves->elves[right].proposed_x) &&
                (elves->elves[left].proposed_y == elves->elves[right].proposed_y))
            {
                elves->elves[left].can_move = FALSE;
                elves->elves[right].can_move = FALSE;
#ifdef DEBUG_DAY_23
                printf("Elves at row=%d, col=%d and row=%d, col=%d both propose moves to row=%d %d col=%d %d. Setting both to not make their moves\n", 
                        elves->elves[left].pos_y, elves->elves[left].pos_x,
                        elves->elves[right].pos_y, elves->elves[right].pos_x,
                        elves->elves[left].proposed_y, elves->elves[right].proposed_y, 
                        elves->elves[left].proposed_x, elves->elves[right].proposed_x);
#endif
            }
        }
    }
    return;
}

static void move_elf(day_23_elf_t * elf, aoc_screen_t * map)
{
    // turn the elf off at the current position and on at the new position
    screen_set(map, elf->pos_x, elf->pos_y, DAY_23_EMPTY);
    screen_set(map, elf->proposed_x, elf->proposed_y, DAY_23_ELF);

#ifdef DEBUG_DAY_23
    printf("Moved Elf from row=%d col=%d to row=%d col=%d\n", elf->pos_y, elf->pos_x, elf->proposed_y, elf->proposed_x);
#endif

    elf->pos_x = elf->proposed_x;
    elf->pos_y = elf->proposed_y;
    return;
}

void play_round(day_23_elves_t * elves, aoc_screen_t * map, day_23_direction_list_t * directions)
{
#ifdef DEBUG_DAY_23
    printf("Playing a round\n");
#endif
    // first step - find neighbors
    for (int i=0; i<elves->num_elves; i++)
    {
        find_elf_neighbors(&elves->elves[i], map);
        determine_move_from_neighbors(&elves->elves[i], directions);
    }
    
    // second step - remove colliding moves and then perform moves
    remove_colliding_moves(elves);
    
    for (int i=0; i<elves->num_elves; i++)
    {
        if ((elves->elves[i].can_move == TRUE) && (elves->elves[i].proposed_direction != DAY_23_DIRECTION_STAY))
        {
            move_elf(&elves->elves[i], map);
        }
    }
    
    return;
}

static void check_and_expand_map(aoc_screen_t * map)
{
    int should_expand = FALSE;
    // check top and bottom rows
    for (int x=map->min_x; x<=map->max_x; x++)
    {
        if (screen_get(map, x, map->min_y) == DAY_23_ELF)
        {
            should_expand = TRUE;
            break;
        }
        if (screen_get(map, x, map->max_y) == DAY_23_ELF)
        {
            should_expand = TRUE;
            break;
        }
    }
    
    // check left and right columns;
    if (should_expand == FALSE)
    {
        for (int y=map->min_y; y<=map->max_y; y++)
        {
            if (screen_get(map, map->min_x, y) == DAY_23_ELF)
            {
                should_expand = TRUE;
                break;
            }
            if (screen_get(map, map->max_x, y) == DAY_23_ELF)
            {
                should_expand = TRUE;
                break;
            }
        }
    }
    
    if (should_expand == TRUE)
    {
        screen_expand(map, DAY_23_EMPTY);
    }
    return;
}

static int calculate_empty_in_region(day_23_elves_t * elves)
{
    int min_x = elves->elves[0].pos_x;
    int max_x = elves->elves[0].pos_x;
    int min_y = elves->elves[0].pos_y;
    int max_y = elves->elves[0].pos_y;

    for (int i=1; i<elves->num_elves; i++)
    {
        if (elves->elves[i].pos_x < min_x)
        {
            min_x = elves->elves[i].pos_x;
        }
        if (elves->elves[i].pos_x > max_x)
        {
            max_x = elves->elves[i].pos_x;
        }
        if (elves->elves[i].pos_y < min_y)
        {
            min_y = elves->elves[i].pos_y;
        }
        if (elves->elves[i].pos_y > max_y)
        {
            max_y = elves->elves[i].pos_y;
        }
    }
    
#ifdef DEBUG_DAY_23
    printf("There are %d elves covering %d rows %d to %d and %d column %d to %d. This results in an area of %d and num empty of %d\n",
            elves->num_elves, (max_y - min_y + 1), min_y, max_y, (max_x - min_x + 1), min_x, max_x,
            (max_x - min_x + 1)*(max_y - min_y + 1),
            (max_x - min_x + 1)*(max_y - min_y + 1) - elves->num_elves);
#endif
    
    return ((max_x - min_x + 1)*(max_y - min_y + 1))-elves->num_elves;
}

void advance_direction_list(day_23_direction_list_t * directions)
{
    int temp = directions->directions[0];
    directions->directions[0] = directions->directions[1];
    directions->directions[1] = directions->directions[2];
    directions->directions[2] = directions->directions[3];
    directions->directions[3] = temp;
}

void day_23_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_23_elves_t elves;
    aoc_screen_t map;
    day_23_direction_list_t directions;
    directions.directions[0] = DAY_23_DIRECTION_NORTH;
    directions.directions[1] = DAY_23_DIRECTION_SOUTH;
    directions.directions[2] = DAY_23_DIRECTION_WEST;
    directions.directions[3] = DAY_23_DIRECTION_EAST;
    

    if (parse_input(filename, &map) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }

    init_elves(&elves, &map);

    check_and_expand_map(&map);

#ifdef DEBUG_DAY_23
    printf("Expanded screen:\n");
    display_screen(&map);
#endif

    for (int i=0; i<10; i++)
    {
        play_round(&elves, &map, &directions);
        check_and_expand_map(&map);
#ifdef DEBUG_DAY_23
        printf("After round %d:\n", i+1);
        display_screen(&map);
#endif
        advance_direction_list(&directions);
    }


    snprintf(result, MAX_RESULT_LENGTH+1, "%d", calculate_empty_in_region(&elves));

    cleanup_screen(&map);

    return;
}
