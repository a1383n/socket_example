#include <stdio.h>
#include <stdlib.h>		// exit()
#include <signal.h>		// signal()
#include <string.h>

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
    exit(0);
}

int main() {
    printf("PID: %d\n", getpid());
    fflush(stdout);

    signal(SIGTERM, terminate);
//    signal(SIGKILL, terminate);
    signal(SIGINT, terminate);

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
