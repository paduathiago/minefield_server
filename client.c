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

    struct sockaddr_storage storage;
    if (parse_addr(argv[1], argv[2], &storage) != 0)
        logexit("parse_addr");

    // it is still necessary to make it flexible to ipv6
    int sockfd = socket(, SOCK_STREAM, 0);
    if(sockfd == -1) 
        logexit("socket");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(connect(sockfd, addr, sizeof(storage)) != 0)
        logexit("connect");

    
    
}