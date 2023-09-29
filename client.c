#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

#define COMMAND_LEN 12
#define NCOMMANDS 6

int is_input_valid(const struct action action_received, const char *command, const int x, const int y)
{
    char *valid_commands[] = {"start", "reveal", "flag", "remove_flag", "reset", "exit"};
    int is_command_found = 0;
    for(int i = 0; i < NCOMMANDS; i++)
    {
        if(!strcmp(command, valid_commands[i]))
        {
            is_command_found = 1;
            break;
        }
    }
    if(!is_command_found)
    {
        printf("error: command not found");
        return 0;
    }
    if(x < 0 || x >= TABLE_DIMENSION || y < 0 || y >= TABLE_DIMENSION)
    {
        printf("error: invalid cell");
        return 0;
    }
    if(!strcmp(command, "reveal"))
    {
        if(action_received.board[x][y] != -2)
        {
            printf("error: cell already revealed");
            return 0;
        }
    }
    else if(!strcmp(command, "flag"))
    {
        if(action_received.board[x][y] == -3)
        {
            printf("error: cell already has a flag");
            return 0;
        }
        if(action_received.board[x][y] != -2)
        {
            printf("error: cannot insert flag in revealed cell");
            return 0;
        }
        
    }
    else if(!strcmp(command, "remove_flag"))
    {
        if(action_received.board[x][y] != -3)
        {
            printf("error: cell does not have a flag");
            return 0;
        }
    }
    return 1;
}

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

    struct action action_received;
    while(1)
    {
        struct action action_sent;
        action_sent.type = -1;
        
        char action_str[COMMAND_LEN];
        char line[50];
        int is_in_valid = 0;
              
        while(!is_in_valid)  // trap to ensure input is free of errors
        {
            fgets(line, sizeof(line), stdin);
            int entries = sscanf(line, "%s %d,%d", action_str, &action_sent.coordinates[0], &action_sent.coordinates[1]);
            if(entries != 3)
            {
                action_sent.coordinates[0] = -1;
                action_sent.coordinates[1] = -1;
            }
            is_in_valid = is_input_valid(action_received, action_str, action_sent.coordinates[0], action_sent.coordinates[1]);
        }
        action_sent.type == encode_action(action_str);

        size_t count_bytes_sent = send(sockfd, &action_sent, sizeof(struct action), 0);
        if(count_bytes_sent != sizeof(struct action))
            logexit("send");

        int total_bytes_received = receive_all(sockfd, &action_received, sizeof(struct action));
        if(total_bytes_received != sizeof(struct action))
            logexit("receive_all");
    }
}