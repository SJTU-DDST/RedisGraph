#ifndef _MERGE_H
#define _MERGE_H
#include "coo_matrix_dpu.h"
#include "gb_matrix_slice_dpu.h"
#include "heap_wram.h"
#include "global.h"
void merge_coo_dpu_set(linear_mram_allocator *allocator, heap *h, coo_matrix_dpu_set *coo_set);
void heap_merge(heap *h, coo_matrix_dpu_iterator *iterator_set, uint32_t iterator_set_size, coo_matrix_dpu result);
#endif