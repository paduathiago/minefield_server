#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define TABLE_DIMENSION 4
#define NBOMBS 3

#define START 0
#define REVEAL 1
#define FLAG 2
#define STATE 3
#define REMOVE_FLAG 4
#define RESET 5
#define WIN 6
#define EXIT 7
#define GAME_OVER 8

struct action {
    int type;
    int coordinates[2];
    int board[TABLE_DIMENSION][TABLE_DIMENSION];
};

void parse_addr(const char *addrstr, const char *addrport, struct sockaddr_storage *storage);
char** mount_answer_board(int **board);
void logexit(const char *msg);
void server_sockaddr_init(char *protocol, char * addr, struct sockaddr_storage *storage);
int encode_action(const char *action_str);
size_t receive_all(int socket, void *buffer, size_t size);
void print_board(char **board);

#endif