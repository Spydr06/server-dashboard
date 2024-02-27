#include "server.h"
#include "encoding.h"
#include "mime.h"

#include <asm-generic/socket.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <regex.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define _(str) str
#define REQUEST_BUFFER_SIZE 65536

struct server {
    int fd;
    in_port_t port;

    const char* root;
    struct sockaddr_in addr;
};

enum http_response {
    HTTP_NOT_FOUND,
    HTTP_
};

static void* server_handle_clients(void* attr);
static void handle_request(const server_t* server, int fd);
static void handle_http_get(const server_t* server, int fd, char* request);

server_t* server_init(int port, const char* root) {
    server_t* server = calloc(1, sizeof(struct server));
    assert(server != NULL);

    server->port = port;
    server->root = root;

    if((server->fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Failed to create the socket: %s\n", strerror(errno));
        goto fail;
    }

    int ret;
    if((ret = setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) < 0) {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %s\n", strerror(-ret));
    }

    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = INADDR_ANY;
    server->addr.sin_port = htons(port);

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

pthread_t server_launch(server_t* server) {
    pthread_t thread;

    int ret;
    if((ret = pthread_create(&thread, NULL, server_handle_clients, server)) != 0) {
        fprintf(stderr, "Failed to create server thread: %s\n", strerror(errno));
        return 0;
    }

    return thread;
}

void server_finish(server_t* server, pthread_t thread)
{
    if(thread != 0)
        pthread_cancel(thread);
    close(server->fd);
    free(server);
}

static void* server_handle_clients(void* attr) {
    assert(attr != NULL);
    server_t* server = attr;

    char inet_buf[INET6_ADDRSTRLEN];

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof client_addr;
        
        int client_fd = accept(server->fd, (struct sockaddr *) &client_addr, &client_len);
        if(client_fd < 0) {
            fprintf(stderr, "Failed to accept client: %s\n", strerror(errno));
            continue;
        }

        inet_ntop(
            client_addr.sin_family,
            client_addr.sin_family == AF_INET ? (void*) &(((struct sockaddr_in*) &client_addr)->sin_addr) : (void*) &(((struct sockaddr_in6*) &client_addr)->sin6_addr),
            inet_buf,
            sizeof inet_buf - 1
        );

        fprintf(stdout, "Info: Connection from: %s\n", inet_buf);

        handle_request(server, client_fd);

        close(client_fd);
    }
}

static void handle_request(const server_t* server, int fd) {
    char request[REQUEST_BUFFER_SIZE];

    int bytes_recvd = recv(fd, request, sizeof request - 1, 0);
    if(bytes_recvd <= 0) {
        fprintf(stderr, "Error receiving bytes: %s\n", strerror(-bytes_recvd));
        return;
    }

    if(strncmp(request, "GET", 3) == 0) {
        handle_http_get(server, fd, request);
    }
    else
        fprintf(stderr, "Error: Unhandled request: %s\n", request);
}

static int respond_404(int fd) {
    static const char response[] = _("HTTP/1.1 404 NOT FOUND\r\n\
Content-Type: text/plain\r\n\
\r\n\
404 Not Found");

    int rv = send(fd, response, sizeof response - 1, 0);
    if(rv < 0)
        fprintf(stderr, "Error sending response: %s\n", strerror(-rv));

    return rv;
}

static void handle_http_get(const server_t* server, int fd, char* request) {
    regex_t regex;
    regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
    regmatch_t matches[2];

    if(regexec(&regex, request, 2, matches, 0) != 0) {
        fprintf(stderr, "Error parsing GET request\n");
        return;
    }

    request[matches[1].rm_eo] = '\0';
    const char* url_encoded_filename = request + matches[1].rm_so;

    size_t filepath_size = 1024 + strlen(url_encoded_filename);
    char* filepath = malloc(filepath_size * sizeof(char));

    memcpy(filepath, server->root, strlen(server->root) + 1);
    strcat(filepath, "/");
    url_decode(filepath + strlen(filepath), url_encoded_filename);

retry_open:
    ;

    int file_fd = open(filepath, O_RDONLY);
    if(file_fd == -1) {
        respond_404(fd);
        goto finish;
    }

    struct stat file_stat;
    fstat(file_fd, &file_stat);

    if(S_ISDIR(file_stat.st_mode))
    {
        close(file_fd);
        strncat(filepath, "/index.html", filepath_size);
        goto retry_open;
    }

    const char* mime_type = get_mime_type(filepath);
    
    size_t response_size = 1024 + file_stat.st_size;
    char* response = calloc(response_size, sizeof(char));
    snprintf(
        response,
        response_size,
        "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n",
        mime_type
    );

    size_t header_size = strlen(response);
    size_t bytes_read = read(file_fd, response + header_size, response_size - header_size);

    send(fd, response, header_size + bytes_read, 0);

    close(fd);
    free(response);
finish:
    regfree(&regex);
    free(filepath);
}

