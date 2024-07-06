#include "headers.h"

int changedir(const char *path)
{
    int ret = chdir(path);
    if (ret < 0)
    {
        perror(RED "warp failed" RESET);
        return 1;
    }
    else
    {
        char *curr_dir = malloc(sizeof(char) * 2048);
        getcwd(curr_dir, 2048);
        printf("%s\n", curr_dir);
        free(curr_dir);
        curr_dir = NULL;
        return 0;
    }
}

int warp(cmd *input, const char *home, char *prev_dir)
{

    if (input->argc <= 0 || strcmp("warp", input->argv[0]) != 0)
    {
        return 1;
    }
    int ret;

    if (input->argc == 1)
    {
        char *curr_dir = getcwd(NULL, 0); // save curr dir
        if (curr_dir == NULL)
        {
            perror(RED "warp failed" RESET);
            return 1;
        }
        if (changedir(home) == -1)
        {
            free(curr_dir);
            perror(RED "warp failed" RESET);
            return 1;
        }

        strcpy(prev_dir, curr_dir);
        free(curr_dir);
        return 0;
    }

    for (int j = 1; j < input->argc; j++)
    {
        // printf("PREV DIR = %s\n", *prev_dir);

        char *curr = input->argv[j];

        char *curr_dir = getcwd(NULL, 0); // save curr dir
        if (curr_dir == NULL)
        {
            perror(RED "warp failed" RESET);
            return 1;
        }

        if (curr[0] == '-')
        { // 2. wants to go to prev dir
            if (prev_dir[0] != '\0')
            {
                ret = changedir(prev_dir);
            }
            else
            {
                fprintf(stderr, RED"OLDPWD not set\n"RESET);
                // curr = strtok_r(NULL, " \t", &input_dup);
                continue;
            }
        }
        else if (curr[0] == '~')
        { // 1. starts with ~
            char *path = malloc((strlen(home) + strlen(curr) + 1) * sizeof(char));
            if (path == NULL)
            {
                perror(RED "malloc" RESET);
                return 1;
            }
            strcpy(path, home);
            strcat(path, curr + 1);
            ret = changedir(path);
            free(path);
            path = NULL;
        }
        else
        {
            ret = changedir(curr);
        }

        // change prev dir to the saved dir
        if (ret == 0)
        {
            strcpy(prev_dir, curr_dir);
            free(curr_dir);
        }
        else if(j == input->argc-1)
        {
            return 1;
        }

        // curr = strtok_r(NULL, " \t", &input_dup);
    }
    return 0;

    // ===== once we have the input in path
}

