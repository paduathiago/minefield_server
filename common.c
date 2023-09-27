#include "common.h"

void logexit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
