#include "headers.h"

int fg(cmd *input, list bg_procs)
{
    if (input->argc != 2)
    {
        fprintf(stderr, RED "fg: exactly one argument expected\n" RESET);
        return 1;
    }

    // pid = input->argv[1]

    errno = 0;
    long pid = strtol(input->argv[1], NULL, 10);
    if (errno != 0)
    {
        perror(RED "No such process found.\n" RESET);
        return 1;
    }
    if (pid <= 0)
    {
        fprintf(stderr, RED "No such process found\n" RESET);
        return 1;
    }
    for (int i = 0; i < strlen(input->argv[1]); i++)
    {
        if (input->argv[1][i] < '0' || input->argv[1][i] > '9')
        {
            fprintf(stderr, RED "No such process found\n" RESET);
            return 1;
        }
    }

    // check if pid exists in bg_procs
    list curr = bg_procs->next;
    list prev = bg_procs;
    while (curr != NULL)
    {
        if ((long)curr->pid == pid)
        {
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL)
    {
        fprintf(stderr, RED "No such process\n" RESET);
        return 1;
    }

    // send SIGCONT
    if (kill(curr->pid, SIGCONT) == -1)
    {
        perror(RED "kill" RESET);
        return 1;
    }
    // wait for process to end
    gettimeofday(&start_time, NULL); // Get the starting time
    // waitpid(curr->pid, NULL, 0);
    while (waitpid(curr->pid, NULL, WNOHANG) != -1 && ctc == 0 && ctz == 0)
    {
        continue;
    }
    if (ctc == 1)
    {
        kill(curr->pid, SIGKILL);
        ctc = 0;
        printf("\n");
    }
    if (ctz == 1)
    {
        kill(curr->pid, SIGTSTP);
        start_time.tv_sec = 0;
        start_time.tv_usec = 0;
        ctz = 0;
        printf("\n");
        return 0;
    }
    gettimeofday(&end_time, NULL); // Get the ending time
    strcpy(commandtoprint, curr->cmd);

    // delete from linked list
    prev->next = curr->next;
    free(curr);

    return 0;
}

int bg(cmd *input, list bg_procs)
{
    if (input->argc != 2)
    {
        fprintf(stderr, RED "bg: exactly one argument expected\n" RESET);
        return 1;
    }

    // pid = input->argv[1]
    errno = 0;
    long pid = strtol(input->argv[1], NULL, 10);
    if (errno != 0)
    {
        perror(RED "No such process found.\n" RESET);
        return 1;
    }
    if (pid <= 0)
    {
        fprintf(stderr, RED "No such process found\n" RESET);
        return 1;
    }
    for (int i = 0; i < strlen(input->argv[1]); i++)
    {
        if (input->argv[1][i] < '0' || input->argv[1][i] > '9')
        {
            fprintf(stderr, RED "No such process found\n" RESET);
            return 1;
        }
    }

    // check if pid exists in bg_procs
    list curr = bg_procs->next;
    list prev = bg_procs;
    while (curr != NULL)
    {
        if (curr->pid == pid)
        {
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL)
    {
        fprintf(stderr, RED "No such process found\n" RESET);
        return 1;
    }

    // check if the process is stopped
    char *path = malloc(400 * sizeof(char));
    if (path == NULL)
    {
        perror(RED "malloc" RESET);
        return 1;
    }
    sprintf(path, "/proc/%d/stat", curr->pid);
    FILE *proc = fopen(path, "r");
    if (proc == NULL)
    {
        perror(RED "No such process exists" RESET);
        return 1;
    }
    int pid1;
    char status;
    fscanf(proc, "%d %s %c", &pid1, path, &status);
    free(path);
    if (status != 'T')
    {
        fprintf(stderr, RED "Process is not stopped.\n" RESET);
        return 1;
    }

    // send SIGCONT
    if (kill(curr->pid, SIGCONT) == -1)
    {
        perror(RED "kill" RESET);
        return 1;
    }

    return 0;
}

void ctrlc(int signal_number)
{
    ctc = 1;
}

void ctrlz(int signal_number)
{
    ctz = 1;
}