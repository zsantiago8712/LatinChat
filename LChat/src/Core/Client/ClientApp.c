#include "Core/Client/ClientApp.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Core/Client/Client.h"
#include "Core/Mensajes.h"
#include "Utils/clog.h"
#include "Utils/types.h"

typedef struct {
    Client client;
    bool running;
} App;

#define NewApp \
    (App) {    \
        false  \
    }

static App* getApp(void);

void initApp(void) {
    App* app = getApp();

    initLogger(STDERR);
    // setLogFile("Logs/LChatServer.logs");

    // TODO: INIT GUI ASK FOR PORT IF KEY PRESSED ESC USE DEFAULT PORT
    app->client = createClient(DEFAULT_PORT, DEFAULT_IP);

    if (app->client.client_fd == -1) {
        LOG_FATAL("Could not create socket");
        exit(EXIT_FAILURE);
    }

    app->running = true;
    LOG_DEBUG("Client started");
}

void runApp(void) {
    App* app = getApp();
    Mensaje mensaje = NewMessage;

    i32 firstTime = 0;
    // TODO: GUI
    while (app->running) {
        mensaje.longitud = updateClient(&app->client);
        if (mensaje.longitud > 0) {
            app->client.memory.num_mensajes++;

            getLastMessage(&app->client.memory, mensaje.contenido,
                           mensaje.longitud);
            puts(mensaje.contenido);

            // TODO: obtener mensaje de memoria compartida
            // TODO: mostrar mensaje en GUI o en printf por mientras
        } else if (mensaje.longitud == SERVER_DISCONNECTED) {
            LOG_DEBUG("Server disconnected");
            app->running = false;
        }

        if (firstTime == 1) {
            const char* msg = "(@Daniel): Yes!!!";
            const u32 size = strlen(msg);
            sendMessage(&app->client.memory, msg);
            sendNotifiacion(&app->client, size);
            puts(msg);
        } else if (firstTime == 5) {
            app->running = false;
        }
        firstTime += 1;
    }
}

void terminateApp(void) {
    App* app = getApp();
    destroyClient(&app->client);
    terminateLogger();
}

static App* getApp(void) {
    static App app = NewApp;
    return &app;
}
