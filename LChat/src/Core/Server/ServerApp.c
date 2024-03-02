#include "Core/Server/ServerApp.h"
#include <stdbool.h>
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
    setLogFile("Logs/LChatServer.log");
    initGui();
    createMainWindow(&app->gui);
    createInputWindow(&app->gui);
}

void runApp(void) {
    App* app = getApp();

    // TODO: INIT GUI ASK FOR PORT IF KEY PRESSED ESC USE DEFAULT PORT
    const i32 port = getInputWindow(&app->gui);

    app->server = createServer(port);
    app->running = true;

    createChatWindow(&app->gui);

    while (app->running) {
        acceptNewConnection(&app->server);
        app->running = false;
    }
}

void terminateApp(void) {
    App* app = getApp();
    terminateGui(&app->gui);
    destroyServer(app->server);
    terminateLogger();
}

static App* getApp(void) {
    static App app = NewApp;
    return &app;
}
