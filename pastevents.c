#include "headers.h"

int pastevents(cmd *input, char *home, char *prev_dir, list proc_list)
{
    if (input->argc <= 0 || strcmp(input->argv[0], "pastevents") != 0)
    {
        return 1;
    }

    char *path = malloc((strlen(home) + 20) * sizeof(char));
    if (path == NULL)
    {
        perror(RED "malloc" RESET);
        return 1;
    }
    strcpy(path, home);
    strcat(path, "/pastevents");

    if (input->argc >= 2)
    {
        if (strcmp(input->argv[1], "purge") == 0)
        {
            FILE *file = fopen(path, "w");
            if (file == NULL)
            {
                perror(RED "pastevents" RESET);
                return 1;
            }
            if (fclose(file) != 0)
            {
                perror(RED "pastevents" RESET);
            }
            free(path);
            path = NULL;
            return 0;
        }

        else if (strcmp(input->argv[1], "execute") == 0)
        {
            if (input->argc == 2)
            {
                fprintf(stderr, RED "\033[1;31mpastevents: Invalid syntax\033[0m\n" RESET);
                free(path);
                return 1;
            }

            int cmd = 0;
            char **arr = malloc(15 * sizeof(char *));
            if (arr == NULL)
            {
                perror(RED "malloc" RESET);
                return 1;
            }

            // malloc 4096 array for each one
            for (int i = 0; i < 15; i++)
            {
                arr[i] = malloc(4096 * sizeof(char));
                if (arr[i] == NULL)
                {
                    perror(RED "malloc" RESET);
                    for (int j = 0; j < i; j++)
                    {
                        free(arr[j]);
                        arr[j] = NULL;
                    }
                    free(arr);
                    arr = NULL;
                    return 1;
                }
            }

            readpastevents(arr, &cmd, home);

            errno = 0;
            long int index = strtol(input->argv[2], NULL, 10);
            if (errno != 0)
            {
                fprintf(stderr, "\033[1;31mpastevents: Invalid index\033[0m\n");
                for (int i = 0; i < 15; i++)
                {
                    free(arr[i]);
                    arr[i] = NULL;
                }
                free(arr);
                return 1;
            }
            if (index < 1 || index > 15 || index > cmd)
            {
                fprintf(stderr, "\033[1;31mpastevents: Invalid index\033[0m\n");
                for (int i = 0; i < 15; i++)
                {
                    free(arr[i]);
                    arr[i] = NULL;
                }
                // input->argv[2] = NULL;
                free(arr);
                free(path);
                return 1;
            }
            for (int i = 0; i < strlen(input->argv[2]); i++)
            {
                // if character is not a number perror
                if (input->argv[2][i] < '0' || input->argv[2][i] > '9')
                {
                    fprintf(stderr, "\033[1;31mpastevents: Invalid index\033[0m\n");
                    for (int i = 0; i < 15; i++)
                    {
                        free(arr[i]);
                        arr[i] = NULL;
                    }
                    free(arr);
                    free(path);
                    return 1;
                }
            }

            char *command = malloc((strlen(arr[cmd - index]) + 2) * sizeof(char));
            if (command == NULL)
            {
                perror(RED "malloc" RESET);
                return 1;
            }
            strcpy(command, arr[cmd - index]);
            // printf("will try to execute %s in index %d\n", command, index);
            parse(command, home, prev_dir, proc_list);
            // free(input->argv[2]);
            // input->argv[2] = malloc((strlen(command) + 2) * sizeof(char));
            // strcpy(input->argv[2], command);
            // free(command);

            for (int i = 0; i < 15; i++)
            {
                // printf("printing %s to file\n", arr[i]);
                free(arr[i]);
                arr[i] = NULL;
            }
            free(arr);
            free(path);
            free(command);
            path = NULL;
            return 0;
            // replace input with parsed command
        }
        else
        {
            printf("\033[1;31mpastevents: Invalid syntax\033[0m\n");
            free(path);
            return 1;
        }
    }

    // print file contents
    // printf("pasteventshihihi:\n");
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        fprintf(stderr, RED "\033[1;31mpastevents: could not open file\033[0m\n" RESET);
        return 1;
    }
    char *buffer = malloc(4096 * sizeof(char));
    if (buffer == NULL)
    {
        perror(RED "malloc" RESET);
        return 1;
    }
    while (fgets(buffer, 4096, file) != NULL)
    {
        printf("%s", buffer);
    }
    free(buffer);
    if (fclose(file) != 0)
    {
        perror(RED "pastevents" RESET);
    }
    free(path);
    path = NULL;
    return 0;
}

void pasteventsadd(char *command, char *home)
{
    int cmd = 0;
    char **arr = malloc(15 * sizeof(char *));
    if (arr == NULL)
    {
        perror(RED "malloc" RESET);
        return;
    }

    // malloc 4096 array for each one
    for (int i = 0; i < 15; i++)
    {
        arr[i] = malloc(4096 * sizeof(char));
        if (arr[i] == NULL)
        {
            perror(RED "malloc" RESET);
            for (int j = 0; j < i; j++)
            {
                free(arr[j]);
                arr[j] = NULL;
            }
            free(arr);
            arr = NULL;
            return;
        }
    }
    readpastevents(arr, &cmd, home);
    if (arr == NULL)
    {
        fprintf(stderr, RED "pasteventsadd: error reading pastevents\n" RESET);
        return;
    }

    // printf("[adding]cmds = %d\n", cmd);
    // for (int i = 0; i < cmd; i++)
    // {
    //     printf("%s\n", arr[i]);
    // }
    // printf("done---\n");

    char *path = malloc((strlen(home) + 20) * sizeof(char));
    if (path == NULL)
    {
        perror(RED "malloc" RESET);
        return;
    }
    strcpy(path, home);
    strcat(path, "/pastevents");

    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        perror(RED "Error opening file" RESET);
        return;
    }

    // if command not the same as last line
    if (cmd == 0 || arr == NULL)
    {
        fprintf(file, "%s\n", command);
    }
    else if (cmd >= 1 && strcmp(command, arr[cmd - 1]) != 0)
    {
        // printf("comparison: %s || %s %d", command, arr[cmd - 1], strcmp(command, arr[cmd - 1]));
        // if commands > 15 then delete first line
        if (cmd >= 15)
        {
            // write strings 2 to 15
            for (int i = 1; i < 15; i++)
            {
                fprintf(file, "%s\n", arr[i]);
            }
            // then write command
            fprintf(file, "%s\n", command);
        }
        else
        {
            // write all cmds into file followed by command
            for (int i = 0; i < cmd; i++)
            {
                fprintf(file, "%s\n", arr[i]);
            }
            fprintf(file, "%s\n", command);
        }
    }
    else
    {
        for (int i = 0; i < cmd; i++)
        {
            fprintf(file, "%s\n", arr[i]);
        }
    }

    free(path);
    path = NULL;
    // have to free arr
    for (int i = 0; i < 15; i++)
    {
        free(arr[i]);
        arr[i] = NULL;
    }
    free(arr);
    arr = NULL;

    if (fclose(file) != 0)
    {
        perror(RED "pastevents" RESET);
    }
}

void readpastevents(char **arr, int *count, char *home)
{
    *count = 0;
    char *path = malloc((strlen(home) + 20) * sizeof(char));
    if (path == NULL)
    {
        perror(RED "malloc" RESET);
        *count = 0;
        return;
    }
    strcpy(path, home);
    strcat(path, "/pastevents");

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        perror(RED "pastevents" RESET);
        return;
    }

    *count = 0;
    // char c;
    while (fgets(arr[*count], 4096, file))
    {
        if (arr[*count][strlen(arr[*count]) - 1] == '\n')
        {
            arr[*count][strlen(arr[*count]) - 1] = '\0';
        }
        *count = *count + 1;
        if (*count >= 15)
        {
            break;
        }
    }

    fclose(file);
    free(path);
    path = NULL;
    return;
}

char *pasteventsreplace(char *input, char *home)
{
    char *replacedstr = malloc(4096 * sizeof(char));
    strcpy(replacedstr, input);
    if (strlen(input) == 0)
    {
        return replacedstr;
    }
    // check if pastevents execute exists in the string
    char *ptr = strstr(replacedstr, "pastevents execute ");

    if (ptr == NULL)
    {
        return replacedstr;
    }
    // otherwise pastevents execute was found

    int count = 0;
    char **arr = malloc(15 * sizeof(char *));
    if (arr == NULL)
    {
        perror(RED "malloc" RESET);
        return replacedstr;
    }

    // malloc 4096 array for each one
    for (int i = 0; i < 15; i++)
    {
        arr[i] = malloc(4096 * sizeof(char));
        if (arr[i] == NULL)
        {
            perror(RED "malloc" RESET);
            for (int j = 0; j < i; j++)
            {
                free(arr[j]);
                arr[j] = NULL;
            }
            free(arr);
            arr = NULL;
            return replacedstr;
        }
    }
    readpastevents(arr, &count, home);

    char *temp = malloc(4096 * sizeof(char));
    if (temp == NULL)
    {
        perror(RED "malloc" RESET);
        return replacedstr;
    }
    strcpy(temp, "\0");

    while (ptr != NULL)
    {
        // check what the pastevents index is
        int index = (ptr + strlen("pastevents execute ")) - replacedstr;
        int i = 0;
        while (replacedstr[index] != ' ' && replacedstr[index] != '\0')
        {
            temp[i] = replacedstr[index];
            i++;
            index++;
        }
        temp[i] = '\0';

        errno = 0;
        long int j = strtol(temp, NULL, 10);
        if (errno != 0)
        {
            for (int i = 0; i < 15; i++)
            {
                free(arr[i]);
                arr[i] = NULL;
            }
            free(temp);
            free(arr);
            return replacedstr;
        }
        if (j < 1 || j > 15 || j > count)
        {
            for (int i = 0; i < 15; i++)
            {
                free(arr[i]);
                arr[i] = NULL;
            }
            free(temp);
            free(arr);
            return replacedstr;
        }
        for (int i = 0; i < strlen(temp); i++)
        {
            // if character is not a number perror
            if (temp[i] < '0' || temp[i] > '9')
            {
                for (int i = 0; i < 15; i++)
                {
                    free(arr[i]);
                    arr[i] = NULL;
                }
                free(temp);
                free(arr);
                return replacedstr;
            }
        }

        strcpy(temp, "\0");
        // strncpy(temp, replacedstr, ptr - replacedstr);

        // copy in what is before the pastevents execute
        int substrPos = ptr - replacedstr;
        int k = 0;
        for (k = 0; k < substrPos; k++)
        {
            temp[k] = replacedstr[k];
        }
        temp[k] = '\0';

        // copy in what is in pastevents file
        strcat(temp, arr[count - j]);

        // if there is an ; or & at the end of temp
        int tempidx = strlen(temp) - 1;
        while (tempidx >= 0 && temp[tempidx] == ' ')
        {
            tempidx--;
        }
        if (tempidx >= 0 && (temp[tempidx] == ';' || temp[tempidx] == '&'))
        {
            // if there is a ; or & at the beginning of replacedstr+index
            int idx = 0;
            while (replacedstr[index + idx] == ' ')
            {
                idx++;
            }
            if (replacedstr[index + idx] != '\0' && (replacedstr[index + idx] == ';' || replacedstr[index + idx] == '&'))
            {
                idx++;
            }
            // copy in rest of the string
            strcat(temp, replacedstr + index + idx);
        }
        else
        {
            // copy in rest of the string
            strcat(temp, replacedstr + index);
        }

        // // copy in rest of the string
        // strcat(temp, replacedstr + index);

        strcpy(replacedstr, temp);

        // printf("replacedstr = [%s]\n", replacedstr);

        strcpy(temp, "\0");
        ptr = strstr(replacedstr, "pastevents execute ");
    }

    // free arr
    for (int i = 0; i < 15; i++)
    {
        // printf("printing %s to file\n", arr[i]);
        free(arr[i]);
        arr[i] = NULL;
    }
    free(arr);
    free(temp);
    return replacedstr;
}
