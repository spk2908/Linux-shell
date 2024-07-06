#include "headers.h"

struct timeval start_time, end_time;
char *commandtoprint;

volatile sig_atomic_t ctc = 0;
volatile sig_atomic_t ctz = 0;

int main()
{
    // get home directory
    char *home = getcwd(NULL, 0);
    int len = strlen(home);

    // set previous directory
    char *prev_dir = malloc(sizeof(char) * 4096);
    memset(prev_dir, '\0', sizeof(char) * 4096);

    // make list of bg procs
    list l = create_proclist();
    // cmd *prev_cmd = malloc(sizeof(char));

    // make pastevents file
    FILE *file = fopen("pastevents", "a");
    fclose(file);

    start_time.tv_sec = 0;
    start_time.tv_usec = 0;
    end_time.tv_sec = 0;
    end_time.tv_usec = 0;
    commandtoprint = malloc(sizeof(char) * 4096);
    memset(commandtoprint, '\0', sizeof(char) * 4096);

    struct sigaction ctrlc_action;
    memset(&ctrlc_action, 0, sizeof(struct sigaction));
    ctrlc_action.sa_handler = ctrlc;
    ctrlc_action.sa_flags = 0;
    sigfillset(&ctrlc_action.sa_mask);
    sigaction(SIGINT, &ctrlc_action, NULL);

    struct sigaction ctrlz_action;
    memset(&ctrlz_action, 0, sizeof(struct sigaction));
    ctrlz_action.sa_handler = ctrlz;
    ctrlz_action.sa_flags = 0;
    sigfillset(&ctrlz_action.sa_mask);
    if(sigaction(SIGTSTP, &ctrlz_action, NULL) != 0){
        perror(RED"sigaction"RESET);
        return 1;
    }

    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        if (prompt(home, len) == 1)
        {
            continue;
        }

        strcpy(commandtoprint, "\0");

        char *input = malloc(sizeof(char) * 4096);
        if (fgets(input, 4096, stdin) == NULL)
        {
            if (ctc == 0 && ctz == 0)
            {
                // iterate through linked list and kill all children.
                list curr = l->next;
                while (curr != NULL)
                {
                    kill(curr->pid, SIGKILL);
                    curr = curr->next;
                }

                // free linked list
                list temp = l;
                while (l != NULL)
                {
                    temp = l;
                    l = l->next;
                    free(temp);
                }

                free(input);
                free(home);
                free(prev_dir);
                free(commandtoprint);
                exit(0);
            }
            else
            {
                ctc = 0;
                ctz = 0;
                printf("\n");
                free(input);
                delete_endedprocs(l);
                continue;
            }
        }
        ctc = 0;
        ctz = 0;
        delete_endedprocs(l);

        if (strlen(input) >= 1 && input[strlen(input) - 1] == '\n')
        {
            input[strlen(input) - 1] = '\0';
        }
        if (strlen(input) == 0)
        {
            free(input);
            input = NULL;
            continue;
        }

        char *formattedinput = format(input);
        if (formattedinput == NULL)
        {
            fprintf(stderr, RED "Invalid syntax\n" RESET);
            free(input);
            input = NULL;
            continue;
        }
        char *reconstructed = pasteventsreplace(formattedinput, home);

        if (reconstructed == NULL)
        {
            free(input);
            input = NULL;
            free(formattedinput);
            formattedinput = NULL;
            continue;
        }

        // printf("reconstructed = [%s]\n", reconstructed);

        parse(reconstructed, home, prev_dir, l);

        if (strstr(reconstructed, "pastevents") == NULL)
        {
            pasteventsadd(reconstructed, home);
        }

        free(input);
        input = NULL;
        free(formattedinput);
        formattedinput = NULL;
        free(reconstructed);
        reconstructed = NULL;
    }
}
