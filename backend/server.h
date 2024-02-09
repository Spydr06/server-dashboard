#ifndef SERVER_H
#define SERVER_H

#define BACKLOG 10

typedef struct server server_t;

server_t* server_init(int port);
int server_launch(server_t* server);
void server_finish(server_t* server);

#endif /* SERVER_H */

