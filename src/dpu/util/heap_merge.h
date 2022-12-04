#ifndef __HEAP_MERGE_H
#define __HEAP_MERGE_H
#include "heap.h"
#include "coo_matrix.h"
void coo_merge(heap *h, coo_matrix_iterator  *iterator_set, uint32_t iterator_set_size, coo_matrix result);
void coo_merge_test();
#endif