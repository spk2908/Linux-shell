#include "headers.h"

int execute(cmd *input, int fd1, int fd2, char *home, char *prev_dir, list proc_list)
{
    // execute child
    if (input->argc <= 0)
    {
        return 1;
    }

    if (strcmp(input->argv[0], "warp") == 0)
    {
        // printf("and went to warp.\n");
        return warp(input, home, prev_dir);
    }
    else if (strcmp(input->argv[0], "peek") == 0)
    {
        // printf("and went to peek.\n");
        return peek(input, home);
    }
    else if (strcmp(input->argv[0], "pastevents") == 0)
    {
        // printf("and went to pastevents.\n");
        return pastevents(input, home, prev_dir, proc_list);
    }
    else if (strcmp(input->argv[0], "proclore") == 0)
    {
        return proclore(input, home);
    }
    else if (strcmp(input->argv[0], "seek") == 0)
    {
        return seek(input, home, prev_dir);
    }
    else if (strcmp(input->argv[0], "neonate") == 0)
    {
        return neonate(input);
    }
    else if (strcmp(input->argv[0], "iMan") == 0)
    {
        return iman(input);
    }
    else if (strcmp(input->argv[0], "activities") == 0)
    {
        if (input->argc > 1)
        {
            fprintf(stderr, RED "activities: too many arguments\n" RESET);
            return 1;
        }
        return activities(proc_list);
    }
    else if (strcmp(input->argv[0], "fg") == 0)
    {
        return fg(input, proc_list);
    }
    else if (strcmp(input->argv[0], "bg") == 0)
    {
        return bg(input, proc_list);
    }
    else if (strcmp(input->argv[0], "ping") == 0)
    {
        return ping(input);
    }
    else // execvp
    {

        gettimeofday(&start_time, NULL); // Get the starting time

        pid_t child_pid = fork();

        if (child_pid == 0)
        {
            if (fd1 != -1)
            {
                close(fd1);
            }
            if (fd2 != -1)
            {
                close(fd2);
            }

            if (input->bg == 1)
            {
                setpgid(0, 0);
            }
            execvp(input->argv[0], &input->argv[0]); // execute child
            fprintf(stderr, RED "ERROR: %s is not a valid command\n" RESET, input->argv[0]);
            exit(1);
        }

        else if (child_pid > 0)
        { // Parent process
            // add child pid and child command to linked list
            if (input->bg == 1)
            {
                start_time.tv_sec = 0;
                start_time.tv_usec = 0;
                fprintf(stderr, "%d\n", child_pid);
                add_proc(proc_list, child_pid, input->argv[0]);
                return 0;
            }
            else
            {
                int status;
                while (waitpid(child_pid, &status, WNOHANG) != -1 && ctc == 0 && ctz == 0)
                {
                    continue;
                }

                if (ctc == 1)
                {
                    kill(child_pid, SIGINT);
                    ctc = 0;
                    gettimeofday(&end_time, NULL); // Get the ending time
                    strcpy(commandtoprint, input->argv[0]);
                    printf("\n");
                }
                if (ctz == 1)
                {
                    kill(child_pid, SIGTSTP);
                    gettimeofday(&end_time, NULL); // Get the ending time
                    strcpy(commandtoprint, input->argv[0]);
                    add_proc(proc_list, child_pid, input->argv[0]);
                    ctz = 0;
                    printf("\n");
                    return 1;
                }
                if (WIFEXITED(status))
                {
                    gettimeofday(&end_time, NULL); // Get the ending time
                    strcpy(commandtoprint, input->argv[0]);
                    // printf("\ntime spent = %d\n", input->time);
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status == 0)
                    {
                        return 0;
                    }
                    else
                    {
                        return 1;
                    }
                }
                else
                {
                    return 1;
                }
            }
        }

        else
        {
            fprintf(stderr, RED "fork" RESET);
            return 1;
        }
    }
}