#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_25.h"
#include "file_utils.h"

#define MAX_LEN 32

long long snafu_to_decimal(char * snafu)
{
    long long decimal = 0;
    long long multiplier = 1; // start at 1
    
#ifdef DEBUG_DAY_25
    printf("Converting snafu number %s\n", snafu);
#endif
    for (int pos=strlen(snafu) - 1; pos >=0; pos--)
    {
        switch (snafu[pos])
        {
            case '0':
            case '1':
            case '2':
                decimal += (((long long)snafu[pos]-'0') * multiplier);
                break;
            case '-':
                decimal -= multiplier;
                break;
            case '=':
                decimal -= (multiplier * 2ll); // 2 long long
                break;
            default:
                fprintf(stderr, "Invalid snafu character %c detected\n", snafu[pos]);
                break;
        }
        multiplier *= 5ll; // multiply by 5 for the next multiplier
    }
#ifdef DEBUG_DAY_25
    printf("Snafu number %s is decimal number %lld\n", snafu, decimal);
#endif
    return decimal;
}

void decimal_to_snafu(long long decimal, char * snafu)
{
#ifdef DEBUG_DAY_25
    printf("Converting decimal number %lld\n", decimal);
#endif
    char reversed[MAX_LEN];
    int pos = 0;
    do
    {
        long long current_mod = decimal % 5ll; // mod 5
        decimal /= 5ll; // divide by 5 for the next position
        switch (current_mod)
        {
            case 0:
                reversed[pos] = '0';
                break;
            case 1:
                reversed[pos] = '1';
                break;
            case 2:
                reversed[pos] = '2';
                break;
            case 3:
                reversed[pos] = '='; // 2 less than the multiple of 5
                decimal += 1ll; // add 1 to decimal for the carry
                break;
            case 4:
                reversed[pos] = '-'; // 1 less than the multipole of 5
                decimal += 1ll; // add 1 to decimal for the carry
                break;
        }
        pos++;
    } while (decimal > 0ll); // while greater than 0 long long
    
    // pos is now the number of characters
    for (int i=0; i<pos; i++)
    {
        snafu[i] = reversed[pos-i-1];
    }
#ifdef DEBUG_DAY_25
    printf("Result is snafu number %s\n", snafu);
#endif
    return;
}


void day_25_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    long long sum = 0;
    file_data_t fd;
    
    // read in the input file;  no delimiters
    
    file_data_init(&fd);
    file_data_read_file(&fd, filename, "", 0, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
    }
    
    line_data_t * ld = fd.head_line;
    
    int row_count = 0;
    while (ld != NULL)
    {
        sum += snafu_to_decimal(ld->head_token->token);
        
        ld = ld->next;
    }
    
    file_data_cleanup(&fd);
    
    char sum_snafu[MAX_LEN];
    
    
    
    decimal_to_snafu(sum, sum_snafu);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%s", sum_snafu);

    return;
}

