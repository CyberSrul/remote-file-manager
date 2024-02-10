#include <signal.h>
#include <sys/wait.h>
#include "protocol.h"




void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


int main(int argc, char * argv[])
{
    /* 1. setup */

    int server_sock, client_sock, get_adds_status;
    struct addrinfo sock_spesifics, * server_adds_list, server_add;
    struct sockaddr_storage client_add;
    socklen_t sin_size = sizeof client_add;
    struct sigaction sa;
    char ip[INET6_ADDRSTRLEN], buf[MSGLEN + 1] = {0}, * token;

    // We'll work IPv6 using TCP and this hosts IP
    bzero(& sock_spesifics, sizeof sock_spesifics);
    sock_spesifics.ai_family = AF_INET6;
    sock_spesifics.ai_socktype = SOCK_STREAM;
    sock_spesifics.ai_flags = AI_PASSIVE;



    /* 2. seting up server socket */

    if ((get_adds_status = getaddrinfo(NULL, PORT, & sock_spesifics, & server_adds_list)) != 0) throw(gai_strerror(get_adds_status));

    server_sock = get_working_socket(server_adds_list, & server_add, SERVER);

    freeaddrinfo(server_adds_list);



    /* 3. Getting ready to handle clients */

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
        if ((client_sock = accept(server_sock, (struct sockaddr *) & client_add, & sin_size)) == -1)
        {
            fflush(stderr);
            continue;
        }

        inet_ntop(client_add.ss_family, & (((struct sockaddr_in6 *) & client_add) -> sin6_addr), ip, sizeof ip);
        printf("server: got connection from %s\n", ip);


        // Attending to client
        if ( ! fork())
        {
            close(server_sock);

            if (recv(client_sock, buf, MSGLEN, 0) == -1) throw("could not recive nessage");

            printf("The client's message is: %s \n", buf);

            token = strtok(buf, " ");

            if      (strcmp(token, "GET") == 0)  printf("the client requests to retrive a file \n");
            else if (strcmp(token, "POST") == 0) printf("the client requests to post a file \n");
            else                                 perror("unfamiliar request \n");


            close(client_sock);
            exit(EXIT_SUCCESS);
        }

        close(client_sock);
    }


    return EXIT_SUCCESS;
}