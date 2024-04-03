#include "tcp.h"

static unsigned int _connectedClients = 0;
static struct socket_client *clients[4096];

int create_tcp_socket(struct socket_in *s, int address, int port, int backoff)
{
    int fd, opt = 0, r;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return fd;
    }

    if ((r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) != 0) {
        return r;
    }

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = address;
    sockaddr.sin_port = htons(port);

    if ((r = bind(fd, (const struct sockaddr *) &sockaddr, sizeof(sockaddr))) != 0) {
        return r;
    }

    if ((r = listen(fd, backoff)) != 0) {
        return r;
    }

    s->fd = fd;
    s->opt = opt;
    s->address = sockaddr;
    s->address_size = sizeof(sockaddr);

    return 0;
}

void *internal_handler(void *args)
{
    struct _socket_client* _c = (struct _socket_client*) args;

    connectedClients++;
    ((void (*)(struct socket_client *))_c->handler)(_c->socket_client);
    connectedClients--;

    return NULL;
}

int handle_socket(struct socket_in *s ,void *handle(struct socket_client*))
{
    for (;;) {
        int c_fd;
        if ((c_fd = accept(s->fd, (struct sockaddr *) &s->address, &s->address_size)) < 0) {
            return c_fd;
        }

        pthread_t pthread;

        struct socket_client client;
        client.fd = c_fd;
        client.pthread = &pthread;
        client.server = s;

        struct _socket_client _c;
        _c.socket_client = &client;
        _c.handler = handle;

        clients[_connectedClients++] = &client;

        pthread_create(&pthread, NULL, internal_handler, (void *)&_c);
    }
}

void terminate_socket(struct socket_in* s)
{
    for (int i = 0; i < _connectedClients; ++i) {
        pthread_cancel(*clients[i]->pthread);
        close(clients[i]->fd);
    }

    close(s->fd);
    free(*clients);
    _connectedClients = 0;
}