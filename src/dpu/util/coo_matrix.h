#ifndef __COO_MATRIX_H
#define __COO_MATRIX_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "type.h"
#include "heap.h"

#define COO_MATRIX_OK 0
#define COO_MATRIX_ERR 1
typedef uint32_t CooType;
#define coo_elem_size(type) (sizeof(int32_t)*2)

#define DEFAULT_COO_CAPACITY 1024

typedef struct _coo_matrix_elem
{
    int32_t row;
    int32_t col;
} coo_matrix_elem;

struct _coo_matrix
{
    CooType type;
    uint32_t nnz;
    uint32_t capacity;
    uint32_t nonsense;      // for data align 8
    coo_matrix_elem data[]; // [(i,j,value),(i,j,value),....]
};

typedef struct _coo_matrix *coo_matrix;

typedef struct _coo_matrix_iterator
{
    uint32_t index;
    coo_matrix coo;
} coo_matrix_iterator;

coo_matrix coo_matrix_create(CooType type, uint32_t capacity);
int coo_matrix_insert(coo_matrix coo, int32_t row, int32_t col);
coo_matrix_iterator  *coo_matrix_iterator_init(coo_matrix_iterator  *it, coo_matrix coo);
coo_matrix_elem *coo_matrix_next_elem(coo_matrix_iterator  *it);
void coo_matrix_dump(const coo_matrix coo);
void coo_matrix_test();
coo_matrix coo_matrix_merge(const coo_matrix *coo_results, uint32_t num, int transpose);

#endif