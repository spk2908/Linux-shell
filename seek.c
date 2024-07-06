#include "headers.h"

char *search = NULL;
char *target = NULL;
int d = 0;
int f = 0;
int count = 0;
char *firstmatch = NULL;

int isprefix(const char *pre, const char *str)
{
    for (int i = 0; i < strlen(pre); i++)
    {
        if (pre[i] != str[i])
        {
            return 0;
        }
    }
    return 1;
}

int fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    // printf("fpath = %s\n", fpath);
    // char *filename = getname(fpath);
    // if (filename == NULL)
    // {
    //     return 0;
    // }
    char *filename = fpath + ftwbuf->base;
    if (filename[0] == '.')
    {
        return FTW_SKIP_SUBTREE;
    }

    if (strcmp(fpath, target) == 0)
    {
        return FTW_CONTINUE;
    }

    if (isprefix(search, filename) != 0)
    {

        if (typeflag == FTW_F && d != 1)
        {
            if (count == 0)
            {
                strcpy(firstmatch, fpath);
            }
            count++;
            printf(GREEN ".%s\n" RESET, fpath + strlen(target));
        }
        if ((typeflag == FTW_D || typeflag == FTW_DNR) && f != 1)
        {
            if (count == 0)
            {
                strcpy(firstmatch, fpath);
            }
            count++;
            printf(BLUE ".%s\n" RESET, fpath + strlen(target));
        }
    }
    return FTW_CONTINUE;
}

int seek(cmd *input, char *home, char *prev_dir)
{
    d = 0;
    f = 0;
    count = 0;
    if (input->argc <= 0 || strcmp(input->argv[0], "seek") != 0)
    {
        d = 0;
        f = 0;
        count = 0;
        return 1;
    }

    int i = 1;
    // while i < argc fill in flags for -d, -e ,and -f if a - is encountered
    // otherwise store the string in search and target strings

    int e = 0;
    while (i < input->argc)
    {
        if (input->argv[i][0] == '-')
        {
            int j = 1;
            while (input->argv[i][j] != '\0')
            {
                if (input->argv[i][j] == 'd')
                {
                    d = 1;
                }
                else if (input->argv[i][j] == 'e')
                {
                    e = 1;
                }
                else if (input->argv[i][j] == 'f')
                {
                    f = 1;
                }
                else
                {
                    fprintf(stderr, RED"Invalid flags!\n"RESET);
                    d = 0;
                    f = 0;
                    count = 0;
                    return 1;
                }
                j++;
            }
            i++;
        }
        else
        {
            break;
        }
    }

    if (d == 1 && f == 1)
    {
        fprintf(stderr, RED"Invalid flags!\n"RESET);
        d = 0;
        f = 0;
        count = 0;
        return 1;
    }

    // printf("d = %d, e = %d, f = %d\n", d, e, f);

    if (i < input->argc)
    {
        search = malloc(sizeof(char) * (strlen(input->argv[i]) + 3));
        strcpy(search, input->argv[i]);
        i++;
    }
    else
    {
        fprintf(stderr, RED "peek: missing search term\n" RESET);
        d = 0;
        f = 0;
        count = 0;
        return 1;
    }
    if (i < input->argc)
    {
        if (input->argv[i][0] == '~')
        {
            // replace with home
            char *temp = malloc(sizeof(char) * (strlen(home) + strlen(input->argv[i]) + 1));
            strcpy(temp, home);
            strcat(temp, input->argv[i] + 1);
            target = malloc(sizeof(char) * 4096);
            realpath(temp, target);
        }
        else
        {
            target = malloc(sizeof(char) * 4096);
            realpath(input->argv[i], target);
        }

        i++;
    }
    else
    {
        target = getcwd(NULL, 0);
    }

    // printf("target = %s\n", target);
    firstmatch = malloc(sizeof(char) * 4096);
    nftw(target, fn, 20, FTW_ACTIONRETVAL | FTW_PHYS);

    if (count == 0)
    {
        fprintf(stderr, RED"No matches found!\n"RESET);
        d = 0;
        f = 0;
        count = 0;
        free(firstmatch);
        if (target != NULL)
        {
            free(target);
            target = NULL;
        }
        if (search != NULL)
        {
            free(search);
            search = NULL;
        }

        return 0;
    }

    int ret = 0;

    // printf("count = %d\n", count);

    if (e == 1 && count == 1)
    {
        struct stat filestat;
        stat(firstmatch, &filestat);
        if (S_ISDIR(filestat.st_mode))
        {
            // change prev_dir
            char *dir = getcwd(NULL, 0);
            if (dir == NULL)
            {
                perror(RED "malloc" RESET);
                return 1;
            }
            strcpy(prev_dir, dir);
            free(dir);
            // printf("Changing directory to %s\n", firstmatch);
            errno = 0;
            chdir(firstmatch);
            if (errno == EACCES)
            {
                fprintf(stderr, RED"Missing permissions for task!\n"RESET);
                ret = 1;
            }
            else if (errno != 0)
            {
                perror(RED "chdir" RESET);
                ret = 1;
            }
        }

        else
        {
            // print file contents
            FILE *file = fopen(firstmatch, "r");
            if (file == NULL)
            {
                fprintf(stderr, RED"Missing permissions for task!\n"RESET);
                d = 0;
                f = 0;
                count = 0;
                return 1;
            }

            char *line = malloc(sizeof(char) * 4096);
            while (fgets(line, 4096, file) != NULL)
            {
                printf("%s", line);
            }

            fclose(file);
        }
        free(firstmatch);
        if (target != NULL)
        {
            free(target);
            target = NULL;
        }
        if (search != NULL)
        {
            free(search);
            search = NULL;
        }
    }
    return ret;
    count = 0;
    d = 0;
    f = 0;
}
