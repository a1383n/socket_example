#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#ifndef SOCKET_EXAMPLE_SOCKET_H
#define SOCKET_EXAMPLE_SOCKET_H

static unsigned int connectedClients = 0;

struct socket_in {
    int fd;
    int opt;
    struct sockaddr_in address;
    socklen_t address_size;
};

struct socket_client {
    int fd;
    struct socket_in *server;
    pthread_t *pthread;
};

struct _socket_client {
    void *handler;
    struct socket_client *socket_client;
};

extern int create_tcp_socket(struct socket_in *s, int address, int port, int backoff);

int handle_socket(struct socket_in *s ,void *handle(struct socket_client*));

void terminate_socket(struct socket_in* s);

#endif //SOCKET_EXAMPLE_SOCKET_H
