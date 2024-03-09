
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 2728
extern int errno;
int validare;

int main()
{
    struct sockaddr_in server;
    struct sockaddr_in from;
    fd_set readfds;
    fd_set actfds;
    struct timeval tv;
    int sd, client;
    int optval = 1;
    int fd;
    int nfds;
    int len;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() error.\n");
        return errno;
    }

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("bind() error.\n");
        return errno;
    }

    if (listen(sd, 5) == -1)
    {
        perror("listen() error.\n");
        return errno;
    }

    FD_ZERO(&actfds);
    FD_SET(sd, &actfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    nfds = sd;

    printf("Port is %d\n", PORT);
    fflush(stdout);

    while (1)
    {
        bcopy((char *)&actfds, (char *)&readfds, sizeof(readfds));

        if (select(nfds + 1, &readfds, NULL, NULL, &tv) < 0)
        {
            perror("select() error.\n");
            return errno;
        }

        if (FD_ISSET(sd, &readfds))
        {
            len = sizeof(from);
            bzero(&from, sizeof(from));

            client = accept(sd, (struct sockaddr *)&from, &len);

            if (client < 0)
            {
                perror("accept() error.\n");
                continue;
            }

            if (nfds < client)
                nfds = client;

            FD_SET(client, &actfds);

            printf("[server] S-a conectat clientul cu descriptorul %d.\n", client);
            fflush(stdout);
        }
        for (fd = 0; fd <= nfds; fd++)
        {
            if (fd != sd && FD_ISSET(fd, &readfds))
            {
                int bytes;
                char msg[100];
                char copymsg[101];
                char msgrasp[100] = " ";

                bytes = read(fd, msg, sizeof(msg));
                printf("mesaj %s\n", msg);
                printf("validare = %d\n", validare);
                strcpy(copymsg, msg);
                char *command = strtok(copymsg, " ");
                if (command != NULL)
                {
                    printf("comanda %s\n", command);
                }
                if (bytes < 0)
                {
                    perror("read() error.\n");
                    return errno;
                }
                if (validare == 1)
                {
                    if (strcmp(command, "login") == 0)
                    {
                        strcpy(msgrasp, "Deja sunteti logged in");
                        write(fd, msgrasp, sizeof(msgrasp));
                    }
                    else if (strcmp(msg, "logout") == 0)
                    {
                        strcpy(msgrasp, "Logging out...");
                        write(fd, msgrasp, sizeof(msgrasp));
                        validare = 0;
                    }
                }
                else if (validare == 0)
                {
                    if (strcmp(command, "login") == 0)
                    {
                        strcpy(msgrasp, "Login.");
                        //if(check(msg))
                        validare = 1;
                        write(fd, msgrasp, sizeof(msgrasp));
                    }
                }
                if (strcmp(msg, "exit") == 0)
                {
                    printf("%d client disconnected.\n", fd);
                    fflush(stdout);
                    close(fd);
                    FD_CLR(fd, &actfds);
                }
                else if (strcmp(command, "login") == 0 && validare == 0)
                {
                    strcpy(msgrasp, "Login.");
                    validare = 1;
                    write(fd, msgrasp, sizeof(msgrasp));
                }
                else
                {
                    printf("Comanda necunoscuta.\n");
                    write(fd, "Comanda necunoscuta.", 20);
                }
            }
        }
    }
}
