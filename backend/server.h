#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

#define BACKLOG 10

typedef struct server server_t;

server_t* server_init(int port, const char* root);
pthread_t server_launch(server_t* server);
void server_finish(server_t* server, pthread_t thread);

#endif /* SERVER_H */

