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
#define MSGLEN 1000




void throw(const char * msg)
{
    fprintf(stderr, "%s \n", msg);
    exit(EXIT_FAILURE);
}


int hot_socket(struct addrinfo * cur_add, struct addrinfo * final_add, const int server_mode)
{
    int (* action)(int, const struct sockaddr *, socklen_t) = server_mode ? bind : connect;
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

    if (server_mode && listen(sockfd, BACKLOG) == -1) throw("could not allocate waiting queue");

    * final_add = * cur_add;

    return sockfd;
}


void print_address(const struct sockaddr_in6 * address, const int server_mode) {

    static char ip[INET6_ADDRSTRLEN];

    bzero(ip, sizeof ip);

    inet_ntop(AF_INET6, &(address -> sin6_addr), ip, INET6_ADDRSTRLEN);
    
    printf("%s %s\n", server_mode ? "got connection from:" : "connected to:",  ip);
}



void sock_protocol(struct addrinfo * specs, const int server_mode)
{
    bzero(specs, sizeof * specs);

    specs -> ai_family = AF_INET6;
    specs -> ai_socktype = SOCK_STREAM;

    if (server_mode) specs -> ai_flags = AI_PASSIVE;
}