#include "Utils/dynamicAllocator.h"
#include <assert.h>
#include <stdlib.h>
#include "Utils/clog.h"
#include "Utils/offsetAllocator.h"
#include "Utils/uthash.h"

#define MAX_ALLOCS 1024 * 128
#define MAX_ALLOCATIONS 100

typedef struct {
    Allocation allocation;
    void* mem_block;
    UT_hash_handle hh;
} AllocationEntry;

typedef struct {
    void* block;
    Allocator offset_allocator;
    AllocationEntry allocations[MAX_ALLOCATIONS];
    AllocationEntry* allocations_hash;
} DynamicAllocator;

bool searchAllocation(const void* mem_block);
void deleteAllAllocations(void);

DynamicAllocator* getDynamicAllocator(void) {
    static DynamicAllocator allocator = {.block = NULL,
                                         .allocations_hash = NULL};
    return &allocator;
}

bool initDynamicAllocator(const u32 size) {
    assert(size > 0);

    DynamicAllocator* allocator = getDynamicAllocator();

    initOffsetAllocator(&allocator->offset_allocator, size, MAX_ALLOCS);
    allocator->block = malloc(size);

    if (allocator->block == NULL) {
        return false;
    }

    LOG_INFO("Offset allocator initialized");
    return true;
}

void terminateDynamicAllocator(void) {
    DynamicAllocator* allocator = getDynamicAllocator();
    deleteAllAllocations();
    terminateOffsetAllocator(&allocator->offset_allocator);
    free(allocator->block);
    LOG_INFO("Offset allocator terminated");
}

void* allocate(const u32 size) {
    assert(size > 0);

    DynamicAllocator* allocator = getDynamicAllocator();
    Allocation new_allocation =
        offsetAllocateAllocate(&allocator->offset_allocator, size);

    if (new_allocation.offset == NO_SPACE ||
        new_allocation.metadata == NO_SPACE) {
        freeAllocation(&allocator->offset_allocator, new_allocation);
        return NULL;
    }

    void* new_block = (char*)allocator->block + new_allocation.offset;
    if (searchAllocation(new_block) == true) {
        freeAllocation(&allocator->offset_allocator, new_allocation);
        return NULL;
    }

    allocator->allocations[new_allocation.metadata].allocation = new_allocation;
    allocator->allocations[new_allocation.metadata].mem_block = new_block;

    HASH_ADD_PTR(allocator->allocations_hash, mem_block,
                 &allocator->allocations[new_allocation.metadata]);
    return new_block;
}

bool reallocate(void* old_ptr, const u32 old_size, const u32 new_size) {
    assert(new_size > 0 && old_ptr != NULL && old_size < new_size);

    void* new_ptr = allocate(new_size);

    if (new_ptr == NULL) {
        return false;
    }
    memcpy(new_ptr, old_ptr, old_size);
    deallocate(old_ptr, old_size);

    old_ptr = NULL;
    old_ptr = new_ptr;
    return true;
}

bool searchAllocation(const void* mem_block) {
    DynamicAllocator* allocator = getDynamicAllocator();

    AllocationEntry* found;

    HASH_FIND_PTR(allocator->allocations_hash, mem_block, found);

    if (found == NULL) {
        return false;
    }

    return true;
}

void deleteAllAllocations(void) {
    DynamicAllocator* allocator = getDynamicAllocator();

    AllocationEntry* current_allocation;
    AllocationEntry* tmp;
    //
    HASH_ITER(hh, allocator->allocations_hash, current_allocation, tmp) {
        freeAllocation(&allocator->offset_allocator,
                       current_allocation->allocation);
        HASH_DEL(allocator->allocations_hash, current_allocation);
    }
}

bool deallocate(void* ptr_to_search, const u32 size) {
    assert(ptr_to_search != NULL);
    assert(size > 0);

    DynamicAllocator* allocator = getDynamicAllocator();
    AllocationEntry* allocation = NULL;

    HASH_FIND_PTR(allocator->allocations_hash, ptr_to_search, allocation);

    if (allocation == NULL) {
        return false;
    }

    freeAllocation(&allocator->offset_allocator, allocation->allocation);
    allocator->allocations[allocation->allocation.metadata].mem_block = NULL;
    HASH_DEL(allocator->allocations_hash, allocation);
    return true;
}
