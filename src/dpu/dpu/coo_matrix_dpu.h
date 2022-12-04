#ifndef __COO_Matrix_DPU_H
#define __COO_Matrix_DPU_H
#include <mram.h>
#include <alloc.h>
#include "linear_mram_alloc.h"
#include "../util/coo_matrix.h"

typedef __mram_ptr struct _coo_matrix *coo_matrix_dpu;

typedef struct _coo_matrix_dpu_set
{
    uint32_t size;
    uint32_t capacity;
    __mram_ptr coo_matrix_dpu *coo_set;
} coo_matrix_dpu_set;

typedef struct _coo_matrix_dpu_iterator
{
    uint32_t index;
    coo_matrix_dpu coo;
} coo_matrix_dpu_iterator;

// coo_matrix_dpu api
coo_matrix_dpu coo_matrix_dpu_create(linear_mram_allocator *allocator, CooType type, uint32_t capacity);
int coo_matrix_dpu_insert(coo_matrix_dpu coo, uint32_t row, uint32_t col);
int coo_matrix_dpu_insert_elem(coo_matrix_dpu coo, coo_matrix_elem *elem);
void coo_matrix_dpu_dump(const coo_matrix_dpu coo);

// coo_matrix_dpu_iterator api
coo_matrix_dpu_iterator *coo_matrix_dpu_iterator_init(coo_matrix_dpu_iterator *it, coo_matrix_dpu coo);
__mram_ptr coo_matrix_elem *coo_matrix_dpu_next_elem(coo_matrix_dpu_iterator *it);

// coo_matrix_dpu_set api
coo_matrix_dpu_set *coo_matrix_dpu_set_init(linear_mram_allocator *allocator, coo_matrix_dpu_set *set, uint32_t capacity);
int coo_matrix_dpu_set_insert(coo_matrix_dpu_set *set, coo_matrix_dpu coo);
int coo_matrix_dpu_set_copy(coo_matrix_dpu_set *set_dst, coo_matrix_dpu_set *set_src);
int coo_matrix_dpu_set_clear(linear_mram_allocator *allocator, coo_matrix_dpu_set *set);
void coo_matrix_dpu_set_dump(const coo_matrix_dpu_set *set);
#endif