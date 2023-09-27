#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Error! No IP or Port specified\n");
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    // it is still necessary to make it flexible to ipv6
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) 
        logexit("socket");
    
    if(connect(sockfd, (struct sockaddr *) &ip, sizeof(ip)) == -1)
        logexit("connect");

}