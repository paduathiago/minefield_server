#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

#define COMMAND_LEN 12

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
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    if(sockfd == -1) 
        logexit("socket");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(connect(sockfd, addr, sizeof(storage)) != 0)
        logexit("connect");

    while(1)
    {
        struct action action;

        char action_str[COMMAND_LEN];
        if(scanf("%20s", &action_str) != 1)
        {
            printf("error: command not found");
            return 1;
        }
        action.type = encode_action(action_str);

        if(scanf("%d %d", &action.coordinates[0], &action.coordinates[1]) != 2)
        {
            action.coordinates[0] = -1;
            action.coordinates[1] = -1;
        }

        if(send(sockfd, &action, sizeof(action), 0) == -1)
            logexit("send");

    }

    
}