#include <stdio.h>

#include <signal.h>
#include "tcp.h"

static struct socket_in s;

void *handle_conn(struct socket_client* client) {
    for (;;) {
        char buff[4096];
        long l = read(client->fd, &buff, 4096 - 1);
        if (l <= 0) {
            break;
        }

        send(client->fd, &buff, l, 0);
    }

    close(client->fd);

    return NULL;
}

void terminate() {
    printf("Terminating...\n");
    terminate_socket(&s);
}

int main() {
    signal(SIGTERM, terminate);
    signal(SIGKILL, terminate);

    if(create_tcp_socket(&s, INADDR_ANY, 3000, 16) != 0) {
        perror("socket creation failed");
        return 1;
    }

    if (handle_socket(&s, handle_conn) != 0) {
        perror("failed to handling connections");
        return 1;
    }

    terminate();
}
