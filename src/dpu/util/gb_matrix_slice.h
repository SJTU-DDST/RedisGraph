#ifndef __GB_MATRIXSLICE_H
#define __GB_MATRIXSLICE_H
#include "gb_matrix.h"

struct _gb_matrix_slice
{
    grb_type type;   // the type of each numerical entry
    uint32_t is_csc; // true if stored by column, false if by row
    uint32_t sparsity_control;
    MatrixID matrix_id;

    uint32_t h_start;
    uint32_t h_size; // exact size of A->h
    uint32_t p_start;
    uint32_t p_size; // exact size of A->p
    uint32_t ix_start;
    uint32_t ix_size; // exact size of indices and values

    /* data */
    int32_t *h;  // pointers: p_size >= 8*(plen+1)
    int32_t *p;  // pointers: p_size >= 8*(plen+1)
    int32_t *ix; // [(indices,values),(indices,values),......]
};

typedef struct _gb_matrix_slice_head
{
    grb_type type;   // the type of each numerical entry
    uint32_t is_csc; // true if stored by column, false if by row
    uint32_t sparsity_control;
    MatrixID matrix_id;

    uint32_t h_start;
    uint32_t h_size; // exact size of A->h
    uint32_t p_start;
    uint32_t p_size; // exact size of A->p
    uint32_t ix_start;
    uint32_t ix_size; // exact size of indices and values
} gb_matrix_slice_head;

typedef struct _gb_matrix_slice *gb_matrix_slice;

void gb_matrix_slice_head_dump(gb_matrix_slice_head *gbsh);
void gb_matrix_slice_dump(gb_matrix_slice gbs);

#endif