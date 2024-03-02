#include "protocol.h"




int main(int argc, char *argv[])
{
    if (argc < 4) throw("usage: ./client <host name> <request> <path>");


    /* 1. set up */

    int sockfd, get_adds_status;
    struct addrinfo specs, * server_adds_list, server_add;
    char buf[MSGLEN + 1] = {0};



    /* 2. open hot client socket */

    sock_protocol(& specs, CLIENT);

    if ((get_adds_status = getaddrinfo(argv[1], PORT, & specs, & server_adds_list)) != 0) throw(gai_strerror(get_adds_status));

    sockfd = hot_socket(server_adds_list, & server_add, CLIENT);

    freeaddrinfo(server_adds_list);

    print_address((struct sockaddr_in6 * ) server_add.ai_addr, CLIENT);



    /* 3. send request */

    snprintf(buf, sizeof(buf), "%s %s", argv[2], argv[3]);

    if (send(sockfd, buf, MSGLEN, 0) == -1) throw("could not send message");



    close(sockfd);
    return EXIT_SUCCESS;
}