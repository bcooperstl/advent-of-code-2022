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
//        printf("Resulting board is:\n");
//        display_board(&game->tracking_board);
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

static void init_cube(day_22_cube_t * cube)
{
    for (int i=0; i<4; i++)
    {
        cube->left_right_rotation_faces[i].loaded = FALSE;
        cube->up_down_rotation_faces[i].loaded = FALSE;
        cube->left_right_rotation_faces[i].edge_length = cube->edge_length;
        cube->up_down_rotation_faces[i].edge_length = cube->edge_length;
    }
    return;
}

static void copy_face(day_22_face_t * to, day_22_face_t * from)
{
    memcpy(to, from, sizeof(day_22_face_t));
    return;
}

static void display_face(day_22_face_t * face)
{
    for (int y=0; y<face->edge_length; y++)
    {
        printf("%c", direction_chars[face->map_up_direction]);
        for (int x=0; x<face->edge_length; x++)
        {
            printf("%c", face->cells[y][x].value);
        }
        printf("%c\n", direction_chars[face->map_up_direction]);
    }
    return;
}

static void display_cube(day_22_cube_t * cube)
{
    printf("***DISPLAYING CUBE***\n");
    printf("Left-Right faces:\n");
    printf("Front face:\n");
    display_face(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
    printf("Right face:\n");
    display_face(&cube->left_right_rotation_faces[DAY_22_FACE_RIGHT]);
    printf("Back face:\n");
    display_face(&cube->left_right_rotation_faces[DAY_22_FACE_BACK]);
    printf("Left face:\n");
    display_face(&cube->left_right_rotation_faces[DAY_22_FACE_LEFT]);
    
    printf("Up-Down faces:\n");
    printf("Front face:\n");
    display_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT]);
    printf("Bottom face:\n");
    display_face(&cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM]);
    printf("Back face:\n");
    display_face(&cube->up_down_rotation_faces[DAY_22_FACE_BACK]);
    printf("Top face:\n");
    display_face(&cube->up_down_rotation_faces[DAY_22_FACE_TOP]);

    return;
}

static void rotate_face_clockwise(day_22_face_t * face)
{
    if (face->loaded == FALSE)
    {
        return;
    }
#ifdef DEBUG_DAY_22
    printf("Before rotating clockwise with up direction %d %c:\n", face->map_up_direction, direction_chars[face->map_up_direction]);
    display_face(face);
#endif
    day_22_face_t temp;
    copy_face(&temp, face);
    // source[y][x] --> dest[x][(edge_length-1)-y]
    for (int src_y=0; src_y<face->edge_length; src_y++)
    {
        for (int src_x=0; src_x<face->edge_length; src_x++)
        {
            memcpy(&face->cells[src_x][(face->edge_length - 1) - src_y], &temp.cells[src_y][src_x], sizeof(day_22_cell_t));
        }
    }
    face->map_up_direction = (face->map_up_direction + 1)%4;
#ifdef DEBUG_DAY_22
    printf("After rotating clockwise with up direction %d %c:\n", face->map_up_direction, direction_chars[face->map_up_direction]);
    display_face(face);
#endif
}    

static void rotate_face_counter_clockwise(day_22_face_t * face)
{
    if (face->loaded == FALSE)
    {
        return;
    }
#ifdef DEBUG_DAY_22
    printf("Before rotating counter-clockwise with up direction %d %c:\n", face->map_up_direction, direction_chars[face->map_up_direction]);
    display_face(face);
#endif
    day_22_face_t temp;
    copy_face(&temp, face);
    // source[y][x] --> dest[(edge_length-1)-x][y]
    for (int src_y=0; src_y<face->edge_length; src_y++)
    {
        for (int src_x=0; src_x<face->edge_length; src_x++)
        {
            memcpy(&face->cells[(face->edge_length - 1) - src_x][src_y], &temp.cells[src_y][src_x], sizeof(day_22_cell_t));
        }
    }
    face->map_up_direction = (face->map_up_direction + 3)%4;
#ifdef DEBUG_DAY_22
    printf("After rotating counter-clockwise with up direction %d %c:\n", face->map_up_direction, direction_chars[face->map_up_direction]);
    display_face(face);
#endif
}

static void update_back_face(day_22_face_t * to_face, day_22_face_t * from_face)
{
    if (from_face->loaded == FALSE)
    {
        copy_face(to_face, from_face);
        return;
    }
    if (to_face->loaded == FALSE)
    {
        to_face->loaded = TRUE;
        to_face->edge_length = from_face->edge_length;
    }
#ifdef DEBUG_DAY_22
    printf("Source for back:\n");
    display_face(from_face);
#endif
    // source[y][x] --> dest[(edge_length-1)-y][(edge_length-1)-x]
    for (int src_y=0; src_y<from_face->edge_length; src_y++)
    {
        for (int src_x=0; src_x<from_face->edge_length; src_x++)
        {
            memcpy(&to_face->cells[(from_face->edge_length - 1) - src_y][(from_face->edge_length - 1) - src_x], &from_face->cells[src_y][src_x], sizeof(day_22_cell_t));
        }
    }
    to_face->map_up_direction = (from_face->map_up_direction + 2)%4;
#ifdef DEBUG_DAY_22
    printf("Destination for back:\n");
    display_face(to_face);
#endif
}

// Turning a cube to the left means the front face moves left, the right face to moves front, etc
// The top face will rotate clockwise, and the bottom face will rotate counter-clockwise
static void turn_cube_left(day_22_cube_t * cube)
{
#ifdef DEBUG_DAY_22
    printf("Turning cube to the left\n");
#endif
    day_22_face_t temp;
    copy_face(&temp, &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_RIGHT]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_RIGHT], &cube->left_right_rotation_faces[DAY_22_FACE_BACK]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_BACK], &cube->left_right_rotation_faces[DAY_22_FACE_LEFT]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_LEFT], &temp);
    
    
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
    rotate_face_clockwise(&cube->up_down_rotation_faces[DAY_22_FACE_TOP]);
    rotate_face_counter_clockwise(&cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM]);

    update_back_face(&cube->up_down_rotation_faces[DAY_22_FACE_BACK], &cube->left_right_rotation_faces[DAY_22_FACE_BACK]);
    return;
}

// Turning a cube to the right means the front face moves right, the left face to moves front, etc
// The top face will rotate counter-clockwise, and the bottom face will rotate clockwise
static void turn_cube_right(day_22_cube_t * cube)
{
#ifdef DEBUG_DAY_22
    printf("Turning cube to the right\n");
#endif
    day_22_face_t temp;
    copy_face(&temp, &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_LEFT]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_LEFT], &cube->left_right_rotation_faces[DAY_22_FACE_BACK]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_BACK], &cube->left_right_rotation_faces[DAY_22_FACE_RIGHT]);
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_RIGHT], &temp);
    
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
    rotate_face_counter_clockwise(&cube->up_down_rotation_faces[DAY_22_FACE_TOP]);
    rotate_face_clockwise(&cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM]);

    update_back_face(&cube->up_down_rotation_faces[DAY_22_FACE_BACK], &cube->left_right_rotation_faces[DAY_22_FACE_BACK]);
    return;
}

// Turning a cube up means the front face moves up, the bottom face to moves front, etc
// The right face will rotate clockwise, and the left face will rotate counter-clockwise
static void turn_cube_up(day_22_cube_t * cube)
{
#ifdef DEBUG_DAY_22
    printf("Turning cube up\n");
#endif
    day_22_face_t temp;
    copy_face(&temp, &cube->up_down_rotation_faces[DAY_22_FACE_FRONT]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM], &cube->up_down_rotation_faces[DAY_22_FACE_BACK]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_BACK], &cube->up_down_rotation_faces[DAY_22_FACE_TOP]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_TOP], &temp);
    
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], &cube->up_down_rotation_faces[DAY_22_FACE_FRONT]);
    rotate_face_clockwise(&cube->left_right_rotation_faces[DAY_22_FACE_RIGHT]);
    rotate_face_counter_clockwise(&cube->left_right_rotation_faces[DAY_22_FACE_LEFT]);

    update_back_face(&cube->left_right_rotation_faces[DAY_22_FACE_BACK], &cube->up_down_rotation_faces[DAY_22_FACE_BACK]);
    return;
}

// Turning a cube down means the front face moves to the bottom, the top face to moves front, etc
// The right face will rotate counter-clockwise, and the left face will rotate clockwise
static void turn_cube_down(day_22_cube_t * cube)
{
#ifdef DEBUG_DAY_22
    printf("Turning cube down\n");
#endif
    day_22_face_t temp;
    copy_face(&temp, &cube->up_down_rotation_faces[DAY_22_FACE_FRONT]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->up_down_rotation_faces[DAY_22_FACE_TOP]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_TOP], &cube->up_down_rotation_faces[DAY_22_FACE_BACK]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_BACK], &cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM]);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_BOTTOM], &temp);
    
    copy_face(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], &cube->up_down_rotation_faces[DAY_22_FACE_FRONT]);
    rotate_face_counter_clockwise(&cube->left_right_rotation_faces[DAY_22_FACE_RIGHT]);
    rotate_face_clockwise(&cube->left_right_rotation_faces[DAY_22_FACE_LEFT]);

    update_back_face(&cube->left_right_rotation_faces[DAY_22_FACE_BACK], &cube->up_down_rotation_faces[DAY_22_FACE_BACK]);
    return;
}

static void load_face_from_board(day_22_face_t * face, day_22_board_t * board, int start_y, int start_x)
{
    if (face->loaded == TRUE)
    {
        fprintf(stderr, "***FACE IS ALREADY LOADED...FIX YOUR STUFF***\n");
        return;
    }
    int edge_length = face->edge_length;
    for (int dest_y=0; dest_y<edge_length; dest_y++)
    {
        for (int dest_x=0; dest_x<edge_length; dest_x++)
        {
            face->cells[dest_y][dest_x].value = board->layout[start_y+dest_y][start_x+dest_x];
            face->cells[dest_y][dest_x].input_row = start_y+dest_y;
            face->cells[dest_y][dest_x].input_col = start_x+dest_x;
        }
    }
    face->map_up_direction = DAY_22_UP;
    face->loaded = TRUE;
}


static void load_cube_from_board(day_22_cube_t * cube, day_22_board_t * board)
{
    init_cube(cube);
    int start_x = 1;
    int start_y = 1;
    
    int num_loaded = 0;
    int loaded_x[6];
    int loaded_y[6];
    char path_to_load[6][20];
    for (int i=0; i<6; i++)
    {
        path_to_load[i][0] = '\0';
    }
    
    while (board->layout[start_y][start_x] == DAY_22_VOID)
    {
        start_x += cube->edge_length;
    }
#ifdef DEBUG_DAY_22
    printf("Loading front face from row=%d col=%d\n", start_y, start_x);
#endif
    load_face_from_board(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], board, start_y, start_x);
    copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
    num_loaded = 1;
    loaded_x[0] = start_x;
    loaded_y[0] = start_y;
    
    while (num_loaded != 6)
    {
#ifdef DEBUG_DAY_22
        printf("%d faces loaded...trying for more\n", num_loaded);
#endif
        // check left, right, bottom of each loaded point
        for (int i=0; i<num_loaded; i++)
        {
            // check left
            int test_y = loaded_y[i];
            int test_x = loaded_x[i]-cube->edge_length;
            if (test_x >= 1)
            {
#ifdef DEBUG_DAY_22
                printf(" Checking left of row=%d col=%d at row=%d col=%d\n", loaded_y[i], loaded_x[i], test_y, test_x);
#endif
                if (board->layout[test_y][test_x] != DAY_22_VOID)
                {
#ifdef DEBUG_DAY_22
                    printf("  Potential loading point found\n");
#endif
                    int loaded = FALSE;
                    for (int j=0; j<num_loaded; j++)
                    {
                        if ((loaded_x[j] == test_x) && (loaded_y[j] == test_y))
                        {
                            loaded = TRUE;
                            break;
                        }
                    }
                    if (loaded == TRUE)
                    {
#ifdef DEBUG_DAY_22
                        printf("  Already loaded; skipping\n");
#endif
                    }
                    else
                    {
                        snprintf(path_to_load[num_loaded], 20, "%sL", path_to_load[i]);
#ifdef DEBUG_DAY_22
                        printf("  Loading from path %s\n", path_to_load[num_loaded]);
#endif
                        int path_length = strlen(path_to_load[num_loaded]);
                        for (int j=0; j<path_length; j++)
                        {
                            if (path_to_load[num_loaded][j] == 'L')
                            {
                                turn_cube_right(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'R')
                            {
                                turn_cube_left(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'D')
                            {
                                turn_cube_up(cube);
                            }
                        }
                        load_face_from_board(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], board, test_y, test_x);
                        copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
                        for (int j=path_length-1; j>=0; j--)
                        {
                            if (path_to_load[num_loaded][j] == 'L')
                            {
                                turn_cube_left(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'R')
                            {
                                turn_cube_right(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'D')
                            {
                                turn_cube_down(cube);
                            }
                        }
                        loaded_x[num_loaded] = test_x;
                        loaded_y[num_loaded] = test_y;
                        num_loaded++;
#ifdef DEBUG_DAY_22
                        printf("  Face loaded. Cube is now:\n");
                        display_cube(cube);
#endif
                        break; // reset out
                    }
                }
            }

            // check right
            test_y = loaded_y[i];
            test_x = loaded_x[i]+cube->edge_length;
            if (test_x < (board->num_cols-1))
            {
#ifdef DEBUG_DAY_22
                printf(" Checking right of row=%d col=%d at row=%d col=%d\n", loaded_y[i], loaded_x[i], test_y, test_x);
#endif
                if (board->layout[test_y][test_x] != DAY_22_VOID)
                {
#ifdef DEBUG_DAY_22
                    printf("  Potential loading point found\n");
#endif
                    int loaded = FALSE;
                    for (int j=0; j<num_loaded; j++)
                    {
                        if ((loaded_x[j] == test_x) && (loaded_y[j] == test_y))
                        {
                            loaded = TRUE;
                            break;
                        }
                    }
                    if (loaded == TRUE)
                    {
#ifdef DEBUG_DAY_22
                        printf("  Already loaded; skipping\n");
#endif
                    }
                    else
                    {
                        snprintf(path_to_load[num_loaded], 20, "%sR", path_to_load[i]);
#ifdef DEBUG_DAY_22
                        printf("  Loading from path %s\n", path_to_load[num_loaded]);
#endif
                        int path_length = strlen(path_to_load[num_loaded]);
                        for (int j=0; j<path_length; j++)
                        {
                            if (path_to_load[num_loaded][j] == 'L')
                            {
                                turn_cube_right(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'R')
                            {
                                turn_cube_left(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'D')
                            {
                                turn_cube_up(cube);
                            }
                        }
                        load_face_from_board(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], board, test_y, test_x);
                        copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
                        for (int j=path_length-1; j>=0; j--)
                        {
                            if (path_to_load[num_loaded][j] == 'L')
                            {
                                turn_cube_left(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'R')
                            {
                                turn_cube_right(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'D')
                            {
                                turn_cube_down(cube);
                            }
                        }
                        loaded_x[num_loaded] = test_x;
                        loaded_y[num_loaded] = test_y;
                        num_loaded++;
#ifdef DEBUG_DAY_22
                        printf("  Face loaded. Cube is now:\n");
                        display_cube(cube);
#endif
                        break; // reset out
                    }
                }
            }

            // check bottom
            test_y = loaded_y[i]+cube->edge_length;
            test_x = loaded_x[i];
            if (test_y < (board->num_rows-1))
            {
#ifdef DEBUG_DAY_22
                printf(" Checking bottom of row=%d col=%d at row=%d col=%d\n", loaded_y[i], loaded_x[i], test_y, test_x);
#endif
                if (board->layout[test_y][test_x] != DAY_22_VOID)
                {
#ifdef DEBUG_DAY_22
                    printf("  Potential loading point found\n");
#endif
                    int loaded = FALSE;
                    for (int j=0; j<num_loaded; j++)
                    {
                        if ((loaded_x[j] == test_x) && (loaded_y[j] == test_y))
                        {
                            loaded = TRUE;
                            break;
                        }
                    }
                    if (loaded == TRUE)
                    {
#ifdef DEBUG_DAY_22
                        printf("  Already loaded; skipping\n");
#endif
                    }
                    else
                    {
                        snprintf(path_to_load[num_loaded], 20, "%sD", path_to_load[i]);
#ifdef DEBUG_DAY_22
                        printf("  Loading from path %s\n", path_to_load[num_loaded]);
#endif
                        int path_length = strlen(path_to_load[num_loaded]);
                        for (int j=0; j<path_length; j++)
                        {
                            if (path_to_load[num_loaded][j] == 'L')
                            {
                                turn_cube_right(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'R')
                            {
                                turn_cube_left(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'D')
                            {
                                turn_cube_up(cube);
                            }
                        }
                        load_face_from_board(&cube->left_right_rotation_faces[DAY_22_FACE_FRONT], board, test_y, test_x);
                        copy_face(&cube->up_down_rotation_faces[DAY_22_FACE_FRONT], &cube->left_right_rotation_faces[DAY_22_FACE_FRONT]);
                        for (int j=path_length-1; j>=0; j--)
                        {
                            if (path_to_load[num_loaded][j] == 'L')
                            {
                                turn_cube_left(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'R')
                            {
                                turn_cube_right(cube);
                            }
                            if (path_to_load[num_loaded][j] == 'D')
                            {
                                turn_cube_down(cube);
                            }
                        }
                        loaded_x[num_loaded] = test_x;
                        loaded_y[num_loaded] = test_y;
                        num_loaded++;
#ifdef DEBUG_DAY_22
                        printf("  Face loaded. Cube is now:\n");
                        display_cube(cube);
#endif
                        break; // reset out
                    }
                }
            }

        }
    }
}

void day_22_part_2(char * filename, extra_args_t * extra_args, char * result)
{
    day_22_game_t game;
    day_22_cube_t cube;
    
    read_and_parse_input(filename, &game);
    
    cube.edge_length = PART_2_MAX_EDGE_LENGTH;
    
    if (extra_args->num_extra_args == 1)
    {
        cube.edge_length = strtol(extra_args->extra_args[0], NULL, 10);
    }
    
    printf("Performing part 2 with edge length %d\n", cube.edge_length);
    
    load_cube_from_board(&cube, &game.layout_board);

    snprintf(result, MAX_RESULT_LENGTH+1, "%d", calculate_score(&game));
    
    return;
}
