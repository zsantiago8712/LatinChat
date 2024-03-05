#include "Core/MemoriaCompartida.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Utils/clog.h"
#define PERMISION 0666

bool createSharedMemory(ShareMemory* memory) {
    void* ptr = NULL;
    memory->id = shm_open(memory->name, O_CREAT | O_RDWR, PERMISION);

    if (memory->id <= 0) {
        LOG_FATAL("Could not create shared memory");
        return false;
    }

    struct stat mapstat;
    if (-1 != fstat(memory->id, &mapstat) && mapstat.st_size == 0) {
        if (ftruncate(memory->id, memory->size) < 0) {
            LOG_FATAL("Could not truncate shared memory");
            return false;
        }
    }

    ptr = mmap(NULL, memory->size, PROT_READ | PROT_WRITE, MAP_SHARED,
               memory->id, 0);

    if (ptr == MAP_FAILED) {
        LOG_FATAL("Could not map shared memory");
        return false;
    }

    memory->buffer = ptr;

    memory->semaforo = sem_open(SEM_NAME, O_CREAT, 0644, 2);
    if (memory->semaforo == SEM_FAILED) {
        LOG_FATAL("sem_open failed");
        exit(EXIT_FAILURE);
    }

    sem_post(memory->semaforo);

    return true;
}

bool accesToSharedMemory(ShareMemory* memory) {
    memory->id = shm_open(memory->name, O_RDWR, PERMISION);

    if (memory->id <= 0) {
        LOG_FATAL("Could not access shared memory");
        return false;
    }

    memory->buffer = mmap(NULL, memory->size, PROT_READ | PROT_WRITE,
                          MAP_SHARED, memory->id, 0);

    if (memory->buffer == MAP_FAILED) {
        LOG_FATAL("Could not access shared memory");
        return false;
    }
    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        LOG_FATAL("sem_open failed");
        exit(EXIT_FAILURE);
    }

    return true;
}

void destroySharedMemory(ShareMemory* memory) {
    LOG_DEBUG("Destroying shared memory");
    munmap(memory->buffer, memory->size);
    close(memory->id);
    shm_unlink(memory->name);
}
