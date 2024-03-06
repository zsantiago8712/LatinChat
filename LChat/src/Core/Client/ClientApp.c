#include "Core/Client/ClientApp.h"
#include <stdbool.h>
#include <stdlib.h>
#include "Core/Client/Client.h"
#include "Core/Client/ClientGui.h"
#include "Core/Mensajes.h"
#include "Core/gui.h"
#include "Utils/clog.h"
#include "Utils/types.h"

typedef struct {
    Client client;
    Gui gui;
    bool running;
} App;

#define NewApp                   \
    (App) {                      \
        NewClient, NewGui, false \
    }

static App* getApp(void);

void initApp(void) {
    App* app = getApp();

    initLogger(L_FILE);
    initGui();
    // setLogFile("Logs/LChatServer.logs");

    // TODO: INIT GUI ASK FOR PORT IF KEY PRESSED ESC USE DEFAULT PORT
    app->client = createClient(DEFAULT_PORT, DEFAULT_IP);

    if (app->client.client_fd == -1) {
        LOG_FATAL("Could not create socket");
        exit(EXIT_FAILURE);
    }

    app->running = true;
    LOG_DEBUG("Client started");
    createMainWindow(&app->gui);
    createChatWindow(&app->gui);
}

void runApp(void) {
    App* app = getApp();

    getAllMessages(&app->client, &app->gui);
    Mensaje input = NewMessage;
    Mensaje mensaje = NewMessage;
    // TODO: GUI
    while (app->running) {
        mensaje = NewMessage;
        mensaje.longitud = updateClient(&app->client);

        if (mensaje.longitud > 0) {
            getLastMessage(&app->client.memory, mensaje.contenido,
                           mensaje.longitud);
            LOG_INFO(mensaje.contenido);

            // TODO: mostrar mensaje en GUI o en printf por mientras
        } else if (mensaje.longitud == SERVER_DISCONNECTED) {
            LOG_DEBUG("Server disconnected");
            app->running = false;
            break;
        }

        app->running = processKey(&app->client, &app->gui, &input);
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
