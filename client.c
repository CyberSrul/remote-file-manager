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
#include "protocol.h"

#define BUFLEN 100




int main(int argc, char *argv[])
{
    if (argc < 2) throw("usage: ./client <host name>");


    /* 1. set up */

    int sockfd, numbytes, server_adds_list_status;
    struct addrinfo sock_spesifics, * server_adds_list, * server_add;
    char ip[INET6_ADDRSTRLEN], buf[BUFLEN + 1] = {0};

    // setting up hints, we'll work IPv6 using TCP
    bzero( & sock_spesifics, sizeof sock_spesifics);
    sock_spesifics.ai_family = AF_UNSPEC;
    sock_spesifics.ai_socktype = SOCK_STREAM;



    /* 2. setting up socket and ettempting to connect */

    if ((server_adds_list_status = getaddrinfo(argv[1], PORT, &sock_spesifics, &server_adds_list)) != 0)
    {
        throw(gai_strerror(server_adds_list_status));
    }

    for(server_add = server_adds_list; server_add != NULL; server_add = server_add -> ai_next)
    {
        if ((sockfd = socket(server_add -> ai_family, server_add -> ai_socktype, server_add -> ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, server_add -> ai_addr, server_add -> ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    freeaddrinfo(server_adds_list);

    if (server_add == NULL) throw("could not connect");

    inet_ntop(server_add -> ai_family, & (((struct sockaddr_in6 * ) server_add -> ai_addr) -> sin6_addr), ip, sizeof ip);
    printf("client: connecting to %s\n", ip);



    /* 3. sending a message */

    if ((numbytes = recv(sockfd, buf, BUFLEN, 0)) == -1) throw("did not recive any message");
    printf("client: received '%s'\n", buf);


    close(sockfd);
    return EXIT_SUCCESS;
}