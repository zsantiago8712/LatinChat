#pragma once

#include "Core/gui.h"
#include "Utils/types.h"

void initGui(void);

void createMainWindow(Gui* gui);
void terminateGui(Gui* gui);

void createInputWindow(Gui* gui);
void createChatWindow(Gui* gui);

i32 getInputWindow(Gui* gui);

void showNewMessage(Gui* gui, const char* message, const u32 index);

bool processInputKey(Gui* gui);
