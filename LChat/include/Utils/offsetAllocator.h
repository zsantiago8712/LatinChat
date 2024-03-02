// (C) Sebastian Aaltonen 2023
// MIT License (see file: LICENSE)

// #define USE_16_BIT_OFFSETS

#pragma once

#include "types.h"
// 16 bit offsets mode will halve the metadata storage cost
// But it only supports up to 65536 maximum allocation count
#ifdef USE_16_BIT_NODE_INDICES
typedef u16 NodeIndex;
#else
typedef u32 NodeIndex;
#endif

typedef struct _Node* Node;

#define NUM_TOP_BINS 32
#define BINS_PER_LEAF 8
#define TOP_BINS_INDEX_SHIFT 3
#define LEAF_BINS_INDEX_MASK 0x7
#define NUM_LEAF_BINS (NUM_TOP_BINS * BINS_PER_LEAF)

extern const u32 NO_SPACE;  // Declaración, sin definir aquí

#define EmptyAllocation \
    { .offset = NO_SPACE, .metadata = NO_SPACE }

typedef struct {
    u32 offset;
    NodeIndex metadata;  // internal: node index
} Allocation;

typedef struct {
    u32 totalFreeSpace;
    u32 largestFreeRegion;
} StorageReport;

typedef struct {
    struct {
        u32 size;
        u32 count;
    } freeRegions[NUM_LEAF_BINS];
} StorageReportFull;

typedef struct {
    u32 m_size;
    u32 m_maxAllocs;
    u32 m_freeStorage;

    u32 m_usedBinsTop;
    u8 m_usedBins[NUM_TOP_BINS];
    NodeIndex m_binIndices[NUM_LEAF_BINS];

    Node m_nodes;
    NodeIndex* m_freeNodes;
    u32 m_freeOffset;
} Allocator;

void initOffsetAllocator(Allocator* allocator,
                         const u32 size,
                         const u32 max_allocs);

void resetOffsetAllocator(Allocator* allocator);

void terminateOffsetAllocator(Allocator* allocator);

Allocation offsetAllocateAllocate(Allocator* allocator, const u32 size);

void freeAllocation(Allocator* allocator, Allocation allocation);

u32 allocationSize(const Allocator* allocator, const Allocation allocation);
