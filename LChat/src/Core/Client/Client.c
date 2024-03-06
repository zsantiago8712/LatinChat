#include "Core/Client/Client.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Core/MemoriaCompartida.h"
#include "Core/Mensajes.h"
#include "Core/Server/ServerGui.h"
#include "Utils/clog.h"

static bool createSocket(i32* server_fd);

static bool defineServerDirection(struct sockaddr_in* adress,
                                  const i32 port,
                                  const char ip[16]);

static bool connectToServer(struct sockaddr_in* address, const i32 client_fd);

Client createClient(const u32 port, const char ip[16]) {
    Client new_client = NewClient;

    if (!createSocket(&new_client.client_fd)) {
        LOG_FATAL("Could not create socket");
        return NewClient;
    }

    if (!defineServerDirection(&new_client.adress, port, ip)) {
        LOG_FATAL("Could not bind socket");
        return NewClient;
    }

    new_client.port = port;
    strncpy(new_client.ip, ip, MAX_SIZE_IP);

    if (!connectToServer(&new_client.adress, new_client.client_fd)) {
        LOG_FATAL("Could not connect to server");
        return NewClient;
    }

    if (!createSharedMemory(&new_client.memory)) {
        LOG_FATAL("Could not create shared memory");
        return NewClient;
    }
    LOG_DEBUG("Client created with port: %d and ip: %s", new_client.port,
              new_client.ip);
    return new_client;
}

i32 updateClient(Client* client) {
    struct timeval tv = {
        .tv_sec = 2,
        .tv_usec = 0,
    };

    u32 messagLength = 0;

    FD_ZERO(&client->read_fds);
    FD_SET(client->client_fd, &client->read_fds);

    if (select(client->client_fd + 1, &client->read_fds, NULL, NULL, &tv) < 0) {
        LOG_FATAL("Could not select");
        return messagLength;
    }

    if (FD_ISSET(client->client_fd, &client->read_fds)) {
        if (recv(client->client_fd, &messagLength, sizeof(messagLength), 0) <=
            0) {
            LOG_FATAL("Error on server or server disconnected");
            return SERVER_DISCONNECTED;
        }
    }

    return messagLength;
}

void destroyClient(Client* client) {
    destroySharedMemory(&client->memory);
    close(client->client_fd);
}

void sendNotifiacion(Client* client, const u32 lenMeesage) {
    send(client->client_fd, &lenMeesage, sizeof(lenMeesage), 0);
}

void makeSocketNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        LOG_FATAL("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        LOG_FATAL("fcntl F_SETFL O_NONBLOCK");
        exit(EXIT_FAILURE);
    }
}

void makeSocketBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL, removing O_NONBLOCK");
        exit(EXIT_FAILURE);
    }
}

void getAllMessages(Client* client, Gui* gui) {
    int result;
    fd_set readfds;
    struct timeval timeout;

    // Hacer el socket no bloqueante
    makeSocketNonBlocking(client->client_fd);

    // Configurar el conjunto de descriptores de lectura y el tiempo de espera
    FD_ZERO(&readfds);
    FD_SET(client->client_fd, &readfds);
    timeout.tv_sec = 1;  // Espera hasta 1 segundo para los mensajes
    timeout.tv_usec = 0;

    while (true) {
        Mensaje mensaje = NewMessage;
        FD_ZERO(&readfds);
        FD_SET(client->client_fd, &readfds);
        result = select(client->client_fd + 1, &readfds, NULL, NULL, &timeout);

        if (result < 0) {
            break;
        } else if (result == 0) {
            break;
        }

        // Hay datos disponibles para leer
        ssize_t nbytes = recv(client->client_fd, &mensaje.longitud,
                              sizeof(mensaje.longitud), 0);
        if (nbytes <= 0) {
            break;
        }

        getLastMessage(&client->memory, mensaje.contenido, mensaje.longitud);
        showNewMessage(gui, mensaje.contenido, client->memory.num_mensajes);
    }

    // Cambiar el socket de vuelta a modo bloqueante
    makeSocketBlocking(client->client_fd);
}

static bool createSocket(i32* client_fd) {
    *client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (*client_fd <= 0) {
        return false;
    }

    return true;
}

static bool defineServerDirection(struct sockaddr_in* adress,
                                  const i32 port,
                                  const char ip[16]) {
    memset(adress, 0, sizeof(*adress));

    adress->sin_family = AF_INET;
    adress->sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &adress->sin_addr) <= 0) {
        LOG_FATAL("Invalid address");
        return false;
    }

    return true;
}

static bool connectToServer(struct sockaddr_in* address, const i32 client_fd) {
    if (connect(client_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
        LOG_FATAL("Could not connect to server");
        return false;
    }

    return true;
}
