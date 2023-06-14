#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_22.h"
#include "file_utils.h"

static char direction_chars[4]={DAY_22_MOVE_RIGHT, DAY_22_MOVE_DOWN, DAY_22_MOVE_LEFT, DAY_22_MOVE_UP};

static void init_board(day_22_board_t * board, char init_char)
{
    for (int y=0; y<DAY_22_MAX_DIMENSION+2; y++)
    {
        for (int x=0; x<DAY_22_MAX_DIMENSION+2; x++)
        {
            board->layout[y][x]=init_char;
        }
        board->layout[y][DAY_22_MAX_DIMENSION+2]='\0';
        board->row_min_non_void[y] = 0;
        board->row_max_non_void[y] = 0;
        board->col_min_non_void[y] = 0; // abusing that num_rows = num_cols; to set these for col here instead of in another loop
        board->col_max_non_void[y] = 0;
    }
    board->num_rows = 0;
    board->num_cols = 0;
    return;
}

static void copy_board(day_22_board_t * to, day_22_board_t * from)
{
    memcpy(to, from, sizeof(day_22_board_t));
}

void set_min_max(day_22_board_t * board)
{
#ifdef DEBUG_DAY_22
    printf("Finding min and max values in rows\n");
#endif
    for (int y=1; y<board->num_rows-1; y++) // board of 6 rows would have values in rows 1-4
    {
        for (int x=1; x<board->num_cols-1; x++)
        {
            if (board->layout[y][x] != DAY_22_VOID)
            {
                if (board->row_min_non_void[y] == 0)
                {
                    board->row_min_non_void[y] = x;
                }
                board->row_max_non_void[y] = x;
            }
        }
#ifdef DEBUG_DAY_22
        printf(" Row %d has min col %d and max col %d\n", y, board->row_min_non_void[y], board->row_max_non_void[y]);
#endif
    }

#ifdef DEBUG_DAY_22
    printf("Finding min and max values in cols\n");
#endif
    for (int x=1; x<board->num_cols-1; x++) // board of 6 cols would have values in cols 1-4
    {
        for (int y=1; y<board->num_rows-1; y++)
        {
            if (board->layout[y][x] != DAY_22_VOID)
            {
                if (board->col_min_non_void[x] == 0)
                {
                    board->col_min_non_void[x] = y;
                }
                board->col_max_non_void[x] = y;
            }
        }
#ifdef DEBUG_DAY_22
        printf(" Col %d has min row %d and max row %d\n", x, board->col_min_non_void[x], board->col_max_non_void[x]);
#endif
    }
}

static void display_board(day_22_board_t * board)
{
    printf("The board has %d rows and %d columns\n", board->num_rows, board->num_cols);
    for (int y=0; y<board->num_rows; y++)
    {
        printf("|%s|\n", board->layout[y]);
    }
    return;
}

static void init_instructions(day_22_instructions_t * instructions)
{
    // make all the instructions to move 0 spaces
    for (int i=0; i<DAY_22_MAX_INSTRUCTIONS; i++)
    {
        instructions->instructions[i].type = DAY_22_INSTRUCTION_GO;
        instructions->instructions[i].num_to_go = 0;
    }
    instructions->num_instructions = 0;
    return;
}

static void read_and_parse_input(char * filename, day_22_game_t * game)
{
    file_data_t fd;
    
    // read in the input file;  no delimiters
    
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return;
    }
    
    init_board(&game->layout_board, DAY_22_VOID);
    game->layout_board.num_rows = 1; // skip the first row - all spaces
    game->layout_board.num_cols = 2; // at least 2 columns - left and right spaces
    
    line_data_t * ld = fd.head_line;
    
    while (ld != NULL && strlen(ld->head_token->token) > 0)
    {
        char * token = ld->head_token->token;
        printf("[%s]\n", token);
        int row_len = strlen(token);
        if ((row_len + 2) > game->layout_board.num_cols)
        {
            game->layout_board.num_cols = row_len + 2; // add the 2 for the left and right space
        }
        
        
        memcpy(&game->layout_board.layout[game->layout_board.num_rows][1], token, row_len);
        
        game->layout_board.num_rows++;
        ld = ld->next;
    }
    
    game->layout_board.num_rows++; // bottom row of spaces
    
    // set null terminators after the right added space in each column
    for (int y=0; y<game->layout_board.num_rows; y++)
    {
        game->layout_board.layout[y][game->layout_board.num_cols] = '\0';
    }

    set_min_max(&game->layout_board);    

#ifdef DEBUG_DAY_22
    printf("Initial layout board:\n");
    display_board(&game->layout_board);
#endif

    ld = ld->next; // skip blank line to instructions line
    char * inst_line = ld->head_token->token;
    int inst_length = strlen(inst_line);
    int inst_line_idx = 0;
    
    init_instructions(&game->instructions);
    
    do
    {
        char inst = inst_line[inst_line_idx];
        if (inst == DAY_22_CLOCKWISE || inst == DAY_22_COUNTER_CLOCKWISE)
        {
            // this insturction is a turn
            game->instructions.instructions[game->instructions.num_instructions].type = DAY_22_INSTRUCTION_TURN;
            game->instructions.instructions[game->instructions.num_instructions].direction_to_turn = inst;
#ifdef DEBUG_DAY_22
            printf("Instruction %d is to turn %c\n", game->instructions.num_instructions, game->instructions.instructions[game->instructions.num_instructions].direction_to_turn);
#endif
            inst_line_idx++;
        }
        else
        {
            // this instruction is to move either 1 or 2 digit number of spaces - inspected input file
            game->instructions.instructions[game->instructions.num_instructions].type = DAY_22_INSTRUCTION_GO;
            game->instructions.instructions[game->instructions.num_instructions].num_to_go = strtol(&inst_line[inst_line_idx], NULL, 10);
#ifdef DEBUG_DAY_22
            printf("Instruction %d is move forward %d\n", game->instructions.num_instructions, game->instructions.instructions[game->instructions.num_instructions].num_to_go);
#endif
            if (game->instructions.instructions[game->instructions.num_instructions].num_to_go > 9)
            {
                inst_line_idx += 2;
            }
            else
            {
                inst_line_idx += 1;
            }
        }
        game->instructions.num_instructions++;
    } while(inst_line_idx < inst_length);

    
    file_data_cleanup(&fd);
    
    return;
}

static void map_position(day_22_game_t * game)
{
    game->tracking_board.layout[game->pos_y][game->pos_x]=direction_chars[game->direction];
    return;
}

static void init_game(day_22_game_t * game)
{
    copy_board(&game->tracking_board, &game->layout_board);
    game->pos_y = 1; // start on the first row
    game->pos_x = game->layout_board.row_min_non_void[1]; // start on the first non-void column of that row
    game->direction = DAY_22_RIGHT;
    game->instructions_performed = 0;
    map_position(game);
#ifdef DEBUG_DAY_22
    printf("Initial tracking board:\n");
    display_board(&game->tracking_board);
#endif
    
    return;
}

static int calculate_score(day_22_game_t * game)
{
    return ((1000 * game->pos_y) + (4 * game->pos_x) + game->direction);
}

static void perform_turn_instruction(day_22_game_t * game, char direction)
{
    if (direction == DAY_22_CLOCKWISE)
    {
#ifdef DEBUG_DAY_22
        printf(" Turning clockwise from %c", direction_chars[game->direction]);
#endif
        game->direction = (game->direction + 1)%4;
    }
    else if (direction == DAY_22_COUNTER_CLOCKWISE)
    {
#ifdef DEBUG_DAY_22
        printf(" Turning counter-clockwise from %c", direction_chars[game->direction]);
#endif
        game->direction = (game->direction + 3)%4;
    }
    else
    {
        fprintf(stderr, "****INVALID DIRECTION [%c] TO TURN******\n", direction);
        return;
    }
#ifdef DEBUG_DAY_22
    printf(" to %c", direction_chars[game->direction]);
#endif
    map_position(game);
    return;
}

static void perform_move_instruction(day_22_game_t * game, int num_to_go)
{
    switch (game->direction)
    {
        case DAY_22_RIGHT:
#ifdef DEBUG_DAY_22
            printf(" Moving %d to the right starting at row=%d col=%d\n", num_to_go, game->pos_y, game->pos_x);
#endif
            for (int i=0; i<num_to_go; i++)
            {
                int next_x = game->pos_x + 1;
#ifdef DEBUG_DAY_22
                printf("  Move %d increments to col=%d\n", i+1, next_x);
#endif
                if (next_x > game->layout_board.row_max_non_void[game->pos_y])
                {
                    next_x = game->layout_board.row_min_non_void[game->pos_y];
#ifdef DEBUG_DAY_22
                    printf("  Move %d wraps back around to col=%d\n", i+1, next_x);
#endif
                }
                if (game->layout_board.layout[game->pos_y][next_x] == DAY_22_WALL)
                {
#ifdef DEBUG_DAY_22
                    printf("  Wall detected on move %d; moving stops with final position at row=%d, col=%d\n", i+1, game->pos_y, game->pos_x);
#endif
                    break;
                }
                game->pos_x=next_x;
                map_position(game);
#ifdef DEBUG_DAY_22
                printf("  Move performed. Current location is row=%d col=%d\n", game->pos_y, game->pos_x);
#endif
            }
            break;

        case DAY_22_LEFT:
#ifdef DEBUG_DAY_22
            printf(" Moving %d to the left starting at row=%d col=%d\n", num_to_go, game->pos_y, game->pos_x);
#endif
            for (int i=0; i<num_to_go; i++)
            {
                int next_x = game->pos_x - 1;
#ifdef DEBUG_DAY_22
                printf("  Move %d decrements to col=%d\n", i+1, next_x);
#endif
                if (next_x < game->layout_board.row_min_non_void[game->pos_y])
                {
                    next_x = game->layout_board.row_max_non_void[game->pos_y];
#ifdef DEBUG_DAY_22
                    printf("  Move %d wraps back around to col=%d\n", i+1, next_x);
#endif
                }
                if (game->layout_board.layout[game->pos_y][next_x] == DAY_22_WALL)
                {
#ifdef DEBUG_DAY_22
                    printf("  Wall detected on move %d; moving stops with final position at row=%d, col=%d\n", i+1, game->pos_y, game->pos_x);
#endif
                    break;
                }
                game->pos_x=next_x;
                map_position(game);
#ifdef DEBUG_DAY_22
                printf("  Move performed. Current location is row=%d col=%d\n", game->pos_y, game->pos_x);
#endif
            }
            break;

        case DAY_22_DOWN:
#ifdef DEBUG_DAY_22
            printf(" Moving %d down starting at row=%d col=%d\n", num_to_go, game->pos_y, game->pos_x);
#endif
            for (int i=0; i<num_to_go; i++)
            {
                int next_y = game->pos_y + 1;
#ifdef DEBUG_DAY_22
                printf("  Move %d increments to row=%d\n", i+1, next_y);
#endif
                if (next_y > game->layout_board.col_max_non_void[game->pos_x])
                {
                    next_y = game->layout_board.col_min_non_void[game->pos_x];
#ifdef DEBUG_DAY_22
                    printf("  Move %d wraps back around to row=%d\n", i+1, next_y);
#endif
                }
                if (game->layout_board.layout[next_y][game->pos_x] == DAY_22_WALL)
                {
#ifdef DEBUG_DAY_22
                    printf("  Wall detected on move %d; moving stops with final position at row=%d, col=%d\n", i+1, game->pos_y, game->pos_x);
#endif
                    break;
                }
                game->pos_y=next_y;
                map_position(game);
#ifdef DEBUG_DAY_22
                printf("  Move performed. Current location is row=%d col=%d\n", game->pos_y, game->pos_x);
#endif
            }
            break;

        case DAY_22_UP:
#ifdef DEBUG_DAY_22
            printf(" Moving %d up starting at row=%d col=%d\n", num_to_go, game->pos_y, game->pos_x);
#endif
            for (int i=0; i<num_to_go; i++)
            {
                int next_y = game->pos_y - 1;
#ifdef DEBUG_DAY_22
                printf("  Move %d decrements to row=%d\n", i+1, next_y);
#endif
                if (next_y < game->layout_board.col_min_non_void[game->pos_x])
                {
                    next_y = game->layout_board.col_max_non_void[game->pos_x];
#ifdef DEBUG_DAY_22
                    printf("  Move %d wraps back around to row=%d\n", i+1, next_y);
#endif
                }
                if (game->layout_board.layout[next_y][game->pos_x] == DAY_22_WALL)
                {
#ifdef DEBUG_DAY_22
                    printf("  Wall detected on move %d; moving stops with final position at row=%d, col=%d\n", i+1, game->pos_y, game->pos_x);
#endif
                    break;
                }
                game->pos_y=next_y;
                map_position(game);
#ifdef DEBUG_DAY_22
                printf("  Move performed. Current location is row=%d col=%d\n", game->pos_y, game->pos_x);
#endif
            }
            break;

    }
    return;
}

static void play_game(day_22_game_t * game)
{
    while (game->instructions_performed < game->instructions.num_instructions)
    {
#ifdef DEBUG_DAY_22
        printf("Performing instruction %d\n", game->instructions_performed);
#endif
        if (game->instructions.instructions[game->instructions_performed].type == DAY_22_INSTRUCTION_TURN)
        {
            perform_turn_instruction(game, game->instructions.instructions[game->instructions_performed].direction_to_turn);
        }
        else
        {
            perform_move_instruction(game, game->instructions.instructions[game->instructions_performed].num_to_go);
        }
#ifdef DEBUG_DAY_22
        printf("Resulting board is:\n");
        display_board(&game->tracking_board);
#endif
        game->instructions_performed++;
    }
}

void day_22_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_22_game_t game;
    
    read_and_parse_input(filename, &game);
    init_game(&game);
    
    play_game(&game);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%d", calculate_score(&game));
    
    return;
}
