#pragma once

#include <netinet/in.h>
#include "Core/MemoriaCompartida.h"
#include "Utils/types.h"

#define InfoNewMessage \
    (InfoMensaje) {    \
        0, 0           \
    }

typedef struct {
    u32 id_cliente;
    u32 longitud;
} InfoMensaje;

#define MAX_CLIENTS 2

typedef struct {
    i32 port;
    i32 server_fd;
    i32 new_sockets[MAX_CLIENTS];
    i32 num_clients_connected;
    struct sockaddr_in address;
    ShareMemory memory;
    fd_set read_fds;
    u32 max_sd;
} Server;

#define NewServer                             \
    (Server) {                                \
        0, 0, {0, 0}, 0, {}, NewMemory, {}, 0 \
    }

Server createServer(const i32 portChoose);
void destroyServer(Server* server);
bool acceptNewConnection(Server* server);
InfoMensaje updateServer(Server* server);
void sendNotification(Server* server, const i32 sender_id, const u32 size);
void sendNotificationToClinet(const u32 sender_id, const u32 size);
