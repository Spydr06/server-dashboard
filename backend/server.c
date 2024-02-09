#include "server.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define _(str) str

struct server {
    int fd;
    in_port_t port;

    struct sockaddr_in addr;
};

server_t* server_init(int port) {
    server_t* server = calloc(1, sizeof(struct server));
    assert(server != NULL);

    server->port = port;

    if((server->fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Failed to create the socket: %s\n", strerror(errno));
        goto fail;
    }

    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = INADDR_ANY;
    server->addr.sin_port = htons(port);

    int ret;
    if((ret = bind(server->fd, (struct sockaddr *) &server->addr, sizeof server->addr)) < 0) {
        fprintf(stderr, "Failed to bind the socket: %s\n", strerror(-ret));
        goto fail;
    }

    if((ret = listen(server->fd, BACKLOG)) < 0) {
        fprintf(stderr, "Failed to listen on the socket: %s\n", strerror(-ret));
        goto fail;
    }

    return server;
fail:
    free(server);
    return NULL;
}

#define BUFFER_SIZE 1024

int server_launch(server_t* server) {
    sigset_t pending, mask, original;
    sigemptyset(&pending);
    sigemptyset(&mask);

    sigaddset(&mask, SIGINT);
    if(sigprocmask(SIG_BLOCK, &mask, &original) == -1) {
        fprintf(stderr, "Error blocking signal SIGINT: %s\n", strerror(errno));
        return -1;
    }

    char buffer[BUFFER_SIZE];

    char *dummy_html = _("HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n\
<!DOCTYPE html>\
<html lang=\"en\">\
    <head><title>Hello World</title></head>\
    <body>\
        <h1>Hello World!</h1>\
    </body>\
</html>");

    while(1) {
        if(sigpending(&pending) == -1) {
            fprintf(stderr, "Error getting pending signals: %s\n", strerror(errno));
            return -1;
        }

        if(sigismember(&pending, SIGINT))
            break;

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof client_addr;
        
        int client_fd = accept(server->fd, (struct sockaddr *) &client_addr, &client_len);
        if(client_fd < 0) {
            fprintf(stderr, "Failed to accept client: %s\n", strerror(errno));
            continue;
        }

        memset(buffer, 0, sizeof buffer);
        read(client_fd, buffer, sizeof buffer - 1);

        printf("%s\n", buffer);

        write(client_fd, dummy_html, strlen(dummy_html));

        close(client_fd);
    }

    if(sigprocmask(SIG_SETMASK, &original, NULL) == -1) {
        fprintf(stderr, "Error restoring signal mask: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

void server_finish(server_t* server)
{
    close(server->fd);
    free(server);
}

