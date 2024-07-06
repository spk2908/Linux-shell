#include "headers.h"

void parse(char *input, char *home, char *prev_dir, list proc_list)
{
    // nicely formatted input
    char *temp = malloc(4096 * sizeof(char));
    strcpy(temp, "\0");
    // read input into temp until & or ;
    int inputidx = 0;
    int i;
    cmd *currentcommand = malloc(sizeof(cmd));
    currentcommand->argc = 0;
    currentcommand->argv = NULL;
    // char *replacedcmd = malloc(4096 * sizeof(char));
    // strcpy(replacedcmd, "\0");

    while (1)
    {
        i = 0;
        if (inputidx >= strlen(input) || input[inputidx] == '\0')
        {
            break;
        }
        while (input[inputidx] == ';' || input[inputidx] == '&' || input[inputidx] == ' ')
        {
            inputidx++;
        }

        while (input[inputidx] != ';' && input[inputidx] != '&' && input[inputidx] != '\0')
        {
            // printf("inputidx = %d, char = [%c]\n", inputidx, input[inputidx]);
            temp[i] = input[inputidx];
            i++;
            inputidx++;
        }

        temp[i] = '\0';
        // printf("temp = [%s]\n", temp);

        if (input[inputidx] == '&')
        {
            pipeline(temp, currentcommand, 1, home, prev_dir, proc_list);
        }
        else
        {
            pipeline(temp, currentcommand, 0, home, prev_dir, proc_list);
        }

        // free the strings inside current command

        strcpy(temp, "\0");
    }

    free(temp);
    temp = NULL;

    free(currentcommand);
    currentcommand = NULL;

    return;
}


int parse_cmd(char *command, int fd1, int fd2, cmd *new, int bg, char *home, char *prev_dir, list proc_list)
{
    char *commandcopy = malloc(4096 * sizeof(char));
    char *saveptr;
    strcpy(commandcopy, command);
    new->bg = bg;
    new->argc = 0;
    new->argv = malloc(2 * sizeof(char *)); // at first initialise only 2 indeces (num args <= 2)
    if (new->argv == NULL)
    {
        perror(RED "malloc" RESET);
        return 1;
    }
    int count = 0, indeces = 2;

    char *temp = strtok_r(commandcopy, " \t", &saveptr);

    while (temp != NULL)
    {
        // printf("%s\n", temp);
        if (count < indeces)
        {
            new->argv[count] = malloc(4096 * sizeof(char));
            strcpy(new->argv[count], temp);
            count++;
        }
        else
        {
            // double the indeces available
            char **tomp = realloc(new->argv, (indeces * 2) * sizeof(char *));
            if (tomp == NULL)
            {
                perror(RED "realloc" RESET);
                free(new->argv); // Release the previously allocated memory
                new->argc = 0;
                return 1;
            }
            new->argv = tomp;
            indeces *= 2;
            new->argv[count] = malloc(4096 * sizeof(char));
            strcpy(new->argv[count], temp);
            count++;
        }
        temp = strtok_r(NULL, " \t", &saveptr);
    }
    if (count < indeces)
    {
        new->argv[count] = NULL;
    }
    else
    {
        char **tomp = realloc(new->argv, (indeces + 1) * sizeof(char *));
        if (tomp == NULL)
        {
            perror(RED "Failed to reallocate memory" RESET);
            free(new->argv); // Release the previously allocated memory
            new->argc = 0;
            return 1;
        }
        new->argv = tomp;
        new->argv[indeces] = NULL;
    }
    new->argc = count;
    // printf("argc count = %d\n", count);
    int ret = execute(new, fd1, fd2, home, prev_dir, proc_list);

    for (int i = 0; i < new->argc; i++)
    {
        if (new->argv[i] != NULL)
        {
            free(new->argv[i]);
        }
        new->argv[i] = NULL;
    }
    if (new->argv != NULL)
    {
        free(new->argv);
    }
    new->argv = NULL;
    new->argc = 0;

    free(commandcopy);
    commandcopy = NULL;

    return ret;
}
