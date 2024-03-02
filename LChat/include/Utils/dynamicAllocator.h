#pragma once
#include "types.h"

#define GIGABYTES_TO_BYTES(n) ((u32)(n) * 1024 * 1024 * 1024)
#define MEGABYTES_TO_BYTES(n) ((u32)(n) * 1024 * 1024)
#define KILOBYTES_TO_BYTES(n) ((u32)(n) * 1024)

bool initDynamicAllocator(const u32 size);
void terminateDynamicAllocator(void);

void* allocate(const u32 size);
bool deallocate(void* ptr, const u32 size);
bool reallocate(void* old_ptr, const u32 old_size, const u32 new_size);
