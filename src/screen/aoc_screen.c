#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
#include "aoc_screen.h"

#define DEFAULT_MIN_X -10
#define DEFAULT_MAX_X 10
#define DEFAULT_MIN_Y -10
#define DEFAULT_MAX_Y 10
#define DEFAULT_START_CHAR ' '

/* For the screen, the data is stored in a 2-d list, rows, then columns.
 * This means that unlike algebra, the data is accessed by row and then by column.
 * Also, as per standard Advent of Code problems, row numbers increase from top-to-bottom. Columns increase left-to-right
 */

static void allocate_textmap(char ***textmap, int height, int width, char start_char)
{
    *textmap = (char **)malloc(height * sizeof(char *));
    for (int y=0; y<height; y++)
    {
        (*textmap)[y] = (char *)malloc(width * sizeof(char));
        for (int x=0; x<width; x++)
        {
            (*textmap)[y][x]=start_char;
        }
    }
}

static void deallocate_textmap(char ***textmap, int height)
{
    for (int y=0; y<height; y++)
    {
        free((*textmap)[y]);
    }
    free(*textmap);
}

void init_screen(aoc_screen_t * screen, char start_char, int min_x, int max_x, int min_y, int max_y)
{
    screen->start_char = start_char;
    screen->min_x = min_x;
    screen->max_x = max_x;
    screen->min_y = min_y;
    screen->max_y = max_y;
    screen->width = max_x - min_x + 1; // Need to the add the +1 for the 0 column
    screen->height = max_y - min_y + 1; // Need to the add the +1 for the 0 row
    screen->textmap = NULL;
    allocate_textmap(&screen->textmap, screen->height, screen->width, start_char);
}

void init_screen_defaults(aoc_screen_t * screen)
{
    init_screen(screen, DEFAULT_START_CHAR, DEFAULT_MIN_X, DEFAULT_MAX_X, DEFAULT_MIN_Y, DEFAULT_MAX_Y);
}

void copy_screen(aoc_screen_t * dest, aoc_screen_t * src)
{
    deallocate_textmap(&dest->textmap, dest->height);
    dest->start_char = src->start_char;
    dest->min_x = src->min_x;
    dest->max_x = src->max_x;
    dest->min_y = src->min_y;
    dest->max_y = src->max_y;
    dest->width = src->width;
    dest->height = src->height;
    allocate_textmap(&dest->textmap, dest->height, dest->width, dest->start_char);
    for (int y=0; y<dest->height; y++)
    {
        for (int x=0; x<dest->width; x++)
        {
            dest->textmap[y][x]=src->textmap[y][x];
        }
    }
}

void cleanup_screen(aoc_screen_t * screen)
{
    deallocate_textmap(&screen->textmap, screen->height);
}

void load_screen_from_file(aoc_screen_t * screen, file_data_t * fd, char start_char, int min_x, int min_y)
{
    screen->min_y = min_y;
    screen->height = fd->num_lines;
    screen->max_y = min_y + screen->height - 1;
    
    screen->min_x = min_x;
    screen->width = strlen(fd->head_line->head_token->token);
    screen->max_x = min_x + screen->width - 1;
    
    screen->start_char = start_char;
    
    allocate_textmap(&screen->textmap, screen->height, screen->width, screen->start_char);
    line_data_t * ld = fd->head_line;
    for (int y=0; y<screen->height; y++)
    {
        char * line = ld->head_token->token;
        for (int x=0; x<screen->width; x++)
        {
            screen->textmap[y][x]=line[x];
        }
        ld = ld->next;
    }
}

void load_screen_from_file_defaults(aoc_screen_t * screen, file_data_t * fd)
{
    load_screen_from_file(screen, fd, DEFAULT_START_CHAR, 0, 0);
}

void display_screen(aoc_screen_t * screen)
{
    char * line = (char *)malloc(screen->width+1);
    
    for (int y=0; y<screen->height; y++)
    {
        memcpy(line, screen->textmap[y], screen->height);
        line[screen->height]='\0';
        printf("%s\n", line);
    }
    free(line);
}

int equals_screen(aoc_screen_t * first, aoc_screen_t * second)
{
    if ((first->min_x != second->min_x) ||
        (first->max_x != second->max_x) ||
        (first->min_y != second->min_y) ||
        (first->max_y != second->max_y))
    {
        return FALSE;
    }
    
    for (int y=0; y<first->height; y++)
    {
        for (int x=0; x<first->width; x++)
        {
            if (first->textmap[y][x] != second->textmap[y][x])
            {
                return FALSE;
            }
        }
    }
    
    return TRUE;
}    

char screen_get(aoc_screen_t * screen, int x, int y)
{
    if ((x < screen->min_x) || (x > screen->max_x))
    {
        fprintf(stderr, "***X value %d is out of range (%d,%d)\n", x, screen->min_x, screen->max_x);
        return screen->start_char;
    }
    if ((y < screen->min_y) || (y > screen->max_y))
    {
        fprintf(stderr, "***Y value %d is out of range (%d,%d)\n", y, screen->min_y, screen->max_y);
        return screen->start_char;
    }
    return screen->textmap[y-screen->min_y][x-screen->min_x];
}

void screen_set(aoc_screen_t * screen, int x, int y, char value)
{
    if ((x < screen->min_x) || (x > screen->max_x))
    {
        fprintf(stderr, "***X value %d is out of range (%d,%d)\n", x, screen->min_x, screen->max_x);
        return;
    }
    if ((y < screen->min_y) || (y > screen->max_y))
    {
        fprintf(stderr, "***Y value %d is out of range (%d,%d)\n", y, screen->min_y, screen->max_y);
        return;
    }
    screen->textmap[y-screen->min_y][x-screen->min_x] = value;
}

int screen_num_matching(aoc_screen_t * screen, char target)
{
    int count=0;
    for (int y=0; y<screen->height; y++)
    {
        for (int x=0; x<screen->width; x++)
        {
            if (screen->textmap[y][x]==target)
            {
                count++;
            }
        }
    }
    return count;
}

int screen_num_matching_neighbors(aoc_screen_t * screen, int x, int y, char target, int include_diagonals)
{
    int count=0;
    
    if ((x < screen->min_x) || (x > screen->max_x))
    {
        fprintf(stderr, "***X value %d is out of range (%d,%d)\n", x, screen->min_x, screen->max_x);
        return 0;
    }
    if ((y < screen->min_y) || (y > screen->max_y))
    {
        fprintf(stderr, "***Y value %d is out of range (%d,%d)\n", y, screen->min_y, screen->max_y);
        return 0;
    }
    
    // up
    if (y > screen->min_y)
    {
        if (screen->textmap[y-screen->min_y-1][x-screen->min_x] == target)
        {
            count++;
        }
    }
    
    // down
    if (y < screen->max_y)
    {
        if (screen->textmap[y-screen->min_y+1][x-screen->min_x] == target)
        {
            count++;
        }
    }
    
    // left
    if (x > screen->min_x)
    {
        if (screen->textmap[y-screen->min_y][x-screen->min_x-1] == target)
        {
            count++;
        }
    }
    
    // right
    if (x < screen->max_x)
    {
        if (screen->textmap[y-screen->min_y][x-screen->min_x+1] == target)
        {
            count++;
        }
    }
    
    if (include_diagonals)
    {
        // up-left
        if ((y > screen->min_y) && (x > screen->min_x))
        {
            if (screen->textmap[y-screen->min_y-1][x-screen->min_x-1] == target)
            {
                count++;
            }
        }
        
        // up-right
        if ((y > screen->min_y) && (x < screen->max_x))
        {
            if (screen->textmap[y-screen->min_y-1][x-screen->min_x+1] == target)
            {
                count++;
            }
        }
        
        // down-left
        if ((y < screen->max_y) && (x > screen->min_x))
        {
            if (screen->textmap[y-screen->min_y+1][x-screen->min_x-1] == target)
            {
                count++;
            }
        }
        
        // down-right
        if ((y < screen->max_y) && (x < screen->max_x))
        {
            if (screen->textmap[y-screen->min_y+1][x-screen->min_x+1] == target)
            {
                count++;
            }
        }
    }
    return count;
}

void screen_expand(aoc_screen_t * screen, char expand_char)
{
    char ** old_textmap = screen->textmap;
    
    char ** new_textmap = NULL;
    // allocate everything with the expand character
    allocate_textmap(&new_textmap, screen->height+2, screen->width+2, expand_char);
    
    // copy the existing data
    for (int y=0; y<screen->height; y++)
    {
        for (int x=0; x<screen->width; x++)
        {
            new_textmap[y+1][x+1]=old_textmap[y][x];
        }
    }
    
    deallocate_textmap(&old_textmap, screen->height);
    
    screen->textmap = new_textmap;
    screen->height+=2;
    screen->min_y--;
    screen->max_y++;
    screen->width+=2;
    screen->min_x--;
    screen->max_x++;
}
