#include "headers.h"

bool inside = false;

void parsehtml(char *html)
{

    for (int i = 0; html[i] != '\0'; i++)
    {
        if (html[i] == '<')
        {
            inside = true;
            continue;
        }

        if (html[i] == '>')
        {
            inside = false;
            continue;
        }

        if (!inside)
        {
            // check for &lt; and &gt;
            if (html[i] == '&' && i + 3 < strlen(html))
            {

                if (html[i + 1] == 'l' && html[i + 2] == 't' && html[i + 3] == ';')
                {
                    putchar('<');
                    i += 3;
                    continue;
                }
                else if (html[i + 1] == 'g' && html[i + 2] == 't' && html[i + 3] == ';')
                {
                    putchar('>');
                    i += 3;
                    continue;
                }
                // &amp;
                else if (i + 4 < strlen(html) && html[i + 1] == 'a' && html[i + 2] == 'm' && html[i + 3] == 'p' && html[i + 4] == ';')
                {
                    putchar('&');
                    i += 4;
                    continue;
                }
                // check for quotes &quot;
                else if (i + 5 < strlen(html) && html[i + 1] == 'q' && html[i + 2] == 'u' && html[i + 3] == 'o' && html[i + 4] == 't' && html[i + 5] == ';')
                {
                    putchar('"');
                    i += 5;
                    continue;
                }
            }

            putchar(html[i]);
        }
    }
}

int iman(cmd* input)
{
    if (input->argc <= 1)
    {
        fprintf(stderr, RED "iMan: too few arguments\n" RESET);
        return 1;
    }
    else if (input->argc > 2)
    {
        fprintf(stderr, RED "iMan: too many arguments\n" RESET);
        return 1;
    }


    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo("man.he.net", "80", &hints, &result) != 0)
    {
        perror(RED"getaddrinfo"RESET);
        return 1;
    }

    int socketfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (socketfd < 0)
    {
        perror(RED"socket"RESET);
        return 1;
    }

    if (connect(socketfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        perror(RED"connect"RESET);
        return 1;
    }

    freeaddrinfo(result);

    char *getreq = malloc(sizeof(char) * 4096);
    sprintf(getreq, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", input->argv[1]);

    if (send(socketfd, getreq, strlen(getreq), 0) < 0)
    {
        perror(RED"send"RESET);
        return 1;
    }

    free(getreq);


    char *response = malloc(sizeof(char) * 4096);
    int i = 0;
    while (1)
    {
        int bytes = recv(socketfd, response, 4096, 0);
        if (bytes < 0)
        {
            perror(RED"recv"RESET);
            return 1;
        }
        else if (bytes == 0)
        {
            break;
        }
        else
        {
            response[bytes] = '\0';
            if (strstr(response, "No matches") != NULL)
            {
                fprintf(stderr, RED "ERROR: No such command\n" RESET);
                free(response);
                close(socketfd);
                return 1;
            }
            if (i == 0)
            {
                char *ptr1 = strstr(response, "NAME");
                char *ptr2 = strstr(ptr1 + 1, "NAME");
                parsehtml(ptr2);
                i = 1;
            }
            else
            {
                parsehtml(response);
            }
        }
    }

    free(response);
    close(socketfd);
    return 0;
}
