#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

extern int errno;
int port;

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server;
    char msg[100];
    if (argc != 3)
    {
        printf("Sintax: %s <server_adress> <port>\n", argv[0]);
        return -1;
    }
    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() error.\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("connect() error.\n");
        return errno;
    }
    while(1) {
        bzero(msg, 100);
        fflush(stdout);
        fgets(msg, sizeof(msg), stdin);
        if (strlen(msg) > 0)
            msg[strlen(msg)-1] = '\0';
        if (write(sd, msg, sizeof(msg)) <= 0)
        {
            perror("write() error.\n");
            close(sd);
            return errno;
        }
        if (strcmp(msg,"exit") == 0)
        {
            printf("Exiting...\n");
            close(sd);
            exit(0);
        }
        if (read(sd, msg, sizeof(msg)) < 0)
        {
            perror("read() error.\n");
            close(sd);
            return errno;
        }
        printf("%s\n", msg);
    }
    close(sd);
    return 0;
}