#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#define TABLE_DIMENSION 4
#define NBOMBS 3

struct action {
    int type;
    int coordinates[2];
    int board[TABLE_DIMENSION][TABLE_DIMENSION];
};
