#include "Core/Client/Client.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Core/MemoriaCompartida.h"
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

    // LOG_DEBUG("Message length: %d", messagLength);
    return messagLength;
}

void destroyClient(Client* client) {
    destroySharedMemory(&client->memory);
    close(client->client_fd);
}

void sendNotifiacion(Client* client, const u32 lenMeesage) {
    send(client->client_fd, &lenMeesage, sizeof(lenMeesage), 0);
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
    // NOTE: checar si esto no afecta nada!
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
