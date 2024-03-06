#include "Core/Server/ServerApp.h"
#include <stdbool.h>
#include "Core/Mensajes.h"
#include "Core/Server/Server.h"
#include "Core/Server/ServerGui.h"
#include "Core/gui.h"
#include "Utils/clog.h"

typedef struct {
    Server server;
    Gui gui;
    bool running;
} App;

#define NewApp                   \
    (App) {                      \
        NewServer, NewGui, false \
    }

static App* getApp(void);

void initApp(void) {
    App* app = getApp();

    initLogger(L_FILE);
    // setLogFile("Logs/LChatServer.log");
    initGui();
    createMainWindow(&app->gui);
    createInputWindow(&app->gui);
}

void runApp(void) {
    App* app = getApp();
    Mensaje mensaje = NewMessage;

    const i32 port = getInputWindow(&app->gui);

    app->server = createServer(port);
    createChatWindow(&app->gui);
    showNewMessage(&app->gui, "(@Server): Welcome to LChat!\n",
                   app->server.memory.num_mensajes);
    app->running = true;

    while (app->running) {
        InfoMensaje mensaje_socket = updateServer(&app->server);
        if (mensaje_socket.longitud > 0) {
            LOG_DEBUG("Received message from %d", mensaje_socket.id_cliente);
            mensaje.longitud = mensaje_socket.longitud;
            getLastMessage(&app->server.memory, mensaje.contenido,
                           mensaje.longitud);
            showNewMessage(&app->gui, mensaje.contenido,
                           app->server.memory.num_mensajes);
            sendNotification(&app->server, mensaje_socket.id_cliente,
                             mensaje.longitud);
        }
        app->running = processInputKey(&app->gui);
    }
}

void terminateApp(void) {
    App* app = getApp();
    LOG_DEBUG("Terminating server APP");
    terminateGui(&app->gui);
    destroyServer(&app->server);
    terminateLogger();
}

static App* getApp(void) {
    static App app = NewApp;
    return &app;
}
