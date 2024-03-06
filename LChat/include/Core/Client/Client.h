#pragma once

#include <netinet/in.h>
#include "Core/MemoriaCompartida.h"
#include "Core/gui.h"
#include "Utils/types.h"

#define DEFAULT_PORT 8080
#define DEFAULT_IP "127.0.0.1"

#define NewClient                                 \
    (Client) {                                    \
        "\0", "\0", 0, 0, {}, 0, NewMemory, {}, 0 \
    }

#define MAX_SIZE_IP 17
#define MAX_SIZE_USERNAME 13
#define SERVER_DISCONNECTED -1

typedef struct {
    char username[MAX_SIZE_USERNAME];
    char ip[MAX_SIZE_IP];
    i32 port;
    i32 client_fd;
    struct sockaddr_in adress;
    i32 id;
    ShareMemory memory;
    fd_set read_fds;
    u32 max_sd;
} Client;

Client createClient(const u32 port, const char ip[16]);

void destroyClient(Client* client);

void sendNotifiacion(Client* client, const u32 lenMeesage);

i32 updateClient(Client* client);

void getAllMessages(Client* client, Gui* gui);