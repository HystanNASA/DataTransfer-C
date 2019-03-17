#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

const char* HOSTIP;
int PORT = 17000;
char FILENAME[256];

void sendall(const int, char*, int, const int);
int clientRoutine();

int main(int argc, char const *argv[])
{
    int retValue;

    if(argc < 3)
    {
        perror("too few args");
        return 1;
    }

    HOSTIP = argv[1];
    strcpy(FILENAME, argv[2]);

    retValue = clientRoutine();

    return 0;
}

int clientRoutine()
{
    int                 sockfd;
    struct sockaddr_in  addr;
    FILE*               file;
    char                buffer[1024];
    int                 localerrno = 0;

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(HOSTIP);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket error.");
        localerrno = 1;
        goto terminate;
    }
    printf("after socket\n");

    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("connect error.");
        localerrno = 1;
        goto terminate;
    }
    printf("after connect\n");

    sendall(sockfd, FILENAME, strlen(FILENAME), 0);
    recv(sockfd, NULL, 10, 0);

    file = fopen(FILENAME, "rb");
    if(file == NULL)
        goto terminate;

    printf("after opening file\n");
    char c = 1;
    while(c != EOF)
    {   
        for(int i = 0; ((c = fgetc(file)) != EOF) && (i < 1024); i++)
            buffer[i] = c;
        sendall(sockfd, buffer, strlen(buffer), 0);
        printf("reading file");
    }
    fclose(file);

terminate:
    close(sockfd);
    return localerrno;
}

void sendall(const int sockfd, char* buf, int len, const int flags)
{
    while(1)
    {
        int i = send(sockfd, buf + i, len, flags);
        if(i < 1)   break;
        len -= i;
        buf += i;
    }
}