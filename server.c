#include <signal.h>
#include <sys/wait.h>
#include "protocol.h"




void attend_client(int client_sock)
{
    char buf[MSGLEN + 1] = {0}, * token;

    if (recv(client_sock, buf, MSGLEN, 0) == -1) throw("could not recive nessage");

    printf("The client's message is: %s \n", buf);

    token = strtok(buf, " ");

    if      (strcmp(token, "GET") == 0)  printf("the client requests to retrive a file \n");
    else if (strcmp(token, "POST") == 0) printf("the client requests to post a file \n");
    else                                 perror("unfamiliar request \n");


    close(client_sock);
    exit(EXIT_SUCCESS);
}


void sigchld_handler(int s)
{
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


void hang_sig_handlers()
{
    struct sigaction sa;

    // reap all dead processes
    sa.sa_handler = sigchld_handler;
    sigemptyset(& sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, & sa, NULL) == -1) throw("could not hang signal handler");
}


int main(int argc, char * argv[])
{
    /* 1. set up */

    int server_sock, client_sock, get_adds_status;
    struct addrinfo specs, * server_adds_list, server_add;
    struct sockaddr_storage client_add;
    socklen_t sin_size = sizeof client_add;

    hang_sig_handlers();



    /* 2. opening hot server socket */

    sock_protocol(& specs, SERVER);

    if ((get_adds_status = getaddrinfo(NULL, PORT, & specs, & server_adds_list)) != 0) throw(gai_strerror(get_adds_status));

    server_sock = hot_socket(server_adds_list, & server_add, SERVER);

    freeaddrinfo(server_adds_list);

    printf("waiting for connections... \n");



    /* 3. taking care of clients */

    while(1)
    {
        if ((client_sock = accept(server_sock, (struct sockaddr *) & client_add, & sin_size)) == -1)
        {
            fflush(stderr);
            continue;
        }

        print_address((struct sockaddr_in6 *) & client_add, SERVER);

        if ( ! fork())
        {
            close(server_sock);
            attend_client(client_sock);
        }

        close(client_sock);
    }


    return EXIT_SUCCESS;
}