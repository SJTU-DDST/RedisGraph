#include "linear_mram_alloc.h"

void linear_mram_allocator_initial(linear_mram_allocator *allocator,__mram_ptr void *start,size_t size)
{
    allocator->start = start;
    allocator->cur = start;
    allocator->mram_size = size;
    allocator->avail_size = allocator->mram_size;
}

__mram_ptr void *linear_mram_alloc(linear_mram_allocator *allocator, size_t size)
{
    if (size % 8)
    {
        size = (size / 8 + 1) * 8;
    }
    if (allocator->avail_size < size)
        return (__mram_ptr void *)NULL;
    __mram_ptr void *ptr = allocator->cur;
    allocator->cur = (__mram_ptr void *)(allocator->cur + size);
    allocator->avail_size = allocator->avail_size - size;
    return ptr;
}
void linear_mram_free(linear_mram_allocator *allocator, __mram_ptr void *ptr)
{
    return;
}