#ifndef _LINEAR_MRAM_ALLOC_H
#define _LINEAR_MRAM_ALLOC_H
#include <stddef.h>
#include <mram.h>

#define MRAM_HEAP_START_PTR ((__mram_ptr void *)DPU_MRAM_HEAP_POINTER)
#define MRAM_HEAP_END_PTR ((__mram_ptr void *)(0x4000000))

typedef struct linear_mram_allocator
{
    __mram_ptr void *start;
    __mram_ptr void *cur;
    size_t avail_size;
    size_t mram_size;
} linear_mram_allocator;

void linear_mram_allocator_initial(linear_mram_allocator *allocator,__mram_ptr void *start,size_t size);
__mram_ptr void * linear_mram_alloc(linear_mram_allocator* allocator,size_t size);
void linear_mram_free(linear_mram_allocator* allocator,__mram_ptr void *ptr);
#endif