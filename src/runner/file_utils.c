#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
#include "file_utils.h"

#define MAX_LINE_LENGTH 12000

void token_data_init(token_data_t * td)
{
    td->token = NULL;
    td->next = NULL;
}

void token_data_cleanup(token_data_t * td)
{
    if (td->token != NULL)
    {
        free(td->token);
    }
}

// len chars in source from start_pos are copied, plus a null terminator
void token_data_assign_substr(token_data_t * td, char * source, int start_pos, int len)
{
    if (td->token != NULL)
    {
        fprintf(stderr, "***ERROR*** Reassigning to existing token substring over[%s]!!\n", td->token);
    }
    td->token = (char *)malloc(len + 1);
    memset(td->token, '\0', len + 1);
    strncpy(td->token, source+start_pos, len);
    td->token[len] = '\0'; // add the null-terminator
}
    
void token_data_assign_str(token_data_t * td, char * source)
{
    token_data_assign_substr(td, source, 0, strlen(source));
}

void line_data_init(line_data_t * ld)
{
    ld->head_token = NULL;
    ld->tail_token = NULL;
    ld->num_tokens = 0;
    ld->next = NULL;
}

void line_data_cleanup(line_data_t * ld)
{
    token_data_t * curr = ld->head_token;
    while (curr != NULL)
    {
        token_data_t * next = curr->next;
        token_data_cleanup(curr);
        free(curr);
        curr = next;
    }
}

void line_data_add_token(line_data_t * ld, token_data_t * td)
{
    // first token in line - set head and tail to it
    if (ld->num_tokens == 0)
    {
        ld->head_token = td;
        ld->tail_token = td;
        ld->num_tokens = 1;
    }
    // not the first token; set tail->next to this token and this token as tail
    else
    {
        ld->tail_token->next = td;
        ld->tail_token = td;
        ld->num_tokens++;
    }
}

// Really basic way to parse a line based on delimiters. 
// parameters:
//  ld - the line data object to store the results
//  line - the input line to split. assume newline is removed, or else it will be addded to the last item
//  delimiters = the values to split on
//  num_delimiters = the number of delimiters in the list
//  quote = an optional parameter - a quote character to indicate a that quoted sections will be used and ignore delimiters
//  comment_char = an optional parameter - if this is the first character in a line, that line is treated as a comment and skipped.
void line_data_parse_line(line_data_t * ld, char * line, char * delimiters, int num_delimiters, char quote_char, char comment_char)
{
#ifdef DEBUG_RUNNER
    printf("original input is [%s]\n", line);
#endif

    int pos = 0;
    
    if (comment_char && line[0] == comment_char)
    {
#ifdef DEBUG_RUNNER
        printf("Comment line found from %c as 0\n", line[0]);
#endif
        return;
    }
    
    int in_quote = FALSE;
    int current_token_start = 0;
    int current_token_len = 0;
    while (line[pos] != '\0')
    {
#ifdef DEBUG_RUNNER
        printf("Parsing %c at position %d with current_token_start=%d and current_token_len = %d\n", line[pos], pos, current_token_start, current_token_len);
#endif
        if (quote_char && in_quote)
        {
            if (line[pos] == quote_char)
            {
                in_quote = FALSE;
#ifdef DEBUG_RUNNER
                printf("Leaving quotes at %d\n", pos);
#endif
            }
            else
            {
                current_token_len++;
            }
        }
        else
        {
            if (quote_char && line[pos] == quote_char)
            {
                in_quote = TRUE;
#ifdef DEBUG_RUNNER
                printf("In quotes at %d\n", pos);
#endif
                if (current_token_len != 0)
                {
                    fprintf(stderr, "***Cannot handle parsing a quote when token length is non-zero\n");
                }
                current_token_start = pos + 1;
                current_token_len = 0;
            }
            else
            {
                int matched = FALSE;
                for (int i=0; i<num_delimiters; i++)
                {
                    if (line[pos] == delimiters[i])
                    {
#ifdef DEBUG_RUNNER
                        printf("Delimiter %c found at %d\n", line[pos], pos);
#endif
                        matched = TRUE;
                    }
                }
                if (matched) // we are at a delimiter
                {
                    // ABCDE,ABCDE,ABCDE
                    // first string goes from 0 to 4...construct as string(0,5). pos will be 5 for the comma. so 5-0=5
                    // second string goes from 6 to 10...construct as string(6,5)...pos wil be 11 for the comma. so 11-6=5
                    // third string goes from 12 to 16...construct as string(12,5) but need to do this out of loop as its the last string
                        
                    token_data_t * td = (token_data_t *)malloc(sizeof(token_data_t));
                    token_data_init(td);
                    token_data_assign_substr(td, line, current_token_start, current_token_len);
                    line_data_add_token(ld, td);
#ifdef DEBUG_RUNNER
                    printf("appending [%s] from %d length %d as a token\n", td->token, current_token_start, current_token_len);
#endif
                    current_token_start = pos + 1;
                    current_token_len = 0;
                }
                else
                {
                    current_token_len++;
                }
            }
        }
        pos++;
    }
    // append the last string. pos will be pointed to the null terminator at 17, so string(12,5) would be pos(17)-start(12)
    token_data_t * td = (token_data_t *)malloc(sizeof(token_data_t));
    token_data_init(td);
    token_data_assign_substr(td, line, current_token_start, current_token_len);
    line_data_add_token(ld, td);
#ifdef DEBUG_RUNNER
    printf("appending [%s] from %d length %d as the last token\n", td->token, current_token_start, current_token_len);
#endif
    return;
}


void file_data_init(file_data_t * fd)
{
    fd->head_line = NULL;
    fd->tail_line = NULL;
    fd->num_lines = 0;
}

void file_data_cleanup(file_data_t * fd)
{
    line_data_t * curr = fd->head_line;
    while (curr != NULL)
    {
        line_data_t * next = curr->next;
        line_data_cleanup(curr);
        free(curr);
        curr = next;
    }
}

void file_data_add_line(file_data_t * fd, line_data_t * ld)
{
    // first line in file - set head and tail to it
    if (fd->num_lines == 0)
    {
        fd->num_lines = 1;
        fd->head_line = ld;
        fd->tail_line = ld;
    }
    // not the first line - set tail->next to this line and this line as tail
    else
    {
        fd->tail_line->next = ld;
        fd->tail_line = ld;
        fd->num_lines++;
    }
}

void file_data_read_file(file_data_t * fd, char * filename, char * delimiters, int num_delimiters, char quote_char, char comment_char)
{
    FILE * infile = NULL;
    char line_buffer[MAX_LINE_LENGTH + 1];
    
    infile = fopen(filename, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening file %s for reading\n", filename);
        return;
    }
    
    while (fgets(line_buffer, sizeof(line_buffer), infile) != NULL)
    {
        // make sure we have a full line
        int eol_found = FALSE;
        char * eol = strchr(line_buffer, '\r');
        if (eol != NULL)
        {
            *eol = '\0';
            eol_found = TRUE;
        }
        eol = strchr(line_buffer, '\n');
        if (eol != NULL)
        {
            *eol = '\0';
            eol_found = TRUE;
        }
        if (eol_found == FALSE)
        {
            fprintf(stderr, "***ERROR***Line is longer than %d; increase MAX_LINE_LENGTH for reading files\n", MAX_LINE_LENGTH);
            return;
        }
        
        // parse it
        line_data_t * ld = (line_data_t *)malloc(sizeof(line_data_t));
        line_data_init(ld);
        line_data_parse_line(ld, line_buffer, delimiters, num_delimiters, quote_char, comment_char);
        file_data_add_line(fd, ld);
    }
    fclose(infile);
}

