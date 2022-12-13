#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aoc_solutions.h"
#include "aoc_day_7.h"
#include "file_utils.h"

#define NUM_FILES_ALLOC 2
#define NUM_DIRS_ALLOC 1

static void init_directory(day_7_directory_t * dir, char * name, day_7_directory_t * parent)
{
    strncpy(dir->dirname, name, DAY_7_NAME_LEN+1);
    dir->dirname[DAY_7_NAME_LEN] = '\0';
    dir->total_size = 0;
    dir->files = (day_7_file_t *)malloc(NUM_FILES_ALLOC * sizeof(day_7_file_t));
    dir->total_files = NUM_FILES_ALLOC;
    dir->used_files = 0;
    dir->dirs = (day_7_directory_t *)malloc(NUM_DIRS_ALLOC * sizeof(day_7_directory_t));
    dir->total_dirs = NUM_DIRS_ALLOC;
    dir->used_dirs = 0;
    dir->parent = parent;
#ifdef DEBUG_DAY_7
    printf("Initialized directory %s with %d files and %d directories\n", name, NUM_FILES_ALLOC, NUM_DIRS_ALLOC);
#endif
}

static void cleanup_directory(day_7_directory_t * dir)
{
    for (int i=0; i<dir->used_dirs; i++)
    {
        cleanup_directory(&dir->dirs[i]);
    }
    free(dir->files);
    free(dir->dirs);
}


static void dump_directory(day_7_directory_t * dir, int level)
{
    printf("%*s%s\n", level*2, "", dir->dirname);
    for (int i=0; i<dir->used_files; i++)
    {
        printf("%*s%s %d\n", level*2+2, "", dir->files[i].filename, dir->files[i].size);
    }
    for (int i=0; i<dir->used_dirs; i++)
    {
        dump_directory(&dir->dirs[i], level+1);
    }
}

static void add_file(day_7_directory_t * dir, char * filename, int size)
{
    if (dir->total_files == dir->used_files)
    {
#ifdef DEBUG_DAY_7
        printf("Expanding %s from %d files to %d files\n", dir->dirname, dir->total_files, dir->total_files+NUM_FILES_ALLOC);
#endif
        day_7_file_t * new_files = (day_7_file_t *)malloc((dir->total_files + NUM_FILES_ALLOC) * sizeof(day_7_file_t));
        memcpy(new_files, dir->files, dir->total_files * sizeof(day_7_file_t));
        free(dir->files);
        dir->files = new_files;
        dir->total_files += NUM_FILES_ALLOC;
    }
#ifdef DEBUG_DAY_7
    printf("Setting file %d in dir %s to %s\n", dir->used_files, dir->dirname, filename);
#endif
    strncpy(dir->files[dir->used_files].filename, filename, DAY_7_NAME_LEN+1);
    dir->files[dir->used_files].filename[DAY_7_NAME_LEN] = '\0';
    dir->files[dir->used_files].size = size;
    dir->files[dir->used_files].parent = dir;
    dir->used_files++;
}

static void add_directory(day_7_directory_t * dir, char * dirname)
{
    if (dir->total_dirs == dir->used_dirs)
    {
#ifdef DEBUG_DAY_7
        printf("Expanding %s from %d dirs to %d dirs\n", dir->dirname, dir->total_dirs, dir->total_dirs+NUM_DIRS_ALLOC);
#endif
        day_7_directory_t * new_dirs = (day_7_directory_t *)malloc((dir->total_dirs + NUM_DIRS_ALLOC) * sizeof(day_7_directory_t));
        memcpy(new_dirs, dir->dirs, dir->total_dirs * sizeof(day_7_directory_t));
        free(dir->dirs);
        dir->dirs = new_dirs;
        dir->total_dirs += NUM_DIRS_ALLOC;
    }
#ifdef DEBUG_DAY_7
    printf("Setting file %d in dir %s to %s\n", dir->used_dirs, dir->dirname, dirname);
#endif
    init_directory(&dir->dirs[dir->used_dirs], dirname, dir);
    dir->used_dirs++;
}

static int parse_input_and_build_tree(char * filename, day_7_directory_t * slash)
{
    day_7_directory_t * cwd = slash;

#ifdef DEBUG_DAY_7_PARSE
    printf("Parsing - set initial directory to /\n");
#endif
    file_data_t fd;
    line_data_t * ld;
    token_data_t * td;
    
    // read in the input file with space delimiters
    file_data_init(&fd);
    file_data_read_file(&fd, filename, " ", 1, '\0', '\0');
    if (fd.num_lines == 0)
    {
        fprintf(stderr, "Error reading in data from %s\n", filename);
        file_data_cleanup(&fd);
        return FALSE;
    }

    ld = fd.head_line;
    while (ld != NULL)
    {
        td = ld->head_token;
        if (strncmp(td->token, "$", 1) == 0) // line starts with $
        {
            if (strncmp(td->next->token, "cd", 2) == 0) // command is cd
            {
                char * target = td->next->next->token;
                if (strncmp(target, "/", 1) == 0)
                {
#ifdef DEBUG_DAY_7_PARSE
                    printf("Parsing - cd to /\n");
#endif
                    cwd = slash;
                }
                else if (strncmp(target, "..", 2) == 0)
                {
#ifdef DEBUG_DAY_7_PARSE
                    printf("Parsing - cd to .. results in %s\n", cwd->parent);
#endif
                    cwd = cwd->parent;
                }
                else
                {
#ifdef DEBUG_DAY_7_PARSE
                    printf("Parsing - cd to %s\n", target);
#endif
                    day_7_directory_t * subdir = NULL;
                    for (int i=0; i<cwd->used_dirs; i++)
                    {
                        if (strncmp(target, cwd->dirs[i].dirname, DAY_7_NAME_LEN) == 0)
                        {
                            subdir = &cwd->dirs[i];
                            break;
                        }
                    }
                    if (subdir == NULL)
                    {
                        fprintf(stderr, "Subdirectory %s not found under %s\n", target, cwd->dirname);
                        file_data_cleanup(&fd);
                        return FALSE;
                    }
                    cwd = subdir;
                }
            }
            else if (strncmp(td->next->token, "ls", 2) == 0) // command is ls
            {
#ifdef DEBUG_DAY_7_PARSE
                printf("Parsing - ls under %s\n", cwd->dirname);
#endif
            }
            else
            {
                fprintf(stderr, "Invalid command %s\n", td->next->token);
                file_data_cleanup(&fd);
                return FALSE;
            }
        }
        else if (strncmp(td->token, "dir", 3) == 0)
        {
#ifdef DEBUG_DAY_7_PARSE
            printf("Parsing - adding directory %s under %s\n", td->next->token, cwd->dirname);
#endif
            add_directory(cwd, td->next->token);
        }
        else
        {
#ifdef DEBUG_DAY_7_PARSE
            printf("Parsing - adding file %s size %s under %s\n", td->next->token, td->token, cwd->dirname);
#endif
            add_file(cwd, td->next->token, strtol(td->token, NULL, 10));
        }
#ifdef DEBUG_DAY_7_PARSE
        dump_directory(slash, 0);
#endif
        ld = ld->next;
    }

    file_data_cleanup(&fd);
    
    return TRUE;
}

static void calc_total_size(day_7_directory_t * dir)
{
    dir->total_size = 0;
    for (int i=0; i<dir->used_files; i++)
    {
        dir->total_size += dir->files[i].size;
    }
    for (int i=0; i<dir->used_dirs; i++)
    {
        calc_total_size(&dir->dirs[i]);
        dir->total_size += dir->dirs[i].total_size;
    }
#ifdef DEBUG_DAY_7
    printf("Directory %s has total size %d\n", dir->dirname, dir->total_size);
#endif
}

static long sum_for_part_1(day_7_directory_t * dir)
{
    long my_sum = 0;
    if (dir->total_size <= 100000)
    {
        my_sum += dir->total_size;
    }
    for (int i=0; i<dir->used_dirs; i++)
    {
        my_sum += sum_for_part_1(&dir->dirs[i]);
    }
    return my_sum;
}

void day_7_part_1(char * filename, extra_args_t * extra_args, char * result)
{
    day_7_directory_t slash;
    init_directory(&slash, "/", NULL);
    dump_directory(&slash, 0);
    
    if (parse_input_and_build_tree(filename, &slash) != TRUE)
    {
        fprintf(stderr, "Error parsing input %s\n", filename);
    }
    
#ifdef DEBUG_DAY_7
    dump_directory(&slash, 0);
#endif

    calc_total_size(&slash);
    long total_sum = sum_for_part_1(&slash);
    
    snprintf(result, MAX_RESULT_LENGTH+1, "%ld", total_sum);
    
    cleanup_directory(&slash);
    
    return;
}
