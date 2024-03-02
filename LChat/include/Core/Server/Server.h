#pragma once

#include <netinet/in.h>
#include "Utils/types.h"

#define MAX_CLIENTS 2

typedef struct {
    i32 port;
    i32 server_fd;
    i32 new_sockets[MAX_CLIENTS];
    i32 num_clients_connected;
    struct sockaddr_in address;
} Server;

#define NewServer           \
    (Server) {              \
        0, 0, {0, 0}, 0, {} \
    }

Server createServer(const i32 portChoose);
void destroyServer(Server);

bool acceptNewConnection(Server* server);
