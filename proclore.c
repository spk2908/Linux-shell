#include "headers.h"

int proclore(cmd *input, char *home)
{
    /*
    /proc/pid/stat
    pid 1
    Process Status (R/R+/S/S+/Z) 3, tpgid (for fg)
    Process group 5
    Virtual Memory 23
    /proc/pid/exe
    Executable path of process
    readlink
    */

    if (input->argc <= 0 || strcmp("proclore", input->argv[0]) != 0)
    {
        return 1;
    }
    if (input->argc == 1)
    {
        // use proc/self/stat
        FILE *proc = fopen("/proc/self/stat", "r");
        if (proc == NULL)
        {
            perror(RED "No such process exists" RESET);
            return 1;
        }
        int pid;
        char status;
        int fgrp;
        int pgrp;
        unsigned long int vmem;

        char *rand1 = malloc(sizeof(char) * 8192);
        if (rand1 == NULL)
        {
            perror(RED "malloc" RESET);
            return 1;
        }
        int rand2;
        unsigned rand3;
        long unsigned rand4;
        long int rand5;
        long long unsigned int rand6;
        errno = 0;
        fscanf(proc, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu",
               &pid, rand1, &status, &rand2, &pgrp, &rand2, &rand2, &fgrp, &rand3, &rand4, &rand4, &rand4, &rand4, &rand4, &rand4, &rand5, &rand5, &rand5, &rand5, &rand5, &rand5, &rand6, &vmem);

        if (errno != 0)
        {
            perror(RED "proclore" RESET);
            return 1;
        }
        free(rand1);
        printf("pid : %d\n", pid);
        printf("process status : %c", status);
        if (pgrp == fgrp)
        {
            printf("+");
        }
        printf("\n");
        printf("process group : %d\n", pgrp);
        printf("virtual memory : %lu\n", vmem);
        fclose(proc);

        char *buffer = malloc(sizeof(char) * 8192);
        if (buffer == NULL)
        {
            perror(RED "malloc" RESET);
            return 1;
        }
        memset(buffer, '\0', sizeof(char) * 8192);
        ssize_t len = readlink("/proc/self/exe", buffer, 8192);
        if (len == -1)
        {
            free(buffer);
            perror(RED "executable path" RESET);
            return 0;
        }
        if (prefix(home, buffer) == 0)
        {
            printf("executable path : ~%s\n", buffer + strlen(home));
        }
        else
        {
            printf("executable path : %s\n", buffer);
        }
        free(buffer);
        return 0;
    }

    else if (input->argc == 2)
    {
        char *pid_str = malloc(sizeof(char) * 8192);
        if (pid_str == NULL)
        {
            perror(RED "malloc" RESET);
            return 1;
        }
        strcpy(pid_str, "/proc/");
        strcat(pid_str, input->argv[1]);
        strcat(pid_str, "/stat");

        FILE *proc = fopen(pid_str, "r");
        if (proc == NULL)
        {
            perror(RED "No such process exists" RESET);
            return 1;
        }
        int pid;
        char status;
        int fgrp;
        int pgrp;
        unsigned long int vmem;

        char *rand1 = malloc(sizeof(char) * 8192);
        if (rand1 == NULL)
        {
            perror(RED "malloc" RESET);
            return 1;
        }
        int rand2;
        unsigned rand3;
        long unsigned rand4;
        long int rand5;
        long long unsigned int rand6;
        errno = 0;
        fscanf(proc, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu",
               &pid, rand1, &status, &rand2, &pgrp, &rand2, &rand2, &fgrp, &rand3, &rand4, &rand4, &rand4, &rand4, &rand4, &rand4, &rand5, &rand5, &rand5, &rand5, &rand5, &rand5, &rand6, &vmem);

        if (errno != 0)
        {
            perror(RED "proclore" RESET);
            return 1;
        }
        free(rand1);
        fclose(proc);

        strcpy(pid_str, "/proc/");
        strcat(pid_str, input->argv[1]);
        strcat(pid_str, "/exe");

        printf("pid : %d\n", pid);
        printf("process status : %c", status);
        if (pgrp == fgrp)
        {
            printf("+");
        }
        printf("\n");
        printf("process group : %d\n", pgrp);
        printf("virtual memory : %lu\n", vmem);

        char *buffer = malloc(sizeof(char) * 8192);
        if (buffer == NULL)
        {
            perror(RED "malloc" RESET);
            free(pid_str);
            return 1;
        }
        memset(buffer, '\0', sizeof(char) * 8192);
        ssize_t len = readlink(pid_str, buffer, 8192);
        if (len == -1)
        {
            free(buffer);
            free(pid_str);
            perror(RED"executable path"RESET);
            return 0;
        }
        if (prefix(home, buffer) == 0)
        {
            printf("executable path : ~%s\n", buffer + strlen(home));
        }
        else
        {
            printf("executable path : %s\n", buffer);
        }
        free(buffer);
        free(pid_str);
        return 0;
    }

    else
    {
        fprintf(stderr, RED "proclore: Invalid syntax\n" RESET);
        return 1;
    }
}