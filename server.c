#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netinet/in.h>

#include <signal.h>

#define PORT 17000

int socketfd, listener;

void handleAbroting() { close(listener); close(socketfd); putc('\n',stdout); exit(1); }

int main(int argc, char* argv[])
{
    int     socketfd, listener;
    struct  sockaddr_in clientAddr;
    char    buff[128] = {'\0'};

    signal(SIGTSTP, handleAbroting);

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 1)
    {
        perror("Socket error.\n");
        close(socketfd);
        exit(1);
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family        = AF_INET;
    clientAddr.sin_port          = htons(PORT);
    clientAddr.sin_addr.s_addr   = htonl(INADDR_ANY);

    if(bind(listener, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) < 0)
    {
        perror("Bind error.\n");
        close(socketfd);
        exit(1);
    }

    listen(listener, 1);

    if((socketfd = accept(listener, NULL, NULL)) < 0)
    {
        perror("Accpet error.\n");
        close(socketfd);
        exit(1);
    }

    int bytesRead = 0;
    int totalBytes = 0;
    while(1)
    {
        bytesRead = recv(socketfd, buff, sizeof(buff), 0);
        totalBytes += bytesRead;
        if(bytesRead <= 0)
        {
            printf("Received %d bytes\n", totalBytes);
            break;
        }
    }

    printf("%s\n", buff);

    close(socketfd);

    return 0;
}