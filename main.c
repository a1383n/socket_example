#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

struct conn_t {
    int *fd;
    pthread_t *pthread;
};

static int fd, opt;

void *handle_conn(void *args) {
    struct conn_t conn = *(struct conn_t*)args;

    char buff[4096];
    for (;;) {
        long l = read(*conn.fd, &buff, 4096 - 1);
        if (l <= 0) {
            break;
        }

        send(*conn.fd, &buff, l, 0);
    }

    close(*conn.fd);

    return NULL;
}

void terminate() {
    printf("Terminating...");
    close(fd);
    pthread_exit(NULL);
}

int main() {
    signal(SIGTERM, terminate);
    signal(SIGKILL, terminate);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        return 1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0) {
        perror("setsockopt failed");
        return 1;
    }

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(3000);

    if (bind(fd, (const struct sockaddr *) &sockaddr, sizeof(sockaddr)) != 0) {
        perror("bind failed");
        return 1;
    }

    if (listen(fd, 10) != 0) {
        perror("listen failed");
        return 1;
    }

    socklen_t socklen = sizeof(sockaddr);
    int t_c = 0;
    struct conn_t *conn[32];

    for (;;) {
        int c_fd;
        if ((c_fd = accept(fd, (struct sockaddr *) &sockaddr, &socklen)) < 0) {
            perror("accept failed");
            break;
        }

        pthread_t pthread;

        struct conn_t c;
        c.fd = &c_fd;
        c.pthread = &pthread;

        conn[++t_c] = &c;

        pthread_create(&pthread, NULL, handle_conn, (void *)&c);

        printf("Thread ID: %lu, Socket: %d\n", pthread, c_fd);
        fflush(stdout);
    }

    terminate();
}
