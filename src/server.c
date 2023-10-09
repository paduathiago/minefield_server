#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

int **init_current_board()
{
    int **board = (int **)malloc(TABLE_DIMENSION * sizeof(int *));
    for (int i = 0; i < TABLE_DIMENSION; i++)
        board[i] = (int *)malloc(TABLE_DIMENSION * sizeof(int));

    for (int i = 0; i < TABLE_DIMENSION; i++)
    {
        for (int j = 0; j < TABLE_DIMENSION; j++)
            board[i][j] = -2;
    }

    return board;
}

void start_new_game(int ***current_board)
{
    (*current_board) = init_current_board();
}

int **mount_board(char *file)
{
    int **board = (int **)malloc(TABLE_DIMENSION * sizeof(int *));
    for (int i = 0; i < TABLE_DIMENSION; i++)
        board[i] = (int *)malloc(TABLE_DIMENSION * sizeof(int));

    FILE *table_input = fopen(file, "r");
    if (table_input == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    for (int i = 0; i < TABLE_DIMENSION; i++)
        for (int j = 0; j < TABLE_DIMENSION; j++)
            fscanf(table_input, "%d,", &board[i][j]);

    fclose(table_input);
    return board;
}

struct action process_client_action(struct action action_received, int **answer_board_int, int ***current_board, int *count_revealed)
{
    struct action action_sent;

    if (action_received.type == START)
    {
        start_new_game(current_board);
        action_sent.type = STATE;
    }
        
    else if (action_received.type == REVEAL)
    {
        int revealed_cell = answer_board_int[action_received.coordinates[0]][action_received.coordinates[1]];
    
        if(revealed_cell == -1)
        {
            action_sent.type = GAME_OVER;
            (*count_revealed) = 0;
            for (int i = 0; i < TABLE_DIMENSION; i++)
            {
                for (int j = 0; j < TABLE_DIMENSION; j++)
                    action_sent.board[i][j] = answer_board_int[i][j];
            }
            return action_sent;
        }
        else
        {
            (*current_board)[action_received.coordinates[0]][action_received.coordinates[1]] = revealed_cell;
            (*count_revealed)++;

            if((*count_revealed) == (TABLE_DIMENSION * TABLE_DIMENSION) - NBOMBS)
            {
                action_sent.type = WIN;
                for (int i = 0; i < TABLE_DIMENSION; i++)
                {
                    for (int j = 0; j < TABLE_DIMENSION; j++)
                        action_sent.board[i][j] = answer_board_int[i][j];
                }
                return action_sent;
            }
            action_sent.type = STATE;
        }  
    }
    else if (action_received.type == FLAG)
    {
        (*current_board)[action_received.coordinates[0]][action_received.coordinates[1]] = -3;
        action_sent.type = STATE;
    }
    else if (action_received.type == REMOVE_FLAG)
    {
        (*current_board)[action_received.coordinates[0]][action_received.coordinates[1]] = -2;
        action_sent.type = STATE;
    }
    else if (action_received.type == RESET)
    {
        printf("starting new_game\n");
        start_new_game(current_board);
        (*count_revealed) = 0;
        action_sent.type = STATE;
    }
    
    for(int i = 0; i < TABLE_DIMENSION; i++)
    {
        for(int j = 0; j < TABLE_DIMENSION; j++)
            action_sent.board[i][j] = (*current_board)[i][j];
    }

    return action_sent;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Error! No IP Version, input file or Port specified\n");
        return 1;
    }

    extern char *optarg;
    char *input_file;

    int opt;

    char *ip_version = argv[1];
    char * port = argv[2];

    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_file = optarg;
            break;

        default:
            printf("Error! No input File specified\n");
            return 1;
        }
    }

    int **current_board = init_current_board();
    int **answer_board_int = mount_board(input_file);
    char **answer_board_char = mount_answer_board(answer_board_int);
    print_board(answer_board_char);

    struct sockaddr_storage storage;

    server_sockaddr_init(ip_version, port, &storage);
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);

    if (bind(sockfd, (struct sockaddr *)&storage, sizeof(storage)) != 0)
        logexit("bind");

    if (listen(sockfd, 1) != 0)
        logexit("listen");

    struct sockaddr_storage client_storage;
    struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
    struct action action_received;
    struct action action_sent;
    socklen_t client_addr_len = sizeof(client_storage);

    int count_revealed = 0;

    while (1)
    {
        int client_sock = accept(sockfd, client_addr, &client_addr_len);
        if (client_sock == -1)
            logexit("accept");
        printf("client connected\n");

        while(1)
        {
            /*size_t total_bytes_received = receive_all(client_sock, &action_received, sizeof(struct action));
            if (total_bytes_received != sizeof(struct action))
            {
                logexit("receive_all");
            }*/
            recv(client_sock, &action_received, sizeof(struct action), 0);

            if(action_received.type == EXIT)
            {
                printf("client disconnected\n");
                close(client_sock);
                break;
            }
            action_sent = process_client_action(action_received, answer_board_int, &current_board, &count_revealed);
            
            size_t count_bytes_sent = send(client_sock, &action_sent, sizeof(struct action), 0);
            if(count_bytes_sent != sizeof(struct action))
                logexit("send");

            if(action_sent.type == WIN || action_sent.type == GAME_OVER)
            {
                printf("client disconnected\n");  // remove
                close(client_sock);
                break;
            }
        }    
    }
    return 0;
}