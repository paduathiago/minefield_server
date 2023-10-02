#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

#define COMMAND_LEN 12
#define NCOMMANDS 6

int is_input_valid(const struct action action_received, const char *command, const int x, const int y)
{
    char *valid_commands[] = {"start", "reset", "exit"};
    char *valid_with_coordinates[] = {"reveal", "flag", "remove_flag"};
    int is_command_found = 0;

    for(int i = 0; i < NCOMMANDS - 2; i++)
    {
        if(!strcmp(command, valid_with_coordinates[i]))
        {
            is_command_found = 1;
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
            break;
        }
        else if(!strcmp(command, valid_commands[i]))
        {
            is_command_found = 1;
            if(x != -1 || y != -1)
            {
                printf("error: command does not take coordinates");
                return 0;
            }
            break;
        }
    }

    if(!is_command_found)
    {
        printf("error: command not found");
        return 0;
    }
    
    return 1;
}

char **decorate_board(int **board)
{
    char **decorated_board = (char **)malloc(TABLE_DIMENSION * sizeof(char *));
    for (int i = 0; i < TABLE_DIMENSION; i++)
        decorated_board[i] = (char *)malloc(TABLE_DIMENSION * sizeof(char));

    for (int i = 0; i < TABLE_DIMENSION; i++)
    {
        for (int j = 0; j < TABLE_DIMENSION; j++)
        {
            if (board[i][j] == -2)
                decorated_board[i][j] = '-';

            else if (board[i][j] == -3)
                decorated_board[i][j] = '>';
            else
                sprintf(&decorated_board[i][j], "%d", board[i][j]);
        }
    }
    return decorated_board;
}

void process_server_action(struct action action_received)
{
    if(action_received.type == 3)  // state
    {
        char **decorated_board = decorate_board((int **)action_received.board);
        print_board(decorated_board);
        return;
    }
    else if(action_received.type == 6)  // win
        printf("YOU WIN!\n");
        
    else if(action_received.type == 8)  // game over
        printf("GAME OVER!\n");
    
    char **answer_board_char = mount_answer_board((int**)action_received.board);
    print_board(answer_board_char);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Error! No IP or Port specified\n");
        return 1;
    }

    struct sockaddr_storage storage;
    parse_addr(argv[1], argv[2], &storage);

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
        action_sent.type = encode_action(action_str);
        action_sent.board[TABLE_DIMENSION][TABLE_DIMENSION] = action_received.board[TABLE_DIMENSION][TABLE_DIMENSION];

        size_t count_bytes_sent = send(sockfd, &action_sent, sizeof(struct action), 0);
        if(count_bytes_sent != sizeof(struct action))
            logexit("send");
        
        if(action_sent.type == 7)
        {
            close(sockfd);
            break;
        }

        int total_bytes_received = receive_all(sockfd, &action_received, sizeof(struct action));
        if(total_bytes_received != sizeof(struct action))
            logexit("receive_all");
        
        process_server_action(action_received);
        if(action_received.type == 6 || action_received.type == 8)
        {
            close(sockfd);
            break;
        }
    }
}