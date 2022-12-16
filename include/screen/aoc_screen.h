#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "file_utils.h"

struct aoc_screen
{
    char ** textmap;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int width;
    int height;
    char start_char;
};

typedef struct aoc_screen aoc_screen_t;

void init_screen(aoc_screen_t * screen, char start_char, int min_x, int max_x, int min_y, int max_y);
void copy_screen(aoc_screen_t * dest, aoc_screen_t * src);
void cleanup_screen(aoc_screen_t * screen);
void load_screen_from_file(aoc_screen_t * screen, file_data_t * fd, char start_char, int min_x, int min_y);
void display_screen(aoc_screen_t * screen);
int equals_screen(aoc_screen_t * first, aoc_screen_t * second);
char screen_get(aoc_screen_t * screen, int x, int y);
void screen_set(aoc_screen_t * screen, int x, int y, char value);
int screen_num_matching_neighbors(aoc_screen_t * screen, int x, int y, char target, int include_diagonals);
int screen_num_matching(aoc_screen_t * screen, char target);
void screen_expand(aoc_screen_t * screen, char expand_char);

void init_screen_defaults(aoc_screen_t * screen);
void load_screen_from_file_defaults(aoc_screen_t * screen, file_data_t * fd);

//void allocate_textmap(char *** textmap, int height, int width);
//void deallocate_textmap(char *** textmap, int height);
//void expand(expand_char)

#endif
