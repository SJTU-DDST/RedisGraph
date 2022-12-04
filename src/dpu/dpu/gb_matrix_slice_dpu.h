#ifndef __GB_MATRIXSLICE_DPU_H
#define __GB_MATRIXSLICE_DPU_H
#include "../util/gb_matrix_slice.h"
#include "mram_alloc.h"
#include <mram.h>

typedef struct _gb_matrix_slice_dpu
{
    grb_type type;   // the type of each numerical entry
    uint32_t is_csc; // true if stored by column, false if by row
    uint32_t sparsity_control;
    MatrixID matrix_id;

    uint32_t h_start;
    uint32_t h_size;  // exact size of A->h
    uint32_t p_start;
    uint32_t p_size;  // exact size of A->p
    uint32_t ix_start;
    uint32_t ix_size; // exact size of indices and values

    /* data */
    __mram_ptr int32_t *h; // pointers: p_size >= 8*(plen+1)
    __mram_ptr int32_t *p; // pointers: p_size >= 8*(plen+1)
    __mram_ptr int32_t *ix;    // [(indices,values),(indices,values),......]
} gb_matrix_slice_dpu;
void gb_sparse_matrix_slice_dpu_dump(const gb_matrix_slice_dpu* gbsd);
#endif