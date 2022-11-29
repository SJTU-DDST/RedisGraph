#ifndef __HEAP_WRAM_H
#define __HEAP_WRAM_H

#include "../util/heap.h"
#include <alloc.h>

heap *heap_wram_init(heap *h, uint32_t capacity, heapType type);

#endif