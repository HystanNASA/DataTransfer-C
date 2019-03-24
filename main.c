#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define PORT 1710

int     sockfd, mainSock, listenerSock;
int     file;
char    filename[256];

void clientRoutine();
void serverRoutine();
void handleSignal();

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Must be 2 args\n");
        exit(1);
    }

    signal(SIGABRT, handleSignal);

    strcpy(filename, argv[2]);
    if(strcmp(argv[1], "-s") == 0)
    {
        serverRoutine();
    }
    else if(strcmp(argv[1], "-c") == 0) 
    {
        clientRoutine();
    }
    else
    {
        fprintf(stderr, "Must be the -s or -c argument\n");
    }

    return 0;
}

void clientRoutine()
{
    struct sockaddr_in  addr;
    char                data[1024];

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("192.168.0.12");
    memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        fprintf(stderr, "Socket failed\n");
        goto terminate;
    }

    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "Connection failed\n");
        goto terminate;
    }

    int totalBytesWritten = 0;

    if(file = fopen(filename, "rb") < 0)
    {
        fprintf(stderr, "Opening file failed\n");
        goto terminate;
    }

    /* Communicating with server */
    write(sockfd, "ok", sizeof("ok"));

terminate:
    close(sockfd);
    close(file);
}

void serverRoutine()
{
    struct sockaddr_in  addr;
    char                filename[256];
    char                data[1024];
    const int           trueFlag = 1;

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));

    mainSock = socket(AF_INET, SOCK_STREAM, 0);
    if(mainSock < 0)
    {
        fprintf(stderr, "Scoket failed\n");
        goto terminate;
    }
    
    if(setsockopt(mainSock, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
    {
        fprintf(stderr, "setsockopt failed\nErrno: %d", errno);
        goto terminate;
    }

    if(bind(mainSock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "Bindning failed\nErrno: %d\n", errno);
        goto terminate;
    }

    if(listen(mainSock, 5) < 0)
    {
        fprintf(stderr, "Listening failed\n");
        goto terminate;
    }

    listenerSock = accept(mainSock, NULL, NULL);

    /* Communicating with client */
    char buff[16];
    read(listenerSock, buff, sizeof(buff));

    printf("%s\n", buff);

terminate:
    close(mainSock);
    close(listenerSock);
    close(file);
}

void handleSignal()
{
    close(sockfd);
    close(mainSock);
    close(listenerSock);
    close(file);
}