#include "Core/Server/Server.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Utils/clog.h"

static bool createsocket(i32* server_fd);

static bool defineServerDirection(const i32 server_fd,
                                  struct sockaddr_in* address,
                                  const i32 port);

Server createServer(const i32 portChoose) {
    Server new_server = NewServer;

    if (!createsocket(&new_server.server_fd)) {
        LOG_FATAL("Could not create socket");
        exit(EXIT_FAILURE);
    }

    if (!defineServerDirection(new_server.server_fd, &new_server.address,
                               portChoose)) {
        LOG_FATAL("Could not bind socket");
        exit(EXIT_FAILURE);
    }

    return new_server;
}

void destroyServer(const Server server) {
    close(server.server_fd);
}

static bool createsocket(i32* server_fd) {
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (*server_fd <= 0) {
        return false;
    }

    return true;
}

bool acceptNewConnection(Server* server) {
    if (server->num_clients_connected < MAX_CLIENTS) {
        server->new_sockets[++server->num_clients_connected] =
            accept(server->server_fd, (struct sockaddr*)&server->address,
                   (socklen_t*)&server->server_fd);
        return true;
    }

    return false;
}

static bool defineServerDirection(const i32 server_fd,
                                  struct sockaddr_in* address,
                                  const i32 port) {
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    address->sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
        return false;
    }

    if (listen(server_fd, 3) < 0) {
        return false;
    }

    return true;
}
