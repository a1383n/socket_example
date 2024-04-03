#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#ifndef SOCKET_EXAMPLE_SOCKET_H
#define SOCKET_EXAMPLE_SOCKET_H

struct sock_tcp_t {
    int fd;
    int opt;
    struct sockaddr_in addr;
    socklen_t addr_len;
};

struct sock_tcp_client_t {
    int fd;
    struct socket_in *server;
    pthread_t *pthread;
};

struct _sock_tcp_client_t {
    void *handler;
    struct sock_tcp_client_t *socket_client;
};

extern int create_tcp_socket(struct sock_tcp_t *s, int address, int port, int backlog);

extern int handle_socket(struct sock_tcp_t *s, void *handle(struct sock_tcp_client_t *));

extern void terminate_socket(struct sock_tcp_t *s);

#endif //SOCKET_EXAMPLE_SOCKET_H
