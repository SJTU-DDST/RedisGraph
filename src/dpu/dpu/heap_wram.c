#include "heap_wram.h"

//构建一个大小为n的堆
heap *heap_wram_init(heap *h, uint32_t capacity, heapType type, int transpose)
{
    h->type = type;
    h->capacity = capacity;
    h->size = 0;
    if (!transpose)
        h->node_compare_function = heap_node_compare;
    else
        h->node_compare_function = heap_node_compare_transpose;
    h->heap = (heap_node *)mem_alloc(sizeof(heap_node) * (capacity + 1));
    return h;
}
