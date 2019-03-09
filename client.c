#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netinet/in.h>

#include <signal.h>

#include <string.h>

#define PORT 17000

int socketfd;

void handleAbroting() { close(socketfd); putc('\n',stdout); exit(1); }

int main(int argc, char* argv[])
{
    struct sockaddr_in serverAddr;
    char   message[] = {"Hello there!"};

    signal(SIGTSTP, handleAbroting);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0)
    {
        perror("Socket error.\n");
        close(socketfd);
        exit(1);
    }
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(PORT);
    serverAddr.sin_addr.s_addr = (uint32_t)inet_addr("192.168.0.12");

    if(connect(socketfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connection error.\n");
        close(socketfd);
        exit(1);
    }

    int bytesSent = 0;
    int totalBytes = 0;
    while(1)
    {
        bytesSent = send(socketfd, message, sizeof(message), 0);
        totalBytes += bytesSent;
        if(totalBytes == sizeof(message))
        {
            printf("Sent %ld bytes\n", sizeof(message));
            break;
        }
        else if(bytesSent == -1)
        {
            puts("Sending error.\n");
            break;
        }
        
    }

    close(socketfd);

    return 0;
}