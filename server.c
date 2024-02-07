#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "protocol.h"




void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


int main(void)
{
    /* 1. setup */

    int server_sock, client_sock, get_adds_status;
    struct addrinfo sock_spesifics, * server_adds_list, * server_add;
    struct sockaddr_storage client_add;
    socklen_t sin_size;
    char ip[INET6_ADDRSTRLEN];
    struct sigaction sa;

    // setting up hints, we'll work IPv6 using TCP and this hosts IP
    bzero(& sock_spesifics, sizeof sock_spesifics);
    sock_spesifics.ai_family = AF_INET6;
    sock_spesifics.ai_socktype = SOCK_STREAM;
    sock_spesifics.ai_flags = AI_PASSIVE;



    /* 2. seting up server socket */

    if ((get_adds_status = getaddrinfo(NULL, PORT, & sock_spesifics, & server_adds_list)) != 0) throw(gai_strerror(get_adds_status));

    // loop through all the results and bind to the first we can
    for(server_add = server_adds_list; server_add != NULL; server_add = server_add -> ai_next)
    {
        if ((server_sock = socket(server_add -> ai_family, server_add -> ai_socktype, server_add -> ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (bind(server_sock, server_add -> ai_addr, server_add -> ai_addrlen) == -1)
        {
            close(server_sock);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(server_adds_list);

    if (server_add == NULL) throw("could not find any proper internet card");


    /* 3. allocating waiting queue */

    if (listen(server_sock, BACKLOG) == -1) throw("could not allocate wating queue");

    // reap all dead processes
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) throw("sigaction");



    /* 4. taking care of clients */

    printf("server: waiting for connections...\n");

    while(1)
    {
        sin_size = sizeof client_add;
        client_sock = accept(server_sock, (struct sockaddr *) & client_add, & sin_size);
        if (client_sock == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(client_add.ss_family, & (((struct sockaddr_in6 *) & client_add) -> sin6_addr), ip, sizeof ip);
        printf("server: got connection from %s\n", ip);


        // Attending to client
        if ( ! fork())
        {
            close(server_sock);

            if (send(client_sock, "Hello, world!", 13, 0) == -1) perror("send");


            /**
             * 
             * This where most of the wotk is done.
             * We need to accept requests.
             * Read and Write to files.
             * Convert to base64 before we send one.
             * Send accept or error.
             * Syncronize processes.
             * And so own.
             * 
             * **/


            close(client_sock);
            exit(EXIT_SUCCESS);
        }

        // parent doesn't need this
        close(client_sock);
    }


    return EXIT_SUCCESS;
}