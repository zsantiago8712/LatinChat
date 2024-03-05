#include "Core/Mensajes.h"
#include <string.h>
#include "Utils/clog.h"

void sendMessage(ShareMemory* memory, const char* mensaje) {
    sem_wait(memory->semaforo);

    const i32 length = strlen(mensaje);
    if (memory->free_space < length) {
        // TODO: liberar memoria en memoria compartida (osea borrar el primer
        // mensaje)
        LOG_FATAL("No hay espacio suficiente para un nuevo mensaje");
        return;
    }

    LOG_DEBUG("%s -> %d ->%d", mensaje, length, memory->offset);
    // Calcula la posición actual basada en num_mensajes y tamaño de Mensaje
    char* pos = (char*)memory->buffer + memory->offset;

    // Asegurarse de no sobrepasar el tamaño de la memoria compartida
    if (memory->free_space - length < 0) {
        LOG_FATAL("No hay espacio suficiente para un nuevo mensaje");
        return;
    }

    // Copiar el mensaje en la posición calculada
    strncpy(pos, mensaje, length);

    // Decrementar free_space
    memory->free_space -= length;

    memory->size_mensajes[memory->num_mensajes] = length;
    memory->offset += length;

    // Incrementar num_mensajes
    memory->num_mensajes++;
    LOG_DEBUG("SEND offset to %d", memory->offset);
    sem_post(memory->semaforo);
}

void getLastMessage(ShareMemory* memory, char* mensaje, i32 length) {
    sem_wait(memory->semaforo);

    LOG_DEBUG("Get Offset: %d", memory->offset);

    strncpy(mensaje, (char*)memory->buffer + memory->offset, length);
    mensaje[length + 1] = '\0';

    LOG_DEBUG("Get Offset: %d + %d", memory->offset, length);
    memory->offset += length;
    LOG_DEBUG("Get Offset: %d", memory->offset);

    memory->size_mensajes[memory->num_mensajes] = length;
    memory->free_space -= sizeof(char) * length;
    memory->num_mensajes++;

    sem_post(memory->semaforo);
}
