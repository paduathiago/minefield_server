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

int encode_action(const char *action_str)  // it may be possible that server actions dont need to be encoded
{
    if(!strcmp(action_str, "start"))
        return 0;
    else if(!strcmp(action_str, "reveal"))
        return 1;
    else if(!strcmp(action_str, "flag"))
        return 2;
    else if(!strcmp(action_str, "state"))
        return 3;
    else if(!strcmp(action_str, "remove_flag"))
        return 4;
    else if(!strcmp(action_str, "reset"))
        return 5;
    else if(!strcmp(action_str, "win"))
        return 6;
    else if(!strcmp(action_str, "exit"))
        return 7;
    else if (!strcmp(action_str, "game_over"))
        return 8;
    else
    {
        printf("error: command not found");
        return -1;
    }
}

ssize_t receive_all(int socket, void *buffer, size_t size) {
    char *ptr = (char *)buffer;
    ssize_t total_received = 0;

    while (total_received < size) {
        ssize_t bytes_received = recv(socket, ptr + total_received, size - total_received, 0);

        if (bytes_received <= 0) {
            if (bytes_received == 0)  // Connection closed by peer
                return total_received;
            else 
            {
                perror("Error ao receber dados");
                return -1;
            }
        }

        total_received += bytes_received;
    }

    return total_received;
}
