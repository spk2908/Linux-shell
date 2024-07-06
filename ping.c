#include "headers.h"

int ping(cmd *input)
{
    if (input->argc != 3)
    {
        fprintf(stderr, RED "ping: Invalid number of arguments\n" RESET);
        return 1;
    }
    errno = 0;
    long pid = strtol(input->argv[1], NULL, 10);
    if (pid <= 0)
    {
        fprintf(stderr, RED "No such process found\n" RESET);
        return 1;
    }
    if (errno != 0)
    {
        perror(RED "No such process found.\n" RESET);
        return 1;
    }
    for (int i = 0; i < strlen(input->argv[1]); i++)
    {
        // if character is not a number perror
        if (input->argv[1][i] < '0' || input->argv[1][i] > '9')
        {
            fprintf(stderr, RED "Invalid process found\n" RESET);
            return 1;
        }
    }

    errno = 0;
    long sig = strtol(input->argv[2], NULL, 10);
    if (sig <= 0)
    {
        fprintf(stderr, RED "Invalid signal number\n" RESET);
        return 1;
    }
    if (errno != 0)
    {
        perror(RED "Invalid signal number.\n" RESET);
        return 1;
    }
    for (int i = 0; i < strlen(input->argv[2]); i++)
    {
        // if character is not a number perror
        if (input->argv[2][i] < '0' || input->argv[2][i] > '9')
        {
            fprintf(stderr, RED "Invalid signal number\n" RESET);
            return 1;
        }
    }
    sig = sig % 32;

    errno = 0;
    int ret = kill(pid, sig);
    printf("Sent signal %ld to process with pid %ld\n", sig, pid);
    if (ret < 0)
    {
        if (errno == EINVAL)
        {
            fprintf(stderr, RED "Invalid signal number\n" RESET);
        }
        else if (errno == ESRCH)
        {
            fprintf(stderr, RED "No such process found\n" RESET);
        }
        else
        {
            fprintf(stderr, RED "Error in kill\n" RESET);
        }
    }
    return 0;
}