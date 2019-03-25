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

#define PORT        2730
#define FILE_SIZE   256

int     sockfd, mainSock, listeningSock, writingSock;
FILE*   file;
char    filename[FILE_SIZE];

void clientRoutine();
void serverRoutine();
void handleSignal();
int copy(char*, char*, size_t); // returns 1 if '\0' is found, otherwise, returns 0

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
    char                data[1024]          = {'\0'};
    ssize_t             totalBytesWritten   = 0;

    addr.sin_family                         = AF_INET;
    addr.sin_port                           = htons(PORT);
    addr.sin_addr.s_addr                    = inet_addr("192.168.0.12");

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

    file = fopen(filename, "rb");
    if(file == NULL)
    {
        fprintf(stderr, "Opening file failed\n");
        goto terminate;
    }

    /* Write filename then data */
    ssize_t bytes = 0;
    while(totalBytesWritten != strlen(filename) + 1)
    {
        bytes = write(sockfd, filename, strlen(filename) + 1);
        if(bytes == -1)
            break;
        totalBytesWritten += bytes;
    }

    totalBytesWritten = 0;              // clear the variable to use it again
    bytes = 0;                          // ditto

    while(fgets(data, sizeof(data), file) != NULL)
    {
        while(totalBytesWritten != strlen(data))
        {
            bytes = write(sockfd, data, strlen(data));
            if(bytes == -1)
                break;
            totalBytesWritten += bytes;
        }
        memset(data, 0, sizeof(data));
    }
    write(sockfd, '\0', 1); // make sure the file ends

    printf("DONE!\n");

terminate:
    close(sockfd);
    close(file);
}

void serverRoutine()
{
    struct sockaddr_in  addr;
    char                data[1024]  = {'\0'};
    const int           trueFlag    = 1;

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
    listeningSock = accept(mainSock, NULL, NULL);
    
    ssize_t bytes = 0;
    int i = 0;
    while(bytes = read(listeningSock, data, sizeof(data)) > 0)
    {
        if(bytes == -1)
            break;
        if(copy(filename + i, data, FILE_SIZE - i))
            break;
        i += bytes;
    }

    file = fopen(filename, "wb");
    if(file == NULL)
    {
        fprintf(stderr, "Creating file failed\n");
        goto terminate;
    }

    ssize_t totalBytesWritten = 0;
    bytes = 0;
    while(bytes = read(listeningSock, data, sizeof(data)) > 0)
    {
        while(totalBytesWritten != strlen(data))
        {
            bytes = fprintf(file, "%s", data);
            if(bytes == -1)
                break;
            totalBytesWritten += bytes;
        }
    }

    printf("DONE!\n");

terminate:
    close(mainSock);
    close(listeningSock);
    close(file);
}

int copy(char* dst, char* src, size_t len) // TODO: Optimization
{
    int i;
    for(i = 0; (src[i] != '\0') && (i < len); i++) dst[i] = src[i];
    if(src[i] == '\0') return 1;
    return 0;
}

void handleSignal()
{
    close(sockfd);
    close(mainSock);
    close(listeningSock);
    close(file);
}