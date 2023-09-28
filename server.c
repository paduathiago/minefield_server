#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

int** mount_board(char *file)
{
    int** board = (int**)malloc(TABLE_DIMENSION * sizeof(int*));
    for (int i = 0; i < TABLE_DIMENSION; i++) 
        board[i] = (int*)malloc(TABLE_DIMENSION * sizeof(int));
    
    FILE *table_input = fopen(file, "r");
    if (table_input == NULL) 
    {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    for (int i = 0; i < TABLE_DIMENSION; i++)
        for(int j = 0; j < TABLE_DIMENSION; j++)
            fscanf(table_input, "%d,", &board[i][j]);

    fclose(table_input);
    return board;
}


int main(int argc, char *argv[])
{    
    if(argc != 5)
    {
        printf("Error! No IP Version, input file or Port specified\n");
        return 1;
    }

    extern char * optarg;
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
    
    int** board = mount_board(input_file);

    struct sockaddr_storage storage;

    server_sockaddr_init(ip_version, port, &storage);
    int sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    
    if(bind(sockfd, (struct sockaddr *)&storage, sizeof(storage)) != 0)
        logexit("bind");

    if(listen(sockfd, 1) != 0)
        logexit("listen");
    
    struct sockaddr_storage client_storage;
    struct sockaddr * client_addr = (struct sockaddr *) (&client_storage);
    struct action action_received;
    struct action action_sent;
    
    while (1)
    {
        int client_sock = accept(sockfd, client_addr, sizeof(client_storage));
        if(client_sock == -1)
            logexit("accept");
        printf("client connected");

        size_t total_bytes_received = receive_all(client_sock, &action_received, sizeof(struct action));

    }

    return 0;
}