#include "Core/Server/Server.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Core/Mensajes.h"
#include "Utils/clog.h"

#define PERMISION 0666
#define GREETING "(@Server): Welcome to LChat!"

static bool createSocket(i32* server_fd);

static bool defineServerDirection(const i32 server_fd,
                                  struct sockaddr_in* address,
                                  const i32 port);

Server createServer(const i32 portChoose) {
    Server new_server = NewServer;

    if (!createSocket(&new_server.server_fd)) {
        LOG_FATAL("Could not create socket");
        exit(EXIT_FAILURE);
    }

    if (!defineServerDirection(new_server.server_fd, &new_server.address,
                               portChoose)) {
        LOG_FATAL("Could not bind socket");
        exit(EXIT_FAILURE);
    }

    if (!createSharedMemory(&new_server.memory)) {
        LOG_FATAL("Could not create shared memory");
        exit(EXIT_FAILURE);
    }

    new_server.max_sd = new_server.server_fd;

    sendMessage(&new_server.memory, GREETING);

    return new_server;
}

// TODO: manejar las desconecciones! Voy a tner que recorrer el array de sockets
InfoMensaje updateServer(Server* server) {
    struct timeval tv = {
        .tv_sec = 2,
        .tv_usec = 0,
    };

    InfoMensaje mensaje = InfoNewMessage;

    FD_ZERO(&server->read_fds);
    FD_SET(server->server_fd, &server->read_fds);

    for (i32 i = 0; i < server->num_clients_connected; i++) {
        LOG_DEBUG("Checking client %d", server->new_sockets[i]);
        FD_SET(server->new_sockets[i], &server->read_fds);
        if (server->new_sockets[i] > (i32)server->max_sd) {
            server->max_sd = server->new_sockets[i];
        }
    }

    if (select(server->max_sd + 1, &server->read_fds, NULL, NULL, &tv) < 0 &&
        errno != EINTR) {
        LOG_ERROR("Error en select()!");
    } else {
        LOG_DEBUG("Select done");
        for (i32 i = 0; i < server->num_clients_connected; i++) {
            if (FD_ISSET(server->new_sockets[i], &server->read_fds)) {
                const i32 result = recv(server->new_sockets[i],
                                        &mensaje.longitud, sizeof(u32), 0);
                if (result > 0) {
                    mensaje.id_cliente = server->new_sockets[i];
                    return mensaje;
                } else if (result == 0) {
                    LOG_DEBUG("Client %d disconnected", server->new_sockets[i]);
                    close(server->new_sockets[i]);

                    for (i32 j = 0; j < server->num_clients_connected; j++) {
                        server->new_sockets[j] = server->new_sockets[j + 1];
                    }
                    server->new_sockets[server->num_clients_connected - 1] = 0;
                    server->num_clients_connected--;
                }
            }
        }
    }

    if (FD_ISSET(server->server_fd, &server->read_fds)) {
        if (!acceptNewConnection(server)) {
            LOG_ERROR("No hay espacio suficiente para un nuevo cliente");
        } else {
            LOG_DEBUG("Accepting new connection %d",
                      server->memory.num_mensajes);
            for (u32 i = 0; i < server->memory.num_mensajes; i++) {
                sendNotificationToClinet(
                    server->new_sockets[server->num_clients_connected - 1],
                    server->memory.size_mensajes[i]);
            }
        }
    }
    return mensaje;
}

void destroyServer(Server* server) {
    LOG_DEBUG("Terminating server");
    destroySharedMemory(&server->memory);
    close(server->server_fd);
}

bool acceptNewConnection(Server* server) {
    socklen_t server_addr_size = sizeof(server->address);
    LOG_DEBUG("Accepting new connection");
    if (server->num_clients_connected < MAX_CLIENTS) {
        const u32 new_socket =
            accept(server->server_fd, (struct sockaddr*)&server->address,
                   &server_addr_size);

        if (new_socket <= 0) {
            LOG_ERROR("Could not accept new connection");
            return false;
        }

        FD_SET(new_socket, &server->read_fds);
        server->new_sockets[server->num_clients_connected++] = new_socket;

        if (server->max_sd < new_socket) {
            server->max_sd = new_socket;
        }

        LOG_DEBUG(
            "!!Se aceptó un nuevo cliente (numero de clientes conectados %d) "
            "nuevo cliente %d",
            server->num_clients_connected, new_socket);
        return true;
    }

    LOG_ERROR("No hay espacio suficiente para un nuevo cliente");
    return false;
}

void sendNotification(Server* server, const i32 sender_id, const u32 size) {
    LOG_DEBUG("Sending notification to %d", sender_id);
    bool succes = false;
    for (i32 i = 0; i < server->num_clients_connected; i++) {
        if (server->new_sockets[i] != sender_id) {
            LOG_DEBUG("Sending notification to %d -> %d",
                      server->new_sockets[i], size);
            if (send(server->new_sockets[i], &size, sizeof(size), 0) <= 0) {
                LOG_ERROR("Could not send notification to %d", sender_id);
            }
        }
    }

    if (!succes) {
        LOG_DEBUG("No se encontro al cliente %d", sender_id);
    }
}

void sendNotificationToClinet(const u32 sender_id, const u32 size) {
    if (send(sender_id, &size, sizeof(size), 0) <= 0) {
        LOG_ERROR("Could not send first notification");
    } else {
        LOG_DEBUG("Notification sent to clinet %d", sender_id);
    }
}

static bool createSocket(i32* server_fd) {
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (*server_fd <= 0) {
        return false;
    }

    return true;
}

static bool defineServerDirection(const i32 server_fd,
                                  struct sockaddr_in* address,
                                  const i32 port) {
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    address->sin_addr.s_addr = INADDR_ANY;

    i32 yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
        LOG_FATAL("setsockopt");
        return false;
    }

    if (bind(server_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
        return false;
    }

    if (listen(server_fd, 3) < 0) {
        return false;
    }

    return true;
}
