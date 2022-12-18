#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_9.h"
#include "file_utils.h"
#include "aoc_screen.h"

#define STATE_1 0 // tail is northwest of head
#define STATE_2 1 // tail is north of head
#define STATE_3 2 // tail is northeast of head
#define STATE_4 3 // tail is west of head
#define STATE_5 4 // tail is same as head
#define STATE_6 5 // tail is east of head
#define STATE_7 6 // tail is southwest of head
#define STATE_8 7 // tail is south of head
#define STATE_9 8 // tail is southeast of head

#define UP 0 // up
#define DOWN 1 // down
#define LEFT 2 // left
#define RIGHT 3 // right

#define EMPTY '.'
#define HEAD 'H'
#define TAIL 'T'
#define BOTH '*'

#define MT2_OFFSET 2

static int build_move_table(day_9_move_table_t * mt);
static int build_move_table_2(day_9_move_table_2_t * mt);


static int parse_input(char * filename, day_9_instructions_t * instructions)
{
    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with space delimiter
    file_data_init(&fd);
    file_data_read_file(&fd, filename, " ", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }

    ld = fd.head_line;
    
    instructions->num_instructions = 0;
    
    while (ld != NULL)
    {
        td = ld->head_token;
        switch (td->token[0])
        {
            case 'U':
                instructions->instructions[instructions->num_instructions].direction=UP;
                break;
            case 'D':
                instructions->instructions[instructions->num_instructions].direction=DOWN;
                break;
            case 'L':
                instructions->instructions[instructions->num_instructions].direction=LEFT;
                break;
            case 'R':
                instructions->instructions[instructions->num_instructions].direction=RIGHT;
                break;
            default:
                fprintf(stderr, "***INVALID DIRECTION %s found\n", td->token);
                break;
        }
        instructions->instructions[instructions->num_instructions].num_steps = strtol(td->next->token, NULL, 10);
        instructions->num_instructions++;
        ld = ld->next;
    }
#ifdef DEBUG_DAY_9
    printf("There were %d instructions parsed\n", instructions->num_instructions);
#endif

    file_data_cleanup(&fd);
    
    return TRUE;
}

void day_9_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_9_move_table_t mt;
    build_move_table(&mt);
    
    day_9_instructions_t instructions;
    if (parse_input(filename, &instructions) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    aoc_screen_t current;
    aoc_screen_t tail_history;
    
    init_screen(&current, EMPTY, -5, 5, -5 ,5);
    init_screen(&tail_history, EMPTY, -5, 5, -5, 5);
    
    int head_x = 0;
    int head_y = 0;
    int tail_x = 0;
    int tail_y = 0;
    int state = STATE_5;
    
    
    screen_set(&current, head_x, head_y, BOTH);
    screen_set(&tail_history, tail_x, tail_y, TAIL);

#ifdef DEBUG_DAY_9
    printf("Head is at (%d,%d). Tail is at (%d,%d). State number is %d\n", head_x, head_y, tail_x, tail_y, state+1);
    printf("Current:\n");
    display_screen(&current);
    printf("Tail history:\n");
    display_screen(&tail_history);
#endif
    
    for (int i=0; i<instructions.num_instructions; i++)
    {
#ifdef DEBUG_DAY_9
        printf("Instruction %d is to move %d steps in direction %d\n", i, instructions.instructions[i].num_steps, instructions.instructions[i].direction);
#endif
        for (int step=0; step<instructions.instructions[i].num_steps; step++)
        {
            day_9_move_t move = mt.moves[state][instructions.instructions[i].direction];
#ifdef DEBUG_DAY_9
            printf("Move table results in head changes (%d,%d) tail changes (%d,%d) new state number %d\n", move.head_delta_x, move.head_delta_y, move.tail_delta_x, move.tail_delta_y, move.next_state + 1);
#endif
            
            screen_set(&current, head_x, head_y, EMPTY);
            screen_set(&current, tail_x, tail_y, EMPTY);
            head_x += move.head_delta_x;
            head_y += move.head_delta_y;
            tail_x += move.tail_delta_x;
            tail_y += move.tail_delta_y;
            state = move.next_state;
            if ((head_x > current.max_x) || 
                (head_x < current.min_x) ||
                (head_y > current.max_y) || 
                (head_y < current.min_y) ||
                (tail_x > current.max_x) || 
                (tail_x < current.min_x) ||
                (tail_y > current.max_y) || 
                (tail_y < current.min_y))
            {
                screen_expand(&current, EMPTY);
                screen_expand(&tail_history, EMPTY);
            }
            if ((head_x == tail_x) && (head_y == tail_y))
            {
                screen_set(&current, head_x, head_y, BOTH);
            }
            else
            {
                screen_set(&current, head_x, head_y, HEAD);
                screen_set(&current, tail_x, tail_y, TAIL);
            }
            screen_set(&tail_history, tail_x, tail_y, TAIL);
#ifdef DEBUG_DAY_9
            printf(" Head is at (%d,%d). Tail is at (%d,%d). State number is %d\n", head_x, head_y, tail_x, tail_y, state+1);
            printf("Current:\n");
            display_screen(&current);
            printf("Tail history:\n");
            display_screen(&tail_history);
#endif
        }
    }

    snprintf(result, MAX_RESULT_LENGTH+1, "%d", screen_num_matching(&tail_history, TAIL));
    
    cleanup_screen(&current);
    cleanup_screen(&tail_history);
    
    return;
}


void day_9_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_9_move_table_2_t mt;
    build_move_table_2(&mt);
    
    day_9_instructions_t instructions;
    if (parse_input(filename, &instructions) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
    aoc_screen_t current;
    aoc_screen_t tail_history;
    
    day_9_position_t positions[10];
    char labels[10]="H123456789";
    
    init_screen(&current, EMPTY, -5, 5, -5 ,5);
    init_screen(&tail_history, EMPTY, -5, 5, -5, 5);
    
    for (int i=0; i<10; i++)
    {
        positions[i].x=0;
        positions[i].y=0;
    }
    
    for (int i=9; i>=0; i--)
    {
        screen_set(&current, positions[i].x, positions[i].y, labels[i]);
    }
    screen_set(&tail_history, positions[9].x, positions[9].y, TAIL);

#ifdef DEBUG_DAY_9
    printf("At start: Head is at (%d,%d). Tail is at (%d,%d).\n", positions[0].x, positions[0].y, positions[9].x, positions[9].y);
    printf("Current:\n");
    display_screen(&current);
    printf("Tail history:\n");
    display_screen(&tail_history);
#endif
    
    for (int i=0; i<instructions.num_instructions; i++)
    {
#ifdef DEBUG_DAY_9
        printf("Instruction %d is to move %d steps in direction %d\n", i, instructions.instructions[i].num_steps, instructions.instructions[i].direction);
#endif
        for (int step=0; step<instructions.instructions[i].num_steps; step++)
        {
            // clear the current screen
            for (int j=0; j<=9; j++)
            {
                screen_set(&current, positions[j].x, positions[j].y, EMPTY);
            }

            // move head
            switch (instructions.instructions[i].direction)
            {
                case UP:
                    positions[0].y--;
                    break;
                case DOWN:
                    positions[0].y++;
                    break;
                case LEFT:
                    positions[0].x--;
                    break;
                case RIGHT:
                    positions[0].x++;
                    break;
            }

#ifdef DEBUG_DAY_9
            printf(" Head moved to (%d,%d)\n", positions[0].x, positions[0].y);
#endif
            // loop over all 9 knots, to make their movements
            for (int j=1; j<=9; j++)
            {
                // move_table is indexed by:
                //  1) tx-hx+2 (the offset so all are positive)
                //  2) ty-hy+2 (the offset so all are position)
                day_9_move_2_t mv = mt.moves[positions[j].x-positions[j-1].x+MT2_OFFSET][positions[j].y-positions[j-1].y+MT2_OFFSET];
                
                // if a not doesn't move, any after it wont' move, so short circuit here
                if (mv.is_noop)
                {
                    break;
                }
                
                positions[j].x+=mv.delta_x;
                positions[j].y+=mv.delta_y;
#ifdef DEBUG_DAY_9
                printf(" Follower %d moved to (%d,%d)\n", j, positions[j].x, positions[j].y);
#endif
            }

            // head will have to need to be expanded for any to need to be expanded
            if ((positions[0].x > current.max_x) || 
                (positions[0].x < current.min_x) ||
                (positions[0].y > current.max_y) || 
                (positions[0].y < current.min_y))
            {
                screen_expand(&current, EMPTY);
                screen_expand(&tail_history, EMPTY);
            }
            
            for (int j=9; j>=0; j--)
            {
                screen_set(&current, positions[j].x, positions[j].y, labels[j]);
            }
            screen_set(&tail_history, positions[9].x, positions[9].y, TAIL);
            
#ifdef DEBUG_DAY_9
            printf("Current:\n");
            display_screen(&current);
            printf("Tail history:\n");
            display_screen(&tail_history);
#endif
        }
    }

    snprintf(result, MAX_RESULT_LENGTH+1, "%d", screen_num_matching(&tail_history, TAIL));
    
    cleanup_screen(&current);
    cleanup_screen(&tail_history);
    
    return;
}



/* For the screen, the data is stored in a 2-d list, rows, then columns.
 * This means that unlike algebra, the data is accessed by row and then by column.
 * Also, as per standard Advent of Code problems, row numbers increase from top-to-bottom. Columns increase left-to-right
 */
static int build_move_table(day_9_move_table_t * mt)
{
    mt->moves[STATE_1][UP].head_delta_x = 0;
    mt->moves[STATE_1][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_1][UP].tail_delta_x = 0;
    mt->moves[STATE_1][UP].tail_delta_y = 0;
    mt->moves[STATE_1][UP].next_state   = STATE_4;
    mt->moves[STATE_1][DOWN].head_delta_x = 0;
    mt->moves[STATE_1][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_1][DOWN].tail_delta_x = 1;
    mt->moves[STATE_1][DOWN].tail_delta_y = 1;
    mt->moves[STATE_1][DOWN].next_state   = STATE_2;
    mt->moves[STATE_1][LEFT].head_delta_x = -1;
    mt->moves[STATE_1][LEFT].head_delta_y = 0;
    mt->moves[STATE_1][LEFT].tail_delta_x = 0;
    mt->moves[STATE_1][LEFT].tail_delta_y = 0;
    mt->moves[STATE_1][LEFT].next_state   = STATE_2;
    mt->moves[STATE_1][RIGHT].head_delta_x = 1;
    mt->moves[STATE_1][RIGHT].head_delta_y = 0;
    mt->moves[STATE_1][RIGHT].tail_delta_x = 1;
    mt->moves[STATE_1][RIGHT].tail_delta_y = 1;
    mt->moves[STATE_1][RIGHT].next_state   = STATE_4;
    
    mt->moves[STATE_2][UP].head_delta_x = 0;
    mt->moves[STATE_2][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_2][UP].tail_delta_x = 0;
    mt->moves[STATE_2][UP].tail_delta_y = 0;
    mt->moves[STATE_2][UP].next_state   = STATE_5;
    mt->moves[STATE_2][DOWN].head_delta_x = 0;
    mt->moves[STATE_2][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_2][DOWN].tail_delta_x = 0;
    mt->moves[STATE_2][DOWN].tail_delta_y = 1;
    mt->moves[STATE_2][DOWN].next_state   = STATE_2;
    mt->moves[STATE_2][LEFT].head_delta_x = -1;
    mt->moves[STATE_2][LEFT].head_delta_y = 0;
    mt->moves[STATE_2][LEFT].tail_delta_x = 0;
    mt->moves[STATE_2][LEFT].tail_delta_y = 0;
    mt->moves[STATE_2][LEFT].next_state   = STATE_3;
    mt->moves[STATE_2][RIGHT].head_delta_x = 1;
    mt->moves[STATE_2][RIGHT].head_delta_y = 0;
    mt->moves[STATE_2][RIGHT].tail_delta_x = 0;
    mt->moves[STATE_2][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_2][RIGHT].next_state   = STATE_1;
    
    mt->moves[STATE_3][UP].head_delta_x = 0;
    mt->moves[STATE_3][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_3][UP].tail_delta_x = 0;
    mt->moves[STATE_3][UP].tail_delta_y = 0;
    mt->moves[STATE_3][UP].next_state   = STATE_6;
    mt->moves[STATE_3][DOWN].head_delta_x = 0;
    mt->moves[STATE_3][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_3][DOWN].tail_delta_x = -1;
    mt->moves[STATE_3][DOWN].tail_delta_y = 1;
    mt->moves[STATE_3][DOWN].next_state   = STATE_2;
    mt->moves[STATE_3][LEFT].head_delta_x = -1;
    mt->moves[STATE_3][LEFT].head_delta_y = 0;
    mt->moves[STATE_3][LEFT].tail_delta_x = -1;
    mt->moves[STATE_3][LEFT].tail_delta_y = 1;
    mt->moves[STATE_3][LEFT].next_state   = STATE_6;
    mt->moves[STATE_3][RIGHT].head_delta_x = 1;
    mt->moves[STATE_3][RIGHT].head_delta_y = 0;
    mt->moves[STATE_3][RIGHT].tail_delta_x = 0;
    mt->moves[STATE_3][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_3][RIGHT].next_state   = STATE_2;
    
    mt->moves[STATE_4][UP].head_delta_x = 0;
    mt->moves[STATE_4][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_4][UP].tail_delta_x = 0;
    mt->moves[STATE_4][UP].tail_delta_y = 0;
    mt->moves[STATE_4][UP].next_state   = STATE_7;
    mt->moves[STATE_4][DOWN].head_delta_x = 0;
    mt->moves[STATE_4][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_4][DOWN].tail_delta_x = 0;
    mt->moves[STATE_4][DOWN].tail_delta_y = 0;
    mt->moves[STATE_4][DOWN].next_state   = STATE_1;
    mt->moves[STATE_4][LEFT].head_delta_x = -1;
    mt->moves[STATE_4][LEFT].head_delta_y = 0;
    mt->moves[STATE_4][LEFT].tail_delta_x = 0;
    mt->moves[STATE_4][LEFT].tail_delta_y = 0;
    mt->moves[STATE_4][LEFT].next_state   = STATE_5;
    mt->moves[STATE_4][RIGHT].head_delta_x = 1;
    mt->moves[STATE_4][RIGHT].head_delta_y = 0;
    mt->moves[STATE_4][RIGHT].tail_delta_x = 1;
    mt->moves[STATE_4][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_4][RIGHT].next_state   = STATE_4;
    
    mt->moves[STATE_5][UP].head_delta_x = 0;
    mt->moves[STATE_5][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_5][UP].tail_delta_x = 0;
    mt->moves[STATE_5][UP].tail_delta_y = 0;
    mt->moves[STATE_5][UP].next_state   = STATE_8;
    mt->moves[STATE_5][DOWN].head_delta_x = 0;
    mt->moves[STATE_5][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_5][DOWN].tail_delta_x = 0;
    mt->moves[STATE_5][DOWN].tail_delta_y = 0;
    mt->moves[STATE_5][DOWN].next_state   = STATE_2;
    mt->moves[STATE_5][LEFT].head_delta_x = -1;
    mt->moves[STATE_5][LEFT].head_delta_y = 0;
    mt->moves[STATE_5][LEFT].tail_delta_x = 0;
    mt->moves[STATE_5][LEFT].tail_delta_y = 0;
    mt->moves[STATE_5][LEFT].next_state   = STATE_6;
    mt->moves[STATE_5][RIGHT].head_delta_x = 1;
    mt->moves[STATE_5][RIGHT].head_delta_y = 0;
    mt->moves[STATE_5][RIGHT].tail_delta_x = 0;
    mt->moves[STATE_5][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_5][RIGHT].next_state   = STATE_4;
    
    mt->moves[STATE_6][UP].head_delta_x = 0;
    mt->moves[STATE_6][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_6][UP].tail_delta_x = 0;
    mt->moves[STATE_6][UP].tail_delta_y = 0;
    mt->moves[STATE_6][UP].next_state   = STATE_9;
    mt->moves[STATE_6][DOWN].head_delta_x = 0;
    mt->moves[STATE_6][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_6][DOWN].tail_delta_x = 0;
    mt->moves[STATE_6][DOWN].tail_delta_y = 0;
    mt->moves[STATE_6][DOWN].next_state   = STATE_3;
    mt->moves[STATE_6][LEFT].head_delta_x = -1;
    mt->moves[STATE_6][LEFT].head_delta_y = 0;
    mt->moves[STATE_6][LEFT].tail_delta_x = -1;
    mt->moves[STATE_6][LEFT].tail_delta_y = 0;
    mt->moves[STATE_6][LEFT].next_state   = STATE_6;
    mt->moves[STATE_6][RIGHT].head_delta_x = 1;
    mt->moves[STATE_6][RIGHT].head_delta_y = 0;
    mt->moves[STATE_6][RIGHT].tail_delta_x = 0;
    mt->moves[STATE_6][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_6][RIGHT].next_state   = STATE_5;
    
    mt->moves[STATE_7][UP].head_delta_x = 0;
    mt->moves[STATE_7][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_7][UP].tail_delta_x = 1;
    mt->moves[STATE_7][UP].tail_delta_y = -1;
    mt->moves[STATE_7][UP].next_state   = STATE_8;
    mt->moves[STATE_7][DOWN].head_delta_x = 0;
    mt->moves[STATE_7][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_7][DOWN].tail_delta_x = 0;
    mt->moves[STATE_7][DOWN].tail_delta_y = 0;
    mt->moves[STATE_7][DOWN].next_state   = STATE_4;
    mt->moves[STATE_7][LEFT].head_delta_x = -1;
    mt->moves[STATE_7][LEFT].head_delta_y = 0;
    mt->moves[STATE_7][LEFT].tail_delta_x = 0;
    mt->moves[STATE_7][LEFT].tail_delta_y = 0;
    mt->moves[STATE_7][LEFT].next_state   = STATE_8;
    mt->moves[STATE_7][RIGHT].head_delta_x = 1;
    mt->moves[STATE_7][RIGHT].head_delta_y = 0;
    mt->moves[STATE_7][RIGHT].tail_delta_x = 1;
    mt->moves[STATE_7][RIGHT].tail_delta_y = -1;
    mt->moves[STATE_7][RIGHT].next_state   = STATE_4;
    
    mt->moves[STATE_8][UP].head_delta_x = 0;
    mt->moves[STATE_8][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_8][UP].tail_delta_x = 0;
    mt->moves[STATE_8][UP].tail_delta_y = -1;
    mt->moves[STATE_8][UP].next_state   = STATE_8;
    mt->moves[STATE_8][DOWN].head_delta_x = 0;
    mt->moves[STATE_8][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_8][DOWN].tail_delta_x = 0;
    mt->moves[STATE_8][DOWN].tail_delta_y = 0;
    mt->moves[STATE_8][DOWN].next_state   = STATE_5;
    mt->moves[STATE_8][LEFT].head_delta_x = -1;
    mt->moves[STATE_8][LEFT].head_delta_y = 0;
    mt->moves[STATE_8][LEFT].tail_delta_x = 0;
    mt->moves[STATE_8][LEFT].tail_delta_y = 0;
    mt->moves[STATE_8][LEFT].next_state   = STATE_9;
    mt->moves[STATE_8][RIGHT].head_delta_x = 1;
    mt->moves[STATE_8][RIGHT].head_delta_y = 0;
    mt->moves[STATE_8][RIGHT].tail_delta_x = 0;
    mt->moves[STATE_8][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_8][RIGHT].next_state   = STATE_7;

    mt->moves[STATE_9][UP].head_delta_x = 0;
    mt->moves[STATE_9][UP].head_delta_y = -1; // going up means the row decreases by 1
    mt->moves[STATE_9][UP].tail_delta_x = -1;
    mt->moves[STATE_9][UP].tail_delta_y = -1;
    mt->moves[STATE_9][UP].next_state   = STATE_8;
    mt->moves[STATE_9][DOWN].head_delta_x = 0;
    mt->moves[STATE_9][DOWN].head_delta_y = 1; // going down means the row increases by 1
    mt->moves[STATE_9][DOWN].tail_delta_x = 0;
    mt->moves[STATE_9][DOWN].tail_delta_y = 0;
    mt->moves[STATE_9][DOWN].next_state   = STATE_6;
    mt->moves[STATE_9][LEFT].head_delta_x = -1;
    mt->moves[STATE_9][LEFT].head_delta_y = 0;
    mt->moves[STATE_9][LEFT].tail_delta_x = -1;
    mt->moves[STATE_9][LEFT].tail_delta_y = -1;
    mt->moves[STATE_9][LEFT].next_state   = STATE_6;
    mt->moves[STATE_9][RIGHT].head_delta_x = 1;
    mt->moves[STATE_9][RIGHT].head_delta_y = 0;
    mt->moves[STATE_9][RIGHT].tail_delta_x = 0;
    mt->moves[STATE_9][RIGHT].tail_delta_y = 0;
    mt->moves[STATE_9][RIGHT].next_state   = STATE_8;
}

/* For the screen, the data is stored in a 2-d list, rows, then columns.
 * This means that unlike algebra, the data is accessed by row and then by column.
 * Also, as per standard Advent of Code problems, row numbers increase from top-to-bottom. Columns increase left-to-right
 */
static int build_move_table_2(day_9_move_table_2_t * mt)
{
    for (int x=0; x<5; x++)
    {
        for (int y=0; y<5; y++)
        {
            mt->moves[x][y].is_noop = TRUE;
            mt->moves[x][y].delta_x = 0;
            mt->moves[x][y].delta_y = 0;
        }
    }
    
    mt->moves[2+MT2_OFFSET][1+MT2_OFFSET].delta_x=-1;
    mt->moves[2+MT2_OFFSET][1+MT2_OFFSET].delta_y=-1;
    mt->moves[2+MT2_OFFSET][1+MT2_OFFSET].is_noop = FALSE;

    mt->moves[2+MT2_OFFSET][0+MT2_OFFSET].delta_x=-1;
    mt->moves[2+MT2_OFFSET][0+MT2_OFFSET].delta_y=0;
    mt->moves[2+MT2_OFFSET][0+MT2_OFFSET].is_noop = FALSE;

    mt->moves[2+MT2_OFFSET][-1+MT2_OFFSET].delta_x=-1;
    mt->moves[2+MT2_OFFSET][-1+MT2_OFFSET].delta_y=1;
    mt->moves[2+MT2_OFFSET][-1+MT2_OFFSET].is_noop = FALSE;

    mt->moves[1+MT2_OFFSET][-2+MT2_OFFSET].delta_x=-1;
    mt->moves[1+MT2_OFFSET][-2+MT2_OFFSET].delta_y=1;
    mt->moves[1+MT2_OFFSET][-2+MT2_OFFSET].is_noop = FALSE;

    mt->moves[0+MT2_OFFSET][-2+MT2_OFFSET].delta_x=0;
    mt->moves[0+MT2_OFFSET][-2+MT2_OFFSET].delta_y=1;
    mt->moves[0+MT2_OFFSET][-2+MT2_OFFSET].is_noop = FALSE;

    mt->moves[-1+MT2_OFFSET][-2+MT2_OFFSET].delta_x=1;
    mt->moves[-1+MT2_OFFSET][-2+MT2_OFFSET].delta_y=1;
    mt->moves[-1+MT2_OFFSET][-2+MT2_OFFSET].is_noop = FALSE;

    mt->moves[-2+MT2_OFFSET][-1+MT2_OFFSET].delta_x=1;
    mt->moves[-2+MT2_OFFSET][-1+MT2_OFFSET].delta_y=1;
    mt->moves[-2+MT2_OFFSET][-1+MT2_OFFSET].is_noop = FALSE;

    mt->moves[-2+MT2_OFFSET][0+MT2_OFFSET].delta_x=1;
    mt->moves[-2+MT2_OFFSET][0+MT2_OFFSET].delta_y=0;
    mt->moves[-2+MT2_OFFSET][0+MT2_OFFSET].is_noop = FALSE;

    mt->moves[-2+MT2_OFFSET][1+MT2_OFFSET].delta_x=1;
    mt->moves[-2+MT2_OFFSET][1+MT2_OFFSET].delta_y=-1;
    mt->moves[-2+MT2_OFFSET][1+MT2_OFFSET].is_noop = FALSE;

    mt->moves[-1+MT2_OFFSET][2+MT2_OFFSET].delta_x=1;
    mt->moves[-1+MT2_OFFSET][2+MT2_OFFSET].delta_y=-1;
    mt->moves[-1+MT2_OFFSET][2+MT2_OFFSET].is_noop = FALSE;

    mt->moves[0+MT2_OFFSET][2+MT2_OFFSET].delta_x=0;
    mt->moves[0+MT2_OFFSET][2+MT2_OFFSET].delta_y=-1;
    mt->moves[0+MT2_OFFSET][2+MT2_OFFSET].is_noop = FALSE;

    mt->moves[1+MT2_OFFSET][2+MT2_OFFSET].delta_x=-1;
    mt->moves[1+MT2_OFFSET][2+MT2_OFFSET].delta_y=-1;
    mt->moves[1+MT2_OFFSET][2+MT2_OFFSET].is_noop = FALSE;

}
