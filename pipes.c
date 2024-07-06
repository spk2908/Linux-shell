#include "headers.h"

#define READ_END 0
#define WRITE_END 1

void pipeline(char *newcommand, cmd *new, int bg, char *home, char *prev_dir, list proc_list)
{
    // process command for > and <
    // find first instance of < and ignore everything before it
    char *command = malloc(sizeof(char) * 4096);
    char* commandtofree = command;
    strcpy(command, newcommand);

    int i = 0;
    while(command[i] != '\0' && command[i] == ' '){
        i++;
    }
    if(command[i] == '|'){
        fprintf(stderr, RED"Invalid use of pipe\n"RESET);
        free(commandtofree);
        return;
    }
    int numpipes = 0;
    for (int i = 0; i < strlen(command); i++)
    {
        if (command[i] == '|')
        {
            if (i + 2 < strlen(command) && (command[i + 2] == ';' || command[i + 2] == '|' || command[i + 2] == '&'))
            {
                fprintf(stderr, RED"Invalid use of pipe\n"RESET);
                free(commandtofree);
                return;
            }
            else if (command[i + 1] == '\0')
            {
                fprintf(stderr, RED"Invalid use of pipe\n"RESET);
                free(commandtofree);
                return;
            }
            if (i - 2 >= 0 && (command[i - 2] == ';' || command[i - 2] == '|' || command[i - 2] == '&'))
            {
                fprintf(stderr, RED"Invalid use of pipe\n"RESET);
                free(commandtofree);
                return;
            }
            numpipes++;
        }
    }
    // printf("numpipes = %d\n", numpipes);
    if (numpipes == 0)
    {
        redirect(command, new, bg, home, prev_dir, proc_list);
        free(commandtofree);
        return;
    }

    int input_fd = dup(STDIN_FILENO);
    int output_fd = dup(STDOUT_FILENO);

    // printf("hello %d %d\n", input_fd, output_fd);

    // int i;
    char *temp = malloc(sizeof(char) * 4096);
    int inputidx = 0;

    int prev_fd[2];
    int new_fd[2];

    int j;

    for (j = 0; j <= numpipes; j += 1)
    {
        // print("hihihihihihi\n");
        i = 0;
        strcpy(temp, "\0");
        while (command[inputidx] != '|' && command[inputidx] != '\0')
        {
            temp[i] = command[inputidx];
            i++;
            inputidx++;
        }
        temp[i] = '\0';
        if (command[inputidx] == '|')
        {
            inputidx++;
        }

        // pipefd[0] refers to the read end of the pipe.  pipefd[1] refers to the write end of the pipe.

        if (pipe(new_fd) < 0)
        {
            perror(RED "pipe" RESET);
            return;
        }

        if (j > 0)
        {
            // not first command i.e. there is a previous cmd
            if (dup2(prev_fd[READ_END], STDIN_FILENO) < 0)
            {
                perror(RED "dup2" RESET);
                break;
            }
        }

        else
        {
            // first command
            if (dup2(input_fd, STDIN_FILENO) < 0)
            {
                perror(RED "dup2" RESET);
                break;
            }
        }

        if (j < numpipes)
        {
            // not last command i.e. there is a later command
            if (dup2(new_fd[WRITE_END], STDOUT_FILENO) < 0)
            {
                perror(RED "dup2" RESET);
                break;
            }
            close(new_fd[WRITE_END]);
        }
        else
        {
            // last command
            if (dup2(output_fd, STDOUT_FILENO) < 0)
            {
                perror(RED "dup2" RESET);
                break;
            }
        }

        // pipes done.

        // now do the redirects if necessary

        // fprintf(stderr, "temp = [%s] ", temp);

        int ret = redirect(temp, new, bg, home, prev_dir, proc_list);

        // fprintf(stderr, "returned %d\n", ret);

        if (j > 0)
        {
            close(prev_fd[0]);
            close(prev_fd[1]);
        }
        prev_fd[0] = new_fd[0];
        // prev_fd[1] = new_fd[1];

        if(ret != 0){
            break;
        }
    }


    dup2(input_fd, STDIN_FILENO);
    dup2(output_fd, STDOUT_FILENO);

    close(prev_fd[0]);
    close(prev_fd[1]);

    close(input_fd);
    close(output_fd);

    free(temp);
    free(commandtofree);
}