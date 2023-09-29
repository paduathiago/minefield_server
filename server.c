#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
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

void start_new_game(struct action *action_sent, int **current_board)
{
    current_board = init_current_board();
    for (int i = 0; i < TABLE_DIMENSION; i++)
    {
        for (int j = 0; j < TABLE_DIMENSION; j++)
            action_sent->board[i][j] = current_board[i][j];
    }
    action_sent->type = 3;
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

struct action process_action(struct action action_received, const int **answer_board_int, int **current_board, int *revealed)
{
    struct action action_sent;
    if (action_received.type == 0)  // start
        start_new_game(&action_sent, current_board);

    else if (action_received.type == 1)  // reveal
    {
        int revealed = answer_board_int[action_received.coordinates[0]][action_received.coordinates[1]];
    
        if(revealed == -1)
        {
            action_sent.type = 8;  // game over
            revealed = 0;
            for (int i = 0; i < TABLE_DIMENSION; i++)
            {
                for (int j = 0; j < TABLE_DIMENSION; j++)
                    action_sent.board[i][j] = answer_board_int[i][j];
            }
            return action_sent;
        }
        else
        {
            current_board[action_received.coordinates[0]][action_received.coordinates[1]] = revealed;
            action_sent.board[action_received.coordinates[0]][action_received.coordinates[1]] = revealed;
            // treat win case
            revealed++;
            if(revealed == (TABLE_DIMENSION * NBOMBS) - NBOMBS)
            {
                action_sent.type = 6;  // win
                for (int i = 0; i < TABLE_DIMENSION; i++)
                {
                    for (int j = 0; j < TABLE_DIMENSION; j++)
                        action_sent.board[i][j] = answer_board_int[i][j];
                }
                return action_sent;
            }
            
            action_sent.type = 3;
        }  
    }
    else if (action_received.type == 2)  // flag
    {
        current_board[action_received.coordinates[0]][action_received.coordinates[1]] = -3;
        action_sent.board[action_received.coordinates[0]][action_received.coordinates[1]] = -3;
        action_sent.type = 3;
    }
    else if (action_received.type == 4)  // remove_flag
    {
        current_board[action_received.coordinates[0]][action_received.coordinates[1]] = -2;
        action_sent.board[action_received.coordinates[0]][action_received.coordinates[1]] = -2;
        action_sent.type = 3;
    }
    else if (action_received.type == 5)  // reset
    {
        printf("starting new_game\n");
        start_new_game(&action_sent, current_board);
        revealed = 0;
    }

    
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
    int port = atoi(argv[2]);

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

    printf("IP Version: %s\n", ip_version); // REMOVE
    printf("Port: %d\n", port);

    int **current_board;
    int **answer_board_int = mount_board(input_file);
    char **answer_board_char = mount_answer_board(answer_board_int);  // incluir na função abaixo
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

    int revealed = 0;

    while (1)
    {
        int client_sock = accept(sockfd, client_addr, sizeof(client_storage));
        if (client_sock == -1)
            logexit("accept");
        printf("client connected");

        size_t total_bytes_received = receive_all(client_sock, &action_received, sizeof(struct action));
        if (total_bytes_received != sizeof(struct action))
            logexit("receive_all");

        action_sent = process_action(action_received, answer_board_int, current_board, &revealed);
        // current_board = action_sent.board;
    }

    return 0;
}