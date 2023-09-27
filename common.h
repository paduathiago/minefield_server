#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#define TABLE_DIMENSION 4

struct action {
    int type;
    int coordinates[2];
    int board[4][4];
};
