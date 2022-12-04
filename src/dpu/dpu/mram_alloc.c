#include "mram_alloc.h"
static block find_block(mram_allocator *allocator, size_t s);
static block new_block(mram_allocator *allocator, size_t s);
static int split_block(mram_allocator *allocator, block b, size_t s);
static size_t align8(size_t s);
static int valid_addr(mram_allocator *allocator, __mram_ptr void *p);
static block merge(mram_allocator *allocator, block b);

void mram_allocator_initial(mram_allocator *allocator, __mram_ptr void *start, __mram_ptr void *end)
{
    allocator->start = start;
    allocator->end = end;
    allocator->cur = start;
    allocator->first_block = (__mram_ptr void *)NULL;
    allocator->last_block = (__mram_ptr void *)NULL;
    allocator->mram_size = (size_t)(allocator->end) - (size_t)(allocator->start);
    allocator->avail_size = allocator->mram_size;
}

// find the first block with size >= the given size
static block find_block(mram_allocator *allocator, size_t s)
{
    // multiple random accesses to MRAM, low efficiency, how to improve?
    block b = allocator->first_block;
    while (b && !(b->free && b->size >= s))
    {
        b = b->next;
    }
    return b;
}

// allocate a new block with given size and add it to the end of the list
// allocator: the allocator managing this mram
// last: the last block in the list
// s   : the given size
// return the allocated block
static block new_block(mram_allocator *allocator, size_t s)
{
    if (allocator->avail_size < s + BLOCK_SIZE)
    {
        printf("\nERROR: not enough space for a new block\n");
        return (__mram_ptr void *)NULL;
    }

    block b, last;
    last = allocator->last_block;
    b = (block)allocator->cur;
    b->size = s;
    b->prev = last;
    if (last)
        last->next = b;
    b->next = (__mram_ptr void *)NULL;
    b->free = 0;
    // not sure whether it is allowed to do algorithmic and comparison opeations to `void *` type
    b->data_addr = (__mram_ptr char *)allocator->cur + BLOCK_SIZE;

    if (!last)
        allocator->first_block = b;
    allocator->last_block = b;
    allocator->cur = (__mram_ptr char *)allocator->cur + BLOCK_SIZE + s;
    allocator->avail_size -= BLOCK_SIZE + s;

    return b;
}

// split a block, the first block's data region shrinks to the given size, and the rest space becomes the second block
// need to guarantee enough space for the second block before calling this method
// b: the block to split
// s: the size of the first data region
// a naive method to aovid internal fragments
static int split_block(mram_allocator *allocator, block b, size_t s)
{
    if (!b->free)
    {
        printf("\nERROR: split a busy block\n");
        return 0;
    }
    block new;
    new = (block)(b->data + s);
    new->size = b->size - s - BLOCK_SIZE;
    new->prev = b;
    new->next = b->next;
    new->free = 1;
    new->data_addr = b->data + s + BLOCK_SIZE;

    b->size = s;
    b->next = new;
    if (new->next)
        new->next->prev = new;

    if (allocator->last_block == b)
        allocator->last_block = new;

    return 1;
}

// align a size to multiple of 8
static size_t align8(size_t s)
{
    if ((s & 0x7) == 0)
        return s;
    return ((s >> 3) + 1) << 3;
}

// whether the pointer is valid to be freed
static int valid_addr(mram_allocator *allocator, __mram_ptr void *p)
{
    if (allocator->first_block)
    {
        if (p >= allocator->start && p < allocator->cur)
        {
            block b = get_block(p);
            return p == b->data_addr;
        }
    }
    return 0;
}

// merge the block and its next block, return the merged block
// a naive method to avoid external fragments
static block merge(mram_allocator *allocator, block b)
{
    if (!b->free)
    {
        printf("ERROR: merge a busy block");
        return (__mram_ptr void *)NULL;
    }
    if (b->next && b->next->free)
    {
        b->size += BLOCK_SIZE + b->next->size;
        if (b->next == allocator->last_block)
            allocator->last_block = b;
        b->next = b->next->next;
        if (b->next)
            b->next->prev = b;
    }
    return b;
}

__mram_ptr void *mram_alloc(mram_allocator *allocator, size_t size)
{
    block b;
    size_t s = align8(size);
    if (allocator->first_block)
    { // list not empty
        b = find_block(allocator, s);
        if (b)
        {
            // split if possible to avoid internal fragments
            if ((b->size - s) >= (BLOCK_SIZE + 8))
                split_block(allocator, b, s);
            b->free = 0;
            allocator->avail_size -= b->size + BLOCK_SIZE;
        }
        else
        {
            b = new_block(allocator, s);
            if (!b)
                return (__mram_ptr void *)NULL;
        }
    }
    else
    { // the list is empty
        b = new_block(allocator, s);
        if (!b)
            return (__mram_ptr void *)NULL;
    }

    return b->data;
}

void mram_free(mram_allocator *allocator, __mram_ptr void *ptr)
{
    block b;
    if (valid_addr(allocator, ptr))
    {
        b = get_block(ptr);
        b->free = 1;
        allocator->avail_size += b->size + BLOCK_SIZE;
        // merge neighbor free blocks to avoid external fragments
        if (b->prev && b->prev->free)
        {
            b = merge(allocator, b->prev); // previous block
        }
        if (b->next)
        {
            merge(allocator, b); // next block
        }
        else
        { // b is the last block in the list, reclaim this block
            if (b->prev)
            {
                allocator->last_block = b->prev;
                b->prev->next = (__mram_ptr void *)NULL;
            }
            else
            { // b is the last and only block in the list
                allocator->first_block = (__mram_ptr void *)NULL;
                allocator->last_block = (__mram_ptr void *)NULL;
            }
            allocator->cur = b;
        }
    }
}

// print information of the allocator
void print_allocator(mram_allocator *allocator)
{
    printf("\n======================ALLOC INFO======================\n");
    printf("Range: 0x%.7x -- 0x%.7x\n", (uint32_t)allocator->start, (uint32_t)allocator->end);
    printf("MRAM size: %u B, available size: %u B, used: %u B\n",
           allocator->mram_size, allocator->avail_size, allocator->mram_size - allocator->avail_size);
    printf("cur: 0x%.7x\n", (uint32_t)allocator->cur);
    printf("list head: 0x%.7x, list tail: 0x%.7x\n", (uint32_t)allocator->first_block, (uint32_t)allocator->last_block);
    printf("======================================================\n");
}

// print information of the list of blocks
void print_list(mram_allocator *allocator)
{
    block b = allocator->first_block;
    int id = 0;
    printf("\n");
    if (!b)
        printf("\nThe list is empty\n");
    while (b)
    {
        print_block(b, id);
        b = b->next;
        id++;
    }
}

// print information of a particular block
void print_block(block b, int id)
{
    printf("-----------------------BLOCK %d------------------------\n", id);
    printf("0x%.7x(size): 0x%.8x,  0x%.7x(free): 0x%.8x\n",
           (uint32_t)&b->size, b->size, (uint32_t)&b->free, b->free);
    printf("0x%.7x(prev): 0x%.8x,  0x%.7x(next): 0x%.8x\n",
           (uint32_t)&b->prev, (uint32_t)b->prev, (uint32_t)&b->next, (uint32_t)b->next);
    printf("0x%.7x(pad) : 0x%.8x,  0x%.7x(addr): 0x%.8x\n",
           (uint32_t)&b->padding, b->padding, (uint32_t)&b->data_addr, (uint32_t)b->data_addr);
    for (int i = 0; i < 2; i++)
    {
        printf("0x%.7x(data+%d): 0x%.8x,  0x%.7x(data+%d): 0x%.8x\n",
               (uint32_t)(b->data + 8 * i), 8 * i, ((__mram_ptr int *)b->data)[2 * i],
               (uint32_t)(b->data + 8 * i + 4), 8 * i + 4, ((__mram_ptr int *)b->data)[2 * i + 1]);
    }
}