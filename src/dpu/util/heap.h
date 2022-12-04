#ifndef __HEAP_H
#define __HEAP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "type.h"

#define HEAP_OK 0
#define HEAP_ERR 1

#define HEAP_MAX_CAPACITY 16
// heap in host

typedef uint32_t heapType;

typedef struct
{
	int32_t row;
	int32_t col;
	uint32_t belong_to;
} heap_node;
typedef struct _heap
{
	heapType type;
	uint32_t size;	   //堆目前的元素
	uint32_t capacity; //堆的大小
	int (*node_compare_function)(heap_node *a, heap_node *b);
	heap_node *heap;   //堆
} heap;

void heap_dump(const heap *h);
heap *heap_init(heap *h, uint32_t capacity, heapType type, int transpose);
heap *heap_clean(heap *h);
int heap_empty(heap *h);
int heap_insert(heap *h, heap_node *node);
int heap_top_try_merge_equalnode(heap *h);
heap_node *heap_top(heap *h);
int heap_remove(heap *h);
int heap_test();
int heap_node_compare(heap_node *a, heap_node *b);
int heap_node_compare_transpose(heap_node *a, heap_node *b);
#endif