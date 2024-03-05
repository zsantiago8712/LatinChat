#pragma once

#include <semaphore.h>
#include "Utils/types.h"

#define MEMORY_SIZE 1024 * 256
#define MEMORY_NAME "/LChatMemory"
#define SEM_NAME "/LChatSemaphore"
#define MAX_NUM_MENSAJES 100

typedef struct {
    void* buffer;
    u32 size;
    i32 id;
    const char* name;
    u32 num_mensajes;
    sem_t* semaforo;
    i32 size_mensajes[MAX_NUM_MENSAJES];
    i32 free_space;
    i32 offset;

} ShareMemory;

#define NewMemory                                                       \
    (ShareMemory) {                                                     \
        NULL, MEMORY_SIZE, 0, MEMORY_NAME, 0, NULL, {0}, MEMORY_SIZE, 0 \
    }

bool createSharedMemory(ShareMemory* memory);
void destroySharedMemory(ShareMemory* memory);
