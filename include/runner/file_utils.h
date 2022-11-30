#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

struct token_data
{
    char * token;
    struct token_data * next;
};

typedef struct token_data token_data_t;

struct line_data
{
    token_data_t * head_token;
    token_data_t * tail_token;
    int num_tokens;
    struct line_data * next;
};

typedef struct line_data line_data_t;

struct file_data
{
    line_data_t * head_line;
    line_data_t * tail_line;
    int num_lines;
};

typedef struct file_data file_data_t;

void token_data_init(token_data_t * td);
void token_data_cleanup(token_data_t * td);
void token_data_assign_substr(token_data_t * td, char * source, int start_pos, int len);
void token_data_assign_str(token_data_t * td, char * source);

void line_data_init(line_data_t * ld);
void line_data_cleanup(line_data_t * ld);
void line_data_add_token(line_data_t * ld, token_data_t * td);
void line_data_parse_line(line_data_t * ld, char * line, char * delimiters, int num_delimiters, char quote_char, char comment_char);

void file_data_init(file_data_t * fd);
void file_data_cleanup(file_data_t * fd);
void file_data_add_line(file_data_t * fd, line_data_t * ld);
void file_data_read_file(file_data_t * fd, char * filename, char * delimiters, int num_delimiters, char quote_char, char comment_char);

#endif
