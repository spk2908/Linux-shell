#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */

int neonate(cmd *input)
{
    if (input->argc <= 2)
    {
        fprintf(stderr, RED "neonate: too few arguments\n" RESET);
        return 1;
    }
    else if (input->argc > 3)
    {
        fprintf(stderr, RED "neonate: too many arguments\n" RESET);
        return 1;
    }
    else if (strcmp(input->argv[1], "-n") != 0)
    {
        fprintf(stderr, RED "neonate: invalid option\n" RESET);
        return 1;
    }

    errno = 0;
    long interval = strtol(input->argv[2], NULL, 10);
    if (errno != 0)
    {
        fprintf(stderr, RED "neonate: invalid interval\n" RESET);
        return 1;
    }
    if (interval <= 0)
    {
        fprintf(stderr, RED "neonate: invalid interval\n" RESET);
        return 1;
    }
    for (int i = 0; i < strlen(input->argv[2]); i++)
    {
        if (input->argv[2][i] < '0' || input->argv[2][i] > '9')
        {
            fprintf(stderr, RED "neonate: invalid interval\n" RESET);
            return 1;
        }
    }

    int original_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (original_flags == -1)
    {
        perror(RED "fcntl" RESET);
        return 1;
    }

    double loadavg1, loadavg5, loadavg15;
    int runningProcesses, totalProcesses;
    pid_t lastpid;
    char c;
    enableRawMode();
    setbuf(stdout, NULL);

    if (fcntl(STDIN_FILENO, F_SETFL, original_flags | O_NONBLOCK) == -1)
    {
        perror(RED "fcntl" RESET);
        return 1;
    }

    FILE *file = fopen("/proc/loadavg", "r");
    if (file == NULL)
    {
        perror(RED "Failed to open /proc/loadavg" RESET);
        if (fcntl(STDIN_FILENO, F_SETFL, original_flags) == -1)
        {
            perror(RED "fcntl" RESET);
            return 1;
        }
        return 1;
    }
    // print for the first time
    fscanf(file, "%lf %lf %lf %d/%d %d", &loadavg1, &loadavg5, &loadavg15, &runningProcesses, &totalProcesses, &lastpid);
    printf("%d\n", lastpid);
    fclose(file);

    time_t start_time = time(NULL);

    while (1)
    {
        // printf("hi\n");
        time_t current_time = time(NULL);
        if (current_time - start_time >= interval)
        {
            FILE *file = fopen("/proc/loadavg", "r");
            if (file == NULL)
            {
                perror(RED "Failed to open /proc/loadavg" RESET);
                if (fcntl(STDIN_FILENO, F_SETFL, original_flags) == -1)
                {
                    perror(RED "fcntl" RESET);
                    return 1;
                }
                return 1;
            }
            fscanf(file, "%lf %lf %lf %d/%d %d", &loadavg1, &loadavg5, &loadavg15, &runningProcesses, &totalProcesses, &lastpid);
            printf("%d\n", lastpid);
            fclose(file);
            start_time = current_time;
        }

        if (read(STDIN_FILENO, &c, 1) == -1)
        {
            continue;
        }

        if (c == 120)
        {
            break;
        }
    }
    disableRawMode();
    if (fcntl(STDIN_FILENO, F_SETFL, original_flags) == -1)
    {
        perror(RED "fcntl" RESET);
        return 1;
    }

    return 0;
}