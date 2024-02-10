#include "protocol.h"




int main(int argc, char *argv[])
{
    if (argc < 4) throw("usage: ./client <host name> <request> <path>");


    /* 1. set up */

    int sockfd, get_adds_status;
    struct addrinfo sock_spesifics, * server_adds_list, server_add;
    char ip[INET6_ADDRSTRLEN], buf[MSGLEN + 1] = {0};

    // We'll work IPv6 using TCP
    bzero( & sock_spesifics, sizeof sock_spesifics);
    sock_spesifics.ai_family = AF_INET6;
    sock_spesifics.ai_socktype = SOCK_STREAM;



    /* 2. setting up socket and ettempting to connect */

    if ((get_adds_status = getaddrinfo(argv[1], PORT, & sock_spesifics, & server_adds_list)) != 0) throw(gai_strerror(get_adds_status));

    sockfd = get_working_socket(server_adds_list, & server_add, CLIENT);

    freeaddrinfo(server_adds_list);

    inet_ntop(server_add.ai_family, & (((struct sockaddr_in6 * ) server_add.ai_addr) -> sin6_addr), ip, sizeof ip);
    printf("client: connecting to %s\n", ip);



    /* 3. sending a message */

    strcat(buf, argv[2]); strcat(buf, " "); strcat(buf, argv[3]);

    if (send(sockfd, buf, MSGLEN, 0) == -1) throw("could not send message");



    close(sockfd);
    return EXIT_SUCCESS;
}