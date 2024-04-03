#include "tcp.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h> // memset()
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <malloc.h>

int create_tcp_socket(struct sock_tcp_t *s, int address, int port, int backlog) {
    int fd, opt = 1;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0) {
        perror("setsockopt failed");
        close(fd);
        return -1;
    }

    memset(&s->addr, 0, sizeof(s->addr));
    s->addr.sin_family = AF_INET;
    s->addr.sin_addr.s_addr = htonl(address);
    s->addr.sin_port = htons(port);
    s->addr_len = sizeof(s->addr);

    if (bind(fd, (const struct sockaddr *) &s->addr, sizeof(s->addr)) != 0) {
        perror("bind failed");
        close(fd);
        return -1;
    }

    if (listen(fd, backlog) != 0) {
        perror("listen failed");
        close(fd);
        return -1;
    }

    s->fd = fd;
    s->opt = opt;

    return 0;
}

void *internal_handler(void *args) {
    struct _sock_tcp_client_t *c = (struct _sock_tcp_client_t *) args;

    (c->handler)(c->socket_client);

    close(c->socket_client->fd);
    free(c->socket_client);

    return NULL;
}

int handle_socket(struct sock_tcp_t *s, void *(*handle)(struct sock_tcp_client_t *)) {
    int epoll_fd, nfds, c_fd;
    struct epoll_event ev, events[MAX_CLIENTS];

    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("epoll creation failed");
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = s->fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s->fd, &ev) == -1) {
        perror("epoll_ctl failed");
        close(epoll_fd);
        return -1;
    }

    for (;;) {
        nfds = epoll_wait(epoll_fd, events, MAX_CLIENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR) {
                // Interrupted system call, continue waiting
                continue;
            } else {
                perror("epoll_wait failed");
                close(epoll_fd);
                return -1;
            }
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == s->fd) {
                if ((c_fd = accept(s->fd, (struct sockaddr *) &s->addr, &s->addr_len)) < 0) {
                    perror("accept failed");
                    close(epoll_fd);
                    return -1;
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = c_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c_fd, &ev) == -1) {
                    perror("epoll_ctl for client socket failed");
                    close(c_fd);
                    close(epoll_fd);
                    return -1;
                }
            } else {
                // malloc to avoid point to same location as last client
                struct sock_tcp_client_t *client = malloc(sizeof(struct sock_tcp_client_t));
                struct _sock_tcp_client_t _client;
                pthread_t pthread;
                client->fd = events[n].data.fd;

                _client.socket_client = client;
                _client.handler = handle;

                pthread_create(&pthread, NULL, internal_handler, (void *) &_client);
                pthread_detach(pthread);
            }
        }
    }

    close(epoll_fd);
    return 0;
}

void terminate_socket(struct sock_tcp_t *s) {
    close(s->fd);
}
