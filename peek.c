#include "headers.h"

void print_permissions(mode_t mode)
{
    printf((mode & S_IFDIR) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

int peek(cmd *input, const char *home)
{
    // printf("hi im in peek with %s\n", input);

    char *input_dup = NULL;

    char *path = NULL;

    int argc = input->argc;

    int a = 0, l = 0;
    int p = 1;

    if (argc <= 0 || input->argv[0] == NULL || (strcmp(input->argv[0], "peek") != 0))
    {
        return 1;
    }
    if (argc > 1)
    {
        if ((strcmp(input->argv[1], "-la") == 0))
        {
            l = 1;
            a = 1;
            p = 2;
        }
        else if ((strcmp(input->argv[1], "-al") == 0))
        {
            l = 1;
            a = 1;
            p = 2;
        }
        else if ((strcmp(input->argv[1], "-l") == 0))
        {
            l = 1;
            p = 2;
        }
        else if ((strcmp(input->argv[1], "-a") == 0))
        {
            a = 1;
            p = 2;
        }
    }
    if (argc > 2)
    {
        if ((strcmp(input->argv[2], "-la") == 0))
        {
            l = 1;
            a = 1;
            p = 3;
        }
        else if ((strcmp(input->argv[2], "-al") == 0))
        {
            l = 1;
            a = 1;
            p = 3;
        }
        else if ((strcmp(input->argv[2], "-l") == 0))
        {
            l = 1;
            p = 3;
        }
        else if ((strcmp(input->argv[2], "-a") == 0))
        {
            a = 1;
            p = 3;
        }
    }

    if (argc == p)
    {
        input_dup = getcwd(NULL, 0);
        if(input_dup == NULL){
            perror(RED"malloc"RESET);
            return 1;
        }
    }
    else
    {
        input_dup = malloc(sizeof(char) * 4096);
        if(input_dup == NULL){
            perror(RED"malloc"RESET);
            return 1;
        }
        strcpy(input_dup, input->argv[p]);
    }

    // struct dirent *file;
    struct dirent **namelist;
    // struct stat file_stat;

    if (input_dup[0] == '~')
    { // 1. starts with ~
        path = malloc((strlen(home) + strlen(input_dup) + 1) * sizeof(char));
        if (path == NULL)
        {
            free(input_dup);
            perror(RED"malloc"RESET);
            return 1;
        }
        strcpy(path, home);
        strcat(path, input_dup + 1);
    }
    else
    {
        path = malloc((strlen(input_dup) + 1) * sizeof(char));
        if (path == NULL)
        {
            free(input_dup);
            perror(RED"malloc"RESET);
            return 1;
        }
        strcpy(path, input_dup);
    }

    int n = scandir(path, &namelist, NULL, alphasort);
    if (n == -1)
    {
        free(input_dup);
        free(path);
        perror(RED"peek"RESET);
        return 1;
    }

    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        free(input_dup);
        free(path);
        perror(RED"peek"RESET);
        return 1;
    }
    int dir = dirfd(dirp);
    if (dir == -1)
    {
        free(input_dup);
        free(path);
        perror(RED"peek"RESET);
        return 1;
    }

    if (l == 1)
    {
        int max_link = 0, max_size = 0, max_user = 0, max_group = 0;
        int blocks = get_total(namelist, n, dir, &max_link, &max_size, &max_user, &max_group, a);
        // printf("ml = %d ms = %d b = %d grp = %d\n", max_link, max_size, blocks, max_group);
        printf("total %d\n", blocks);

        int link = get_digits(max_link);
        int size = get_digits(max_size);

        int i = 0;
        while (i < n)
        {
            if (namelist[i]->d_name[0] == '.' && (a == 0))
            {
                i++;
                continue;
            }
            print_deets(namelist[i], dir, link, size, max_user, max_group);
            free(namelist[i]);
            i++;
        }
    }
    else if (l == 0)
    {
        // char *temp = malloc(sizeof(char) * 4096);
        int i = 0;
        while (i < n)
        {
            if (namelist[i]->d_name[0] == '.' && (a == 0))
            {
                i++;
                continue;
            }

            struct stat file_stat;
            if (fstatat(dir, namelist[i]->d_name, &file_stat, 0) == -1)
            {
                perror(RED"fstat error"RESET);
                i++;
                continue;
            }

            if (S_ISDIR(file_stat.st_mode))
            {
                printf("\033[1;34m%s\n\033[0m", namelist[i]->d_name);
            }
            else if ((file_stat.st_mode & S_IXUSR))
            {
                printf("\033[1;32m%s\n\033[0m", namelist[i]->d_name);
            }
            else
            {
                printf("%s\n", namelist[i]->d_name);
            }
            free(namelist[i]);
            i++;
        }
    }

    closedir(dirp);
    free(path);
    free(input_dup);
    return 0;
}

int get_total(struct dirent **namelist, int n, int dir, int *max_link, int *max_size, int *max_user, int *max_group, int a)
{
    // int max_link = 0, max_size = 0;

    int blocks = 0;

    for (int i = 0; i < n; i++)
    {
        if (namelist[i]->d_name[0] == '.' && (a == 0))
        {
            continue;
        }
        struct stat file_stat;
        if (fstatat(dir, namelist[i]->d_name, &file_stat, 0) == -1)
        {
            perror(RED"fstat error"RESET);
        }

        struct passwd *pw = getpwuid(file_stat.st_uid);
        if (pw == NULL)
        {
            perror(RED"username"RESET);
            return 0;
        }
        struct group *gr = getgrgid(file_stat.st_gid);
        if (gr == NULL)
        {
            perror(RED"group"RESET);
            return 0;
        }
        // printf("%s %d %d\n", gr->gr_name, strlen(gr->gr_name), *max_group);

        if ((int)strlen(pw->pw_name) > *max_user)
        {
            *max_user = strlen(pw->pw_name);
        }
        if ((int)strlen(gr->gr_name) > *max_group)
        {
            *max_group = strlen(gr->gr_name);
        }

        if (file_stat.st_nlink > *max_link)
        {
            *max_link = file_stat.st_nlink;
        }
        if (file_stat.st_size > *max_size)
        {
            *max_size = file_stat.st_size;
        }
        blocks += file_stat.st_blocks;
    }
    return blocks; // / 2;
}

int get_digits(int n)
{
    if (n / 10 == 0)
    {
        return 1;
    }
    return 1 + get_digits(n / 10);
}

void print_deets(struct dirent *file, int dir, int link, int size, int user, int grp)
{
    struct stat file_stat;
    if (fstatat(dir, file->d_name, &file_stat, 0) == -1)
    {
        perror(RED"fstat error"RESET);
    }

    print_permissions(file_stat.st_mode);

    printf(" %*ld ", link, file_stat.st_nlink);

    struct passwd *pw = getpwuid(file_stat.st_uid);
    if (pw == NULL)
    {
        perror(RED"username"RESET);
        return;
    }
    struct group *gr = getgrgid(file_stat.st_gid);
    if (gr == NULL)
    {
        perror(RED"group"RESET);
        return;
    }

    printf("%-*s %-*s ", user, pw->pw_name, grp, gr->gr_name);
    // printf("%s%*s %s%*s ", pw->pw_name, (int)(user - strlen(pw->pw_name)), " ", gr->gr_name, (int)(user - strlen(gr->gr_name)), " ");

    printf("%*lld ", size, (long long)file_stat.st_size);
    // time_t is an integer: the number of seconds elapsed since 00:00:00 on Jan 1, 1970
    // last modified time = file_statst_.mtime
    struct tm* mod_time = localtime(&file_stat.st_mtime);

    char *t = malloc(20 * sizeof(char));
    if (t == NULL)
    {
        perror(RED"malloc"RESET);
        return;
    }
    // strftime(t, 20, "%b %-2d %R", mod_time);


    if (time(NULL) - file_stat.st_mtime < 15778476)
    {
        strftime(t, 20, "%b %-2d %R", mod_time);
    }
    else
    {
        strftime(t, 20, "%b %-2d %-5Y", mod_time);
    }
    printf(" %s ", t);
    free(t);

    if (file_stat.st_mode & S_IFDIR)
    {
        printf("\033[1;34m %s\n\033[0m", file->d_name);
    }
    else if ((file_stat.st_mode & S_IXUSR))
    {
        printf("\033[1;32m %s\n\033[0m", file->d_name);
    }
    else
    {
        printf(" %s\n", file->d_name);
    }
}
