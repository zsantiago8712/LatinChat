#pragma once

#include "Utils/types.h"

#include "Core/MemoriaCompartida.h"

#define MAX_TAMAÑO_MENSAJE 200

typedef struct {
    i32 longitud;                        // Longitud del mensaje
    char contenido[MAX_TAMAÑO_MENSAJE];  // Contenido del mensaje
} Mensaje;

#define NewMessage \
    (Mensaje) {    \
        0, ""      \
    }

void getLastMessage(ShareMemory* memory, char* mensaje, i32 max_length);
void sendMessage(ShareMemory* memory, const char* mensaje);
