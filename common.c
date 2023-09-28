#include <arpa/inet.h>
#include "common.h"

void logexit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


void parse_addr(const char *addrstr, const char *addrport, struct socket_storage *storage)
{
    if(addrport == NULL || addrstr == NULL)
    {
        printf("Error! No IP or Port specified\n");
        exit(1);
    }

    u_int16_t port = (u_int16_t)atoi(addrport);
    if(port == 0)
    {
        printf("Error! Invalid Port\n");
        exit(1);
    }

    port = htons(port);  // host to network short

    struct in_addr inaddr4;
    if(inet_pton(AF_INET, addrstr, &inaddr4))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return;
    }

    struct in6_addr inaddr6;
    if(inet_pton(AF_INET6, addrstr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return;
    }

    else 
    {
        printf("Error! Invalid IP\n");
        exit(1);
    }
}