#include "headers.h"

int redirect(char *command, cmd *new, int bg, char *home, char *prev_dir, list proc_list)
{
    // return;
    /*
    parse till you hit a >, >>, or <
    right! file mode. for > open in write mode, for >> open in append mode, for < open in read mode

    */
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    int input_fd, output_fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // 0644 permissions

    // find first instance of <, > and >>
    char *write_out = NULL;
    char *append_out = strstr(command, ">>");
    if (append_out == NULL)
    {
        write_out = strchr(command, '>');
    }

    char *read = strchr(command, '<');
    char *write;
    int flags = 0;


    if (write_out != NULL && append_out == NULL)
    {
        write = write_out;
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else if (append_out != NULL && write_out == NULL)
    {
        write = append_out;
        flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    else
    {
        write = NULL;
    }

    // if read and write are not null, then check which comes first
    // if write comes first, then check if it is > or >>
    // get the files following write/read symbols
    if (read != NULL)
    {
        char *input_file = malloc(4096 * sizeof(char));

        // scan input file into input_file
        int in_i = read - command;
        int i = 0;
        while (command[in_i] == ' ' || command[in_i] == '<')
        {
            in_i++;
        }
        while (command[in_i] != ' ' && command[in_i] != '\0')
        {
            input_file[i] = command[in_i];
            i++;
            in_i++;
        }
        input_file[i] = '\0';

        if (write != NULL)
        {
            // printf("read and write\n");
            // read and write
            char *output_file = malloc(4096 * sizeof(char));

            // scan output file into output_file
            int out_i = write - command;
            i = 0;
            while (command[out_i] == ' ' || command[out_i] == '>')
            {
                out_i++;
            }
            while (command[out_i] != ' ' && command[out_i] != '\0')
            {
                output_file[i] = command[out_i];
                i++;
                out_i++;
            }
            output_file[i] = '\0';

            // printf("input_file = [%s]\n", input_file);
            // printf("output_file = [%s]\n", output_file);

            input_fd = open(input_file, O_RDONLY);
            output_fd = open(output_file, flags, mode);

            if (input_fd == -1)
            {
                perror(RED "no such input file found" RESET);
                return 1;
            }
            if (output_fd == -1)
            {
                perror(RED "error opening output file" RESET);
                return 1;
            }

            if (dup2(input_fd, STDIN_FILENO) == -1)
            {
                perror(RED"Error redirecting input"RESET);
                return 1;
            }
            fflush(stdout);
            if (dup2(output_fd, STDOUT_FILENO) == -1)
            {
                perror(RED "could not change output file stream" RESET);
                return 1;
            }

            close(input_fd);
            close(output_fd);
            free(input_file);
            free(output_file);

            char *new_command = malloc(4096 * sizeof(char));
            strcpy(new_command, "\0");
            i = 0;
            while (command[i] != '<' && command[i] != '>' && command[i] != '\0')
            {
                new_command[i] = command[i];
                i++;
            }
            new_command[i] = '\0';

            int ret = parse_cmd(new_command, original_stdin, original_stdout, new, bg, home, prev_dir, proc_list);
            free(new_command);
            dup2(original_stdin, STDIN_FILENO);
            fflush(stdout);
            dup2(original_stdout, STDOUT_FILENO);
            close(original_stdin);
            close(original_stdout);
            return ret;
        }
        else
        {
            // only read
            close(original_stdout);
            input_fd = open(input_file, O_RDONLY);
            if (input_fd == -1)
            {
                perror(RED "no such input file found" RESET);
                return 1;
            }
            if (dup2(input_fd, STDIN_FILENO) == -1)
            {
                perror(RED"Error redirecting input"RESET);
                return 1;
            }
            close(input_fd);
            free(input_file);
            char *new_command = malloc(4096 * sizeof(char));
            strcpy(new_command, "\0");
            for (int i = 0; i < read - command; i++)
            {
                new_command[i] = command[i];
            }
            new_command[read - command] = '\0';
            int ret = parse_cmd(new_command, original_stdin, -1, new, bg, home, prev_dir, proc_list);
            free(new_command);
            dup2(original_stdin, STDIN_FILENO);
            close(original_stdin);
            return ret;
        }
    }
    else if (write != NULL)
    {
        close(original_stdin);
        // printf("write\n");
        // only write
        char *output_file = malloc(4096 * sizeof(char));
        int out_i = write - command;
        int i = 0;
        while (command[out_i] == ' ' || command[out_i] == '>')
        {
            out_i++;
        }
        while (command[out_i] != ' ' && command[out_i] != '\0')
        {
            output_file[i] = command[out_i];
            i++;
            out_i++;
        }
        output_file[i] = '\0';

        if (write_out && write == write_out)
        {
            // open file in write mode
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, mode);
        }
        else
        {
            // open file in append mode
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, mode);
        }
        fflush(stdout);
        if (dup2(output_fd, STDOUT_FILENO) == -1)
        {
            perror(RED "could not change output file stream" RESET);
            return 1;
        }
        close(output_fd);
        free(output_file);

        char *new_command = malloc(4096 * sizeof(char));
        strcpy(new_command, "\0");
        for (int i = 0; i < write - command; i++)
        {
            new_command[i] = command[i];
        }
        new_command[write - command] = '\0';
        // fprintf(stderr, "new_command = [%s]\n", new_command);

        int ret = parse_cmd(new_command, -1, original_stdout, new, bg, home, prev_dir, proc_list);
        fflush(stdout);
        free(new_command);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        return ret;
    }
    else
    {
        // no redirection
        // fprintf(stderr, "no redirection\n");
        close(original_stdin);
        close(original_stdout);
        return parse_cmd(command, -1, -1, new, bg, home, prev_dir, proc_list);
    }
}
