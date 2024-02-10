#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "5000"
#define BACKLOG 20
#define CLIENT 0
#define SERVER 1




void throw(const char * msg)
{
    fprintf(stderr, "%s \n", msg);
    exit(EXIT_FAILURE);
}


int get_working_socket(struct addrinfo * cur_add, struct addrinfo * final_add, const int mode)
{
    int (* action)(int, const struct sockaddr *, socklen_t) = mode ? bind : connect;
    int sockfd;


    for(; cur_add != NULL; cur_add = cur_add -> ai_next)
    {
        if ((sockfd = socket(cur_add -> ai_family, cur_add -> ai_socktype, cur_add -> ai_protocol)) == -1)
        {
            fflush(stderr);
            continue;
        }

        if (action(sockfd, cur_add -> ai_addr, cur_add -> ai_addrlen) == -1)
        {
            close(sockfd);
            fflush(stderr);
            continue;
        }

        break;
    }

    if (! cur_add) throw("Failed to initiate a working socket");
    else * final_add = * cur_add;

    return sockfd;
}