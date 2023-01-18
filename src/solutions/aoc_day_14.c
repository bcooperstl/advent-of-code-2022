#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_14.h"
#include "file_utils.h"
#include "aoc_screen.h"

#define PIXEL_WALL '#'
#define PIXEL_SAND 'o'
#define PIXEL_AIR '.'
#define PIXEL_START '+'

static void determine_min_max_area(line_data_t * ld, int * min_x, int * max_x, int * min_y, int * max_y)
{
    // sand flows from 500,0, so define that as the initial starting area
    *min_x = 500;
    *max_x = 500;
    *min_y = 0;
    *max_y = 0;
    
    while (ld != NULL)
    {
        token_data_t * token_x = ld->head_token;
        token_data_t * token_y = token_x->next;
        token_data_t * arrow = token_y->next;
        while (token_x != NULL)
        {
#ifdef DEBUG_DAY_14
            printf("Checking (%s,%s) for new min/max values\n", token_x->token, token_y->token);
#endif
            int x = strtol(token_x->token, NULL, 10);
            int y = strtol(token_y->token, NULL, 10);
            if (x < *min_x)
            {
#ifdef DEBUG_DAY_14
                printf(" Updated min_x from %d to %d\n", *min_x, x);
#endif
                *min_x = x;
            }
            if (x > *max_x)
            {
#ifdef DEBUG_DAY_14
                printf(" Updated max_x from %d to %d\n", *max_x, x);
#endif
                *max_x = x;
            }
            if (y > *max_y)
            {
#ifdef DEBUG_DAY_14
                printf(" Updated max_y from %d to %d\n", *max_y, y);
#endif
                *max_y = y;
            }
            if (arrow != NULL)
            {
                token_x = arrow->next;
                token_y = token_x->next;
                arrow = token_y->next;
            }
            else
            {
                token_x = NULL;
            }
        }
        
        ld = ld->next;
    }
    return;
}

static void map_line(aoc_screen_t * screen, line_data_t * ld)
{
    token_data_t * token_x = ld->head_token;
    token_data_t * token_y = token_x->next;
    token_data_t * arrow = token_y->next;
    int x = strtol(token_x->token, NULL, 10);
    int y = strtol(token_y->token, NULL, 10);
    screen_set(screen, x, y, PIXEL_WALL);
    while (arrow != NULL)
    {
        token_x = arrow->next;
        token_y = token_x->next;
        arrow = token_y->next;
        
        int next_x = strtol(token_x->token, NULL, 10);
        int next_y = strtol(token_y->token, NULL, 10);
#ifdef DEBUG_DAY_14
        printf("Plotting line from (%d,%d) to (%d,%d)\n", x, y, next_x, next_y);
#endif
        if (x == next_x) // will do vertical line
        {
            if (next_y > y) // move down
            {
                while (y != next_y)
                {
                    y++;
                    screen_set(screen, x, y, PIXEL_WALL);
                }
            }
            else // move up
            {
                while (y != next_y)
                {
                    y--;
                    screen_set(screen, x, y, PIXEL_WALL);
                }
            }
        }
        else if (y == next_y)
        {
            if (next_x > x) // move right
            {
                while (x != next_x)
                {
                    x++;
                    screen_set(screen, x, y, PIXEL_WALL);
                }
            }
            else // move left
            {
                while (x != next_x)
                {
                    x--;
                    screen_set(screen, x, y, PIXEL_WALL);
                }
            }
        }
#ifdef DEBUG_DAY_14
        display_screen(screen);
#endif
    }
    return;
}

#define DROP_PLACED 1
#define DROP_OVERFLOW 2

static int drop_one_sand(aoc_screen_t * screen, int max_y)
{
    int x = 500;
    int y = 0;
    while (y != max_y)
    {
        if (screen_get(screen, x, y+1) == PIXEL_AIR) // check below
        {
            y+=1;
        }
        else if (screen_get(screen, x-1, y+1) == PIXEL_AIR) // check down-left
        {
            x-=1;
            y+=1;
        }
        else if (screen_get(screen, x+1, y+1) == PIXEL_AIR) // check down-right
        {
            x+=1;
            y+=1;
        }
        else
        {
            screen_set(screen,x,y,PIXEL_SAND);
#ifdef DEBUG_DAY_14
            printf("Placing grain of sand at (%d,%d)\n", x, y);
            display_screen(screen);
#endif
            return DROP_PLACED;
        }
    }
    
#ifdef DEBUG_DAY_14
    printf("Unable to place grain of sand; sand has overflowed.\n");
#endif
    return DROP_OVERFLOW;    
}

void day_14_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    file_data_t fd;
    line_data_t * ld;
    
    // read in the input file with space and comma delimiteres
    file_data_init(&fd);
    file_data_read_file(&fd, filename, " ,", 2, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }

    ld = fd.head_line;
    
    int min_x, max_x, min_y, max_y;
    determine_min_max_area(ld, &min_x, &max_x, &min_y, &max_y);
    
    aoc_screen_t screen;
    init_screen(&screen, PIXEL_AIR, min_x-1, max_x+1, min_y, max_y);
    
    while (ld != NULL)
    {
        map_line(&screen, ld);
        ld = ld->next;
    }
    
    int count = 0;
    while (drop_one_sand(&screen, max_y) == DROP_PLACED)
    {
        count++;
    }
    
    display_screen(&screen);
    file_data_cleanup(&fd);
        
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", count);
    
    return;
}
