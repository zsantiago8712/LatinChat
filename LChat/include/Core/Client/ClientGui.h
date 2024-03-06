#pragma once
#include "Core/Client/Client.h"
#include "Core/Mensajes.h"
#include "Core/gui.h"
void initGui(void);
void createMainWindow(Gui* gui);
void createChatWindow(Gui* gui);

bool processKey(Client* cliente, Gui* gui, Mensaje* input);
void showNewMessage(Gui* gui, const char* message, const u32 index);