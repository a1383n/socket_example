#include <stdio.h>
#include <signal.h> // signal()
#include <unistd.h> // getpid()

#include "tcp.h"

static struct sock_tcp_t s;

void *handle_conn(struct sock_tcp_client_t *client) {
    char buff[BUF_SIZE];
    for (;;) {
        long l = read(client->fd, &buff, BUF_SIZE - 1);
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
    printf("PID: %d\n", getpid());
    fflush(stdout);

    signal(SIGTERM, terminate);
    signal(SIGKILL, terminate);

    if (create_tcp_socket(&s, INADDR_ANY, 3000, DEFAULT_BACKLOG) != 0) {
        perror("socket creation failed");
        return 1;
    }

    if (handle_socket(&s, handle_conn) != 0) {
        perror("failed to handling connections");
        return 1;
    }

    terminate();
}
