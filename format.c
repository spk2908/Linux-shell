#include "headers.h"

char *format(char *input)
{
    char *output = malloc(sizeof(char) * 2 * strlen(input));
    int inputIndex = 0, outputIndex = 0;
    bool lastWasSpace = false;

    // change all tabs to spaces
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == '\t')
        {
            input[i] = ' ';
        }
    }

    while (input[inputIndex] == ' ' || input[inputIndex] == '\t')
    {
        inputIndex++;
    }

    // Process the input string character by character
    while (input[inputIndex] != '\0')
    {

        if (input[inputIndex] == ' ' || input[inputIndex] == '\t')
        {
            // Skip multiple spaces and tabs
            if (!lastWasSpace)
            {
                output[outputIndex++] = ' ';
                lastWasSpace = true;
            }
        }
        else if (input[inputIndex] == ';' || input[inputIndex] == '&' || input[inputIndex] == '<' || input[inputIndex] == '|')
        {
            char c = input[inputIndex];
            if (lastWasSpace)
            {
                output[outputIndex++] = c;
                output[outputIndex++] = ' ';
            }
            else
            {
                output[outputIndex++] = ' ';
                output[outputIndex++] = c;
                output[outputIndex++] = ' ';
            }
            lastWasSpace = true;
        }
        else if (input[inputIndex] == '>')
        {
            // check if a >> follows
            if (!lastWasSpace)
            {
                output[outputIndex++] = ' ';
            }
            if (inputIndex + 1 < strlen(input) && input[inputIndex + 1] == '>')
            {
                output[outputIndex++] = '>';
                output[outputIndex++] = '>';
                output[outputIndex++] = ' ';
                inputIndex++;
            }
            else
            {
                output[outputIndex++] = '>';
                output[outputIndex++] = ' ';
            }
            lastWasSpace = true;
        }
        else
        {
            output[outputIndex++] = input[inputIndex];
            lastWasSpace = false;
        }
        inputIndex++;
    }

    // Null-terminate the output string
    output[outputIndex] = '\0';


    if (strstr(output, "; ;") != NULL)
    {
        free(output);
        return NULL;
    }
    if (strstr(output, "& &") != NULL)
    {
        free(output);
        return NULL;
    }
    if (strstr(output, "& ;") != NULL)
    {
        free(output);
        return NULL;
    }
    if (strstr(output, "; &") != NULL)
    {
        free(output);
        return NULL;
    }

    // check if string starts with ; or &
    int i = 0;
    while (output[i] == ' ')
    {
        i++;
    }
    if (output[i] == ';' || output[i] == '&')
    {
        free(output);
        return NULL;
    }

    // printf("%s|||\n", output);

    // remove trailing spaces and semicolons
    while (outputIndex - 1 >= 0 && (output[outputIndex - 1] == ' ' || output[outputIndex - 1] == ';'))
    {
        outputIndex--;
    }
    output[outputIndex] = '\0';

    return output;
}
