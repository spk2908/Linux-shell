#include "headers.h"

list create_proclist()
{
    node *new = (node *)malloc(sizeof(node));
    if (new == NULL)
    {
        perror(RED"malloc"RESET);
        return NULL;
    }
    new->pid = -1;
    new->next = NULL;
    return new;
}

void add_proc(list L, pid_t pid, char *command)
{
    // new node
    // insert in increasing order of pid
    node *new = (node *)malloc(sizeof(node));
    if (new == NULL)
    {
        perror(RED"malloc"RESET);
        return;
    }
    node* curr = L->next;
    node* prev = L;
    while(curr != NULL && curr->pid < pid)
    {
        prev = curr;
        curr = curr->next;
    }
    new->pid = pid;
    strcpy(new->cmd, command);
    new->next = curr;
    prev->next = new;
    // hopefully this works
    return;
}

void delete_endedprocs(list L)
{
    list curr = L->next;
    list prev = L;
    int status, ret;
    while (curr != NULL)
    {
        ret = waitpid(curr->pid, &status, WNOHANG);
        if (ret != 0 && ret != -1)
        {
            if (WIFEXITED(status))
            {
                fprintf(stderr, "%s exited normally (%d)\n", curr->cmd, curr->pid);
            }
            else
            {
                fprintf(stderr, "%s failed (%d)\n", curr->cmd, curr->pid);
            }
            prev->next = curr->next;
            free(curr);
            curr = prev->next;
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
}