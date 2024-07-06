#include "headers.h"

int activities(list proc_list)
{
    node *curr = proc_list->next;
    char *path = malloc(400 * sizeof(char));
    if(path == NULL){
        perror(RED "malloc" RESET);
        return 1;
    }
    char *rand1 = malloc(sizeof(char) * 8192);
    if(rand1 == NULL){
        perror(RED "malloc" RESET);
        return 1;
    }
    int pid;
    char status;

    while (curr != NULL)
    {
        sprintf(path, "/proc/%d/stat", curr->pid);
        FILE *proc = fopen(path, "r");
        if (proc == NULL)
        {
            perror(RED "No such process exists" RESET);
            return 1;
        }
        fscanf(proc, "%d %s %c", &pid, rand1, &status);

        if(status == 'T'){
            printf("%d: %s - Stopped\n", curr->pid, curr->cmd);
        }
        else if(status != 'Z'){
            printf("%d: %s - Running\n", curr->pid, curr->cmd);
        }
        curr = curr->next;
    }

    free(path);
    free(rand1);
    return 0;
}