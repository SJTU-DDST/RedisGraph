#ifndef _MRAM_ALLOC_H
#define _MRAM_ALLOC_H
#include <stddef.h>
#include <stdio.h>
#include <mram.h>

#define MRAM_HEAP_START_PTR ((__mram_ptr void *)DPU_MRAM_HEAP_POINTER)
#define MRAM_HEAP_END_PTR ((__mram_ptr void *)(0x4000000))

struct _block{
    size_t size; // size of the data region; 4B
    int free;  //4B
    __mram_ptr struct _block * prev; // previous block; 4B
    __mram_ptr struct _block * next; // next block; 4B
    int padding; //for 8B alignment; 4B
    __mram_ptr void * data_addr; //start address of data region, useful when checking whether a pointer is valid to be freed; 4B

    char data[1]; // start of data region
};
typedef __mram_ptr struct _block *block;
#define BLOCK_SIZE 24
#define get_block(p) ((block) ((__mram_ptr char *)p - BLOCK_SIZE))

typedef struct mram_allocator
{
    __mram_ptr void *start;
    __mram_ptr void *end;
    __mram_ptr void *cur;
    block first_block;
    block last_block;
    size_t avail_size;
    size_t mram_size;
} mram_allocator;

// Initialize the allocator
void mram_allocator_initial(mram_allocator *allocator, __mram_ptr void *start, __mram_ptr void *end);

// allocate a continuous mram region with given size
__mram_ptr void *mram_alloc(mram_allocator* allocator,size_t size);

// free the pre-allocated pointer
void mram_free(mram_allocator* allocator,__mram_ptr void *ptr);

// print information of the allocator
void print_allocator(mram_allocator *allocator);

//print information of the list of blocks
void print_list(mram_allocator *allocator);

//print information of a particular block
void print_block(block b, int id);

#endif